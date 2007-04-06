#!/usr/bin/env python

__version__='$Id$'

import lisaxml
from lisaxml.convertunit import convertUnit

import sys
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
                  type="float", dest="duration", default=31457280.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 31457280 = 2^21 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

# work only with the first source of the first file

inputXML = lisaxml.readXML(inputfile)
mysystem = inputXML.getLISASources()[0]
inputXML.close()

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

(hp0,hc0) = mysystem.waveforms(samples,options.timestep,initialtime)

# impose polarization on waveform

pol = mysystem.Polarization

hp =  math.cos(2*pol) * hp0 + math.sin(2*pol) * hc0
hc = -math.sin(2*pol) * hp0 + math.cos(2*pol) * hc0

mysystem.TimeSeries = lisaxml.TimeSeries((hp,hc),'hp,hc')

mysystem.TimeSeries.Cadence = options.timestep
mysystem.TimeSeries.TimeOffset = initialtime

outputXML = lisaxml.lisaXML(outputfile,author='Michele Vallisneri')
outputXML.SourceData(mysystem)
outputXML.close()
