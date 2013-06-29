#!/usr/bin/env python

__version__='$Id: makebarycentric.py 1092 2009-10-27 02:24:53Z m.vallis@gmail.com $'

import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import re
import math
import string

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] SOURCEDEF.xml BARYCENTRIC.xml",
                      version="$Id: makebarycentric.py 1092 2009-10-27 02:24:53Z m.vallis@gmail.com $")

parser.add_option("-b", "--preBuffer",
                  type="float", dest="prebuffer", default=900.0,
                  help="length of data computed before initial time (s) [default 900.0]")

parser.add_option("-p", "--postBuffer",
                  type="float", dest="postbuffer", default=900.0,
                  help="length of data computed after final time (s) [default 900.0]")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for waveform, not counting prebuffer (s) [default 0.0]")
                                    
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 62914560 = 2^22 * 15; another popular choice 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

parser.add_option("-D", "--debug",
                  action="store_true", dest="debug", default=False,
                  help="compute debugging time series for EMRIs [off by default]")

parser.add_option("-P", "--poldebug",
                  action="store_true", dest="poldebug", default=False,
                  help="use SL polarization convention for EMRIs [off by default]")

parser.add_option("-s", "--sourceName",
                  type="string", dest="sourcename", default=None,
                  help="select a source by name among those in the SOURCEDEF.xml file [default None]")

parser.add_option("-S", "--singleOutput",
                  action="store_true", dest="singleOutput", default=False,
                  help="write all barycentric time series to the same lisaXML file [off by default]")

(options, args) = parser.parse_args()

if options.duration < 10:
    options.duration = options.duration * 31457280

# currently we support only a single source parameter file

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

# cycle through all the sources in the file

inputXML = lisaxml.readXML(inputfile)

allsystems = inputXML.getLISASources()

if options.sourcename:
    chosensources = map(string.strip,options.sourcename.split(','))
    allsystems = [x for x in allsystems if x.name in chosensources]

if options.singleOutput or len(allsystems) == 1:
    outputXML = lisaxml.lisaXML(outputfile,author='Michele Vallisneri')

for cnt,mysystem in zip(range(len(allsystems)),allsystems):
    # print out parameters

    if options.verbose:
        for p in mysystem.parameters:
            print p, ':', mysystem.parstr(p)

    # we are going to override IntegrationStep, if defined by the source, with
    # our own options.timestep. In the future it may be nice to provide for
    # interpolation...

    if ( hasattr(mysystem,'IntegrationStep') and
        convertUnit(mysystem.IntegrationStep,mysystem.IntegrationStep_Unit,'Second') != (options.timestep,'Second') ):
        print >> sys.stderr, "Overriding source IntegrationStep (%s) with our own timestep (%s s)" % (mysystem.parstr('IntegrationStep'),
                                                                                                      options.timestep)

    initialtime = options.inittime - options.prebuffer
    samples = int( (options.duration + options.prebuffer + options.postbuffer) / options.timestep + 0.1 )

    # is this really necessary? FixHarmonics should appear as a default parameter
    if (mysystem.xmltype == 'ExtremeMassRatioInspiral') and (not hasattr(mysystem,'FixHarmonics')):
	    mysystem.FixHarmonics = 0 

    if options.poldebug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        (hp0,hc0) = mysystem.waveforms(samples,options.timestep,initialtime,debug=-1)
    else:
        (hp0,hc0) = mysystem.waveforms(samples,options.timestep,initialtime)

    if options.debug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        (Ap,Ac) = mysystem.waveforms(samples,options.timestep,initialtime,debug=1)
        (ga,al) = mysystem.waveforms(samples,options.timestep,initialtime,debug=2)
        (ps,pl) = mysystem.waveforms(samples,options.timestep,initialtime,debug=3)

    # crop the waveform if requested
    
    if hasattr(mysystem,'RequestTimeOffset') and hasattr(mysystem,'RequestDuration'):
        # the arrays hp0 and hc0 begin at i=0,t=-prebuffer, and go until i=samples,t=duration+postbuffer (excluded, a la C loops)
        # now we wish to crop to t=RequestTimeOffset-prebuffer until t=RequestTimeOffset+RequestDuration+postbuffer,
        # while aligning these times onto the grid; however we cannot go before the first sample, or after the last

        length = int((mysystem.RequestDuration + options.prebuffer + options.postbuffer) / options.timestep)

        if length > len(hp0):
            print "makebarycentric.py: RequestDuration is too large."
            sys.exit(1)

        # this will automatically include the prebuffering period
        initialindex = int(mysystem.RequestTimeOffset / options.timestep)
        finalindex   = initialindex + length
        
        if initialindex < 0:
            initialindex, finalindex = 0, length
        elif finalindex > len(hp0):
            initialindex, finalindex = len(hp0) - length, len(hp0)
        
        hp0 = hp0[initialindex:finalindex].copy()
        hc0 = hc0[initialindex:finalindex].copy()
        
        initialtime = options.timestep * initialindex - options.prebuffer

        # reset RequestOffset and RequestDuration to be aligned on the grid, but don't include buffers        
        mysystem.RequestTimeOffset = initialtime + options.prebuffer
        mysystem.RequestDuration   = options.timestep * length - options.prebuffer - options.postbuffer
    
    # impose polarization on waveform if given
    
    print "^^^^^ Checking polarization" 
    if hasattr(mysystem,'Polarization') and (not hasattr(mysystem,'dopolarization') or not mysystem.dopolarization):
        pol = mysystem.Polarization
        print "Stas (makebarycentric): applying polarization"
        hp =  math.cos(2*pol) * hp0 + math.sin(2*pol) * hc0
        hc = -math.sin(2*pol) * hp0 + math.cos(2*pol) * hc0
    else:
        hp = hp0
        hc = hc0
    
    if options.debug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        mysystem.TimeSeries = lisaxml.TimeSeries((hp,hc,Ap,Ac,ga,al,ps,pl),'hp,hc,Ap,Ac,gamma,alpha,psi,psisl')
    else:
        mysystem.TimeSeries = lisaxml.TimeSeries((hp,hc),'hp,hc')
    
    mysystem.TimeSeries.Cadence = options.timestep
    mysystem.TimeSeries.TimeOffset = initialtime
    
    if options.singleOutput or len(allsystems) == 1:
        outputXML.SourceData(mysystem)
    else:
        if options.sourcename:
            filestr = mysystem.name
        else:
            filestr = '%d' % cnt
        
        if '-barycentric.xml' in outputfile:
            oneoutputfile = re.sub('-barycentric\.xml','-%s-barycentric.xml' % filestr,outputfile)
        elif '.xml' in outputfile:
            oneoutputfile = re.sub('\.xml','-%s.xml' % filestr,outputfile)
        else:
            oneoutputfile = outputfile + ('-%s' % filestr)
        
        oneoutputXML = lisaxml.lisaXML(oneoutputfile,author='Michele Vallisneri')
        oneoutputXML.SourceData(mysystem)
        oneoutputXML.close()

if options.singleOutput or len(allsystems) == 1:
    outputXML.close()

inputXML.close()
