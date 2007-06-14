#!/usr/bin/env python

__version__='$Id: $'

# keep it clean!

import lisaxml
import GalacticBinary

import sys
import getopt
import math
import random

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: $")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-A", "--amplitude",
                  type="float", dest="A", default=1.0e-21,
                  help="amplitude (adimensional)")

parser.add_option("-S", "--requestSN",
                  type="float", dest="RequestSN", default=None,
                  help="requested source amplitude SN (satisfied at TDI-generation time)")

parser.add_option("-b", "--latitude",
                type="float", dest="latitude", default=None,
                help="requested source latitude (rad) [will be randomized if not given]")

parser.add_option("-l", "--longitude",
                type="float", dest="longitude", default=None,
                help="requested source longitude (rad) [will be randomized if not given]")

parser.add_option("-f", "--centerf",
                  type="float", dest="centerf", default=None,
                  help="frequency (Hz) [required]")

parser.add_option("-g", "--deltaf",
                  type="float", dest="deltaf", default=None,
                  help="half width of uniform probability distribution for frequency (Hz) [required]")

parser.add_option("-n", "--sourceName",
                  type="string", dest="sourceName", default="monochromatic Galactic binary",
                  help='name of source [defaults to "monochromatic Galactic binary"]')

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
if options.centerf == None:
    parser.error("You must specify the frequency!")

if options.deltaf == None:
    parser.error("You must specify the frequency width!")

# get the name of the output file

outXMLfile = args[0]

# seed the random number generator

random.seed(options.seed)

mysystem = GalacticBinary.GalacticBinary('GalacticBinary')

mysystem.Frequency         = options.centerf + random.uniform(-options.deltaf,options.deltaf)

if options.latitude == None:
    mysystem.EclipticLatitude  = 0.5*math.pi - math.acos(random.uniform(-1.0,1.0))
else:
    mysystem.EclipticLatitude = options.latitude
    
if options.longitude == None:    
    mysystem.EclipticLongitude = random.uniform(0.0,2.0*math.pi)
else:
    mysystem.EclipticLongitude = options.longitude

mysystem.Amplitude         = options.A
mysystem.Inclination       = math.acos(random.uniform(-1.0,1.0))
mysystem.Polarization      = random.uniform(0.0,2.0*math.pi)
mysystem.InitialPhase      = random.uniform(0.0,2.0*math.pi)

if options.RequestSN:
    mysystem.RequestSN = options.RequestSN
    mysystem.RequestSN_Unit = '1'

if options.verbose:
    for p in mysystem.parameters:
        print p, ':', mysystem.parstr(p)

# write out the XML file

outputXML = lisaxml.lisaXML(outXMLfile,author='M. Vallisneri')
outputXML.SourceData(mysystem,name=options.sourceName)
outputXML.close()
