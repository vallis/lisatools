#!/usr/bin/env python

__version__='$Id: $'

# keep it clean!

import lisaxml2

import numpy

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

parser.add_option("-a", "--deltaamplitude",
                  type="float", dest="deltaA", default=0.0,
                  help="half width of uniform probability distribution for amplitude [defaults to zero]")

parser.add_option("-N", "--sourcenumber",
                  type="int", dest="N", default=1,
                  help="number of sources [defaults to one]")

parser.add_option("-f", "--centerf",
                  type="float", dest="centerf", default=None,
                  help="frequency (Hz) [required]")

parser.add_option("-g", "--deltaf",
                  type="float", dest="deltaf", default=None,
                  help="half width of uniform probability distribution for frequency (Hz) [required]")

parser.add_option("-d", "--centerdotf",
                  type="float", dest="centerdotf", default=None,
                  help="frequency derivative (Hz/s) [optional, defaults to zero]")
                  
parser.add_option("-e", "--deltadotf",
                  type="float", dest="deltadotf", default=None,
                  help="half width of uniform probability distribution for frequency derivative (Hz/s) [optional]")

parser.add_option("-n", "--galaxyName",
                  type="string", dest="galaxyName", default="small Galaxy",
                  help='name of sourcetable [defaults to "small Galaxy"]')

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

if options.centerdotf != None and options.deltadotf == None:
    parser.error("If you specify the frequency derivative, you must give me its width!")

# get the name of the output file

outXMLfile = args[0]

# seed the random number generator

random.seed(options.seed)

binaries = []

for index in range(options.N):
    newbinary = []
    
    newbinary.append(options.centerf + random.uniform(-options.deltaf,options.deltaf))              # Frequency
    
    if options.centerdotf != None:
        newbinary.append(options.centerdotf + random.uniform(-options.deltadotf,options.deltadotf)) # FrequencyDerivative
    
    newbinary.append(0.5*math.pi - math.acos(random.uniform(-1.0,1.0)))             # EclipticLatitude
    newbinary.append(random.uniform(0.0,2.0*math.pi))                               # EclipticLongitude
    
    newbinary.append(options.A + random.uniform(-options.deltaA,options.deltaA))    # Amplitude
    
    newbinary.append(math.acos(random.uniform(-1.0,1.0)))                           # Inclination
    newbinary.append(random.uniform(0.0,2.0*math.pi))                               # Polarization
    newbinary.append(random.uniform(0.0,2.0*math.pi))                               # InitialPhase
    
    binaries.append(newbinary)

# prepare a lisaXML table structure with the binary parameters we've just generated

table = lisaxml2.Table(numpy.array(binaries,'d'),len(binaries),len(binaries[0]),
                                name=options.galaxyName,filetype='Remote',encoding='Text')

# set up the Column names

table.Frequency = None; table.Frequency_Unit = 'Hz'

if options.centerdotf != None:
    table.FrequencyDerivative = None; table.FrequencyDerivative_Unit = 'Hz/Second'

table.EclipticLatitude  = None; table.EclipticLatitude_Unit  = 'Radian'
table.EclipticLongitude = None; table.EclipticLongitude_Unit = 'Radian'
table.Amplitude         = None; table.Amplitude              = '1'
table.Inclination       = None; table.Inclination            = 'Radian'
table.Polarization      = None; table.Polarization           = 'Radian'
table.InitialPhase      = None; table.InitialPhase           = 'Radian'

# make a lisaXML SourceTable object

mygalaxy = lisaxml2.SourceTable('GalacticBinary',name=options.galaxyName)

mygalaxy.__dict__['Table'] = table

# write out the XML file

outputXML = lisaxml2.lisaXML(outXMLfile,author='M. Vallisneri')
outputXML.SourceData.append(mygalaxy)
outputXML.close()
