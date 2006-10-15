#!/usr/bin/env python

# keep it clean!

import lisaxml
import BBH

import sys
import getopt
import math
import random

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: BBH_xml.py 87 2006-10-10 20:29:25Z vallisneri $")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-d", "--distance",
                  type="float", dest="D", default=None,
                  help="distance to source (Pc) [required]")

parser.add_option("-t", "--coalescTime",
                  type="float", dest="Tc", default=None,
                  help="time of coalescence from t = 0 (days) [required]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
if options.D == None:
    parser.error("You must specify the distance!")
    
if options.Tc == None:
    parser.error("You must specify the coalescence time!")

# get the name of the output file

outXMLfile = args[0]

# seed the random number generator

random.seed(options.seed)

# create the source object

mysystem = BBH.BlackHoleBinary('BlackHoleBinary')

# see list of standard parameters in relevant .i file (such as BBH.i)
# note that nonstandard units may be forced by setting attributes that end in _Unit

# standard sky location, polarization, inclination, distance, time offset

mysystem.EclipticLatitude           = 0.5*math.pi - math.acos(random.uniform(-1.0,1.0)) # ecliptic latitude in Radian [-pi,pi]
mysystem.EclipticLongitude          = random.uniform(0.0,2.0*math.pi)                   # ecliptic longitude in Radian [0,2*pi]
mysystem.Polarization               = random.uniform(0.0,2.0*math.pi)                   # polarization in Radian [0,2*pi]
mysystem.Inclination                = math.acos(random.uniform(-1.0,1.0))               # inclination in Radian (0,pi)
mysystem.Distance                   = options.D                                         # distance in Parsec (from command-line parameter)

# define physical parameters of the system

mysystem.Mass1                      = 1.0e6 * random.uniform(1.0,5.0)                   # m1 in SolarMass
mysystem.Mass2                      = mysystem.Mass1 / random.uniform(1.0,4.0)          # m2 in SolarMass
mysystem.CoalescenceTime            = (options.Tc + random.uniform(-20.0,20.0)) * 24 * 3600     # coalescence time in Second (from command-line parameter)
mysystem.InitialAngularOrbitalPhase = random.uniform(0.0,2.0*math.pi)                   # initial orbital phase in Radian

# integration parameters

mysystem.IntegrationStep            = 15.0                                              # integration timestep in seconds
mysystem.TruncationTime             = 0.0                                               # truncation time removed from end of waveform (???)
mysystem.TaperApplied               = 7.0                                               # tapering radius (total masses)

if options.verbose:
    for p in mysystem.parameters:
        print p, ':', mysystem.parstr(p)

# write out the XML file

outputXML = lisaxml.lisaXML(outXMLfile,author='Stas Babak')
outputXML.SourceData(mysystem,name='2PN BBH binary')
outputXML.close()
