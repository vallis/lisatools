#!/usr/bin/env python

__version__='$Id$'

import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import re
import math

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] SOURCEDEF.xml BARYCENTRIC.xml",
                      version="$Id$")

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
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 62914560 = 2^22 * 15]")

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

(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

# cycle through all the sources in the file

inputXML = lisaxml.readXML(inputfile)

allsystems = inputXML.getLISASources()

for mysystem, cnt in zip(allsystems,range(len(allsystems))):
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

    if options.poldebug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        (hp0,hc0) = mysystem.waveforms(samples,options.timestep,initialtime,debug=-1)
    else:
        (hp0,hc0) = mysystem.waveforms(samples,options.timestep,initialtime)

    if options.debug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        (Ap,Ac) = mysystem.waveforms(samples,options.timestep,initialtime,debug=1)
        (ga,al) = mysystem.waveforms(samples,options.timestep,initialtime,debug=2)
        (ps,pl) = mysystem.waveforms(samples,options.timestep,initialtime,debug=3)

    # impose polarization on waveform

    pol = mysystem.Polarization

    hp =  math.cos(2*pol) * hp0 + math.sin(2*pol) * hc0
    hc = -math.sin(2*pol) * hp0 + math.cos(2*pol) * hc0

    if options.debug == True and mysystem.xmltype == 'ExtremeMassRatioInspiral':
        mysystem.TimeSeries = lisaxml.TimeSeries((hp,hc,Ap,Ac,ga,al,ps,pl),'hp,hc,Ap,Ac,gamma,alpha,psi,psisl')
    else:
        mysystem.TimeSeries = lisaxml.TimeSeries((hp,hc),'hp,hc')

    mysystem.TimeSeries.Cadence = options.timestep
    mysystem.TimeSeries.TimeOffset = initialtime

    if len(allsystems) == 1:
        oneoutputfile = outputfile
    else:
        if '-barycentric.xml' in outputfile:
            oneoutputfile = re.sub('-barycentric\.xml','-%d-barycentric.xml' % cnt,outputfile)
        elif '.xml' in outputfile:
            oneoutputfile = re.sub('\.xml','-%d.xml' % cnt,outputfile)
        else:
            oneoutputfile = outputfile + ('-%d' % cnt)

    outputXML = lisaxml.lisaXML(oneoutputfile,author='Michele Vallisneri')
    outputXML.SourceData(mysystem)
    outputXML.close()

inputXML.close()
