#!/usr/bin/env python

import lisaxml

import sys
import getopt
import math
import random

# set ourselves up to parse command-line options

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id$")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int)")

parser.add_option("-d", "--distance",
                  type="float", dest="D", default=None,
                  help="distance to source (Pc)")

parser.add_option("-t", "--coalescTime",
                  type="float", dest="Tc", default=None,
                  help="time of coalescence from t = 0 (days)")
                                
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

# later MLDC.BlackHoleBinary

class BlackHoleBinary(lisaxml.Source):
    outputlist = ( ('EclipticLatitude','Radian',None),
                   ('EclipticLongitude','Radian',None),
                   ('Polarization','Radian',None),
                   ('TimeOffset','Second',None),
                   ('Mass1','SolarMass',None),
                   ('Mass2','SolarMass',None),
                   ('CoalescenceTime','Second',None),
                   ('InitialAngularOrbitalPhase','Radian',None),
                   ('Distance','Parsec',None),
                   ('Inclination','Radian',None),
                   ('IntegrationStep','Second',None),
                   ('TruncationTime','Second','0'),
                   ('TaperApplied','TotalMass','7'),
                   ('MaxDuration','Second','31459080') )

    def __init__(self,name=''):
        super(BlackHoleBinary, self).__init__('BlackHoleBinary',name)

mysystem = BlackHoleBinary('BlackHoleBinary')

# standard sky location, polarization, inclination, distance, time offset

mysystem.EclipticLatitude           = 0.5*math.pi - math.acos(random.uniform(-1.0,1.0)) # ecliptic latitude in Radian [-pi,pi]
mysystem.EclipticLongitude          = random.uniform(0.0,2.0*math.pi)                   # ecliptic longitude in Radian [0,2*pi]
mysystem.Polarization               = random.uniform(0.0,2.0*math.pi)                   # polarization in Radian [0,2*pi]
mysystem.Inclination                = math.acos(random.uniform(-1.0,1.0))               # inclination in Radian (0,pi)
mysystem.Distance                   = options.D                                         # distance in Parsec (from command-line parameter)
mysystem.TimeOffset                 = -900.0                                            # time at beginning of GW-strain arrays

# define physical parameters of the system

mysystem.Mass1                      = 1.0e6 * random.uniform(1.0,5.0)                   # m1 in SolarMass
mysystem.Mass2                      = mysystem.Mass1 / random.uniform(1.0,4.0)          # m2 in SolarMass
mysystem.CoalescenceTime            = (options.Tc + random.uniform(-20.0,20.0)) * 24 * 3600     # coalescence time in Second (from command-line parameter)
mysystem.InitialAngularOrbitalPhase = random.uniform(0.0,2.0*math.pi)                   # initial orbital phase in Radian

# integration parameters

mysystem.IntegrationStep            = 15.0                                              # integration timestep in seconds
mysystem.TruncationTime             = 0.0                                               # truncation time removed from end of waveform
mysystem.TaperApplied               = 7.0                                               # tapering radius (total masses)
mysystem.MaxDuration                = (2**21)*15 - mysystem.TimeOffset                  # maximum length of computed waveform

# write out the XML file

outputXML = lisaxml.lisaXML(outXMLfile,author='Stas Babak')
outputXML.SourceData(mysystem,name='2PN BBH binary')
outputXML.close()
