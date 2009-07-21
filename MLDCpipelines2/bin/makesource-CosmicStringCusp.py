#!/usr/bin/env python

__version__='$Id:  $'

# keep it clean!

import lisaxml
import CosmicStringCusp

import sys
import getopt
import math
import random

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: makesource-BBH.py 297 2007-01-16 21:39:25Z vallisneri $")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-A", "--amplitude",
                  type="float", dest="Amplitude", default=1.0e-21,
                  help="amplitude (Hertz^1/3) [default 1e-21]")

parser.add_option("-S", "--requestSN",
                  type="float", dest="RequestSN", default=None,
                  help="requested source amplitude SN (satisfied at TDI-generation time)")

parser.add_option("-t", "--centralTime",
                  type="float", dest="Tc", default=None,
                  help="central time of the burst from t = 0 (seconds) [required]")

parser.add_option("-r", "--centralTimeRange",
                  type="float", dest="deltaTc", default=None,
                  help="dispersion of the central time (seconds) [+- uniform range, required]")

parser.add_option("-m", "--minimumFrequency",
                  type="float", dest="MinimumFrequency", default=0,
                  help="low-frequency cutoff (Hz) [default is None]")

parser.add_option("-f", "--maximumFrequency",
                  type="float", dest="MaximumFrequency", default=None,
                  help="maximum burst frequency (Hz) [default random log-uniform 1e-3--10 Hz]")

parser.add_option("-T", "--requestTime",
                  type="float", dest="requestTime", default=None,
                  help="request that barycentric time series be stored only for this period around CentralTime (second) [off by defaults]")

parser.add_option("-n", "--sourceName",
                  type="string", dest="sourceName", default="Cosmic string cusp burst",
                  help='name of source [defaults to "Cosmic string cusp burst"]')

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
if options.Tc == None or options.deltaTc == None:
    parser.error("You must specify the central time and its randomization width!")

# get the name of the output file

outXMLfile = args[0]

# seed the random number generator

random.seed(options.seed)

# create the source object

mysystem = CosmicStringCusp.CosmicStringCusp('CosmicStringCusp')

# see list of standard parameters in relevant .i file (such as BBH.i)
# note that nonstandard units may be forced by setting attributes that end in _Unit

mysystem.EclipticLatitude           = 0.5*math.pi - math.acos(random.uniform(-1.0,1.0)) # ecliptic latitude in Radian [-pi,pi]
mysystem.EclipticLongitude          = random.uniform(0.0,2.0*math.pi)                   # ecliptic longitude in Radian [0,2*pi]
mysystem.Polarization               = random.uniform(0.0,2.0*math.pi)
mysystem.Amplitude                  = options.Amplitude

mysystem.CentralTime                = options.Tc + random.uniform(-options.deltaTc,options.deltaTc)

if options.MaximumFrequency != None:
    mysystem.MaximumFrequency = options.MaximumFrequency
else:
    mysystem.MaximumFrequency = 10**random.uniform(-3.0,-1.0)

mysystem.MinimumFrequency = options.MinimumFrequency

if options.RequestSN:
    mysystem.RequestSN = options.RequestSN
    mysystem.RequestSN_Unit = '1'

if options.requestTime:
    mysystem.RequestTimeOffset = int(mysystem.CentralTime - 0.5 * options.requestTime)
    mysystem.RequestTimeOffset_Unit = 'Second'
    
    mysystem.RequestDuration = options.requestTime
    mysystem.RequestDuration_Unit = 'Second'

if options.verbose:
    for p in mysystem.parameters:
        print p, ':', mysystem.parstr(p)

# write out the XML file

outputXML = lisaxml.lisaXML(outXMLfile,author='Michele Vallisneri')
outputXML.SourceData(mysystem,name=options.sourceName)
outputXML.close()
