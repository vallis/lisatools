#!/usr/bin/env python

__version__='$Id$'

import lisaxml
import EMRI  # should be changed to MLDC

import sys
import getopt
import math
import random

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id$")

# remember that default action is "store", default type is "string"
# actions "store_true" and "store_false" handle boolean options
# set default to None for required options

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-d", "--distance",
                  type="float", dest="D", default=1.0e9,
                  help="distance to source (Pc) [required]")

parser.add_option("-S", "--requestSN",
                  type="float", dest="RequestSN", default=None,
                  help="requested source amplitude SN (satisfied at TDI-generation time)")

parser.add_option("-M", "--massSMBH",
                  type="float", dest="massSMBH", default=1.0e6,
                  help="approximate mass of central black hole (defaults to 1.0e6)")

parser.add_option("-n", "--sourceName",
                  type="string", dest="sourceName", default="Analytical kludge EMRI",
                  help='name of source [defaults to "Analytical kludge EMRI"]')

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
# get the name of the output file

outXMLfile = args[0]

# seed the random number generator

random.seed(options.seed)

mysystem = EMRI.ExtremeMassRatioInspiral("ExtremeMassRatioInspiral")

# see list of standard parameters in relevant .i file 
# note that nonstandard units may be forced by setting attributes that end in _Unit

# standard sky location, polarization, spin orientation, distance, spin orientation

mysystem.EclipticLatitude           = 0.5*math.pi - math.acos(random.uniform(-1.0,1.0)) # ecliptic latitude in Radian [-pi/2,pi/2]
mysystem.EclipticLongitude          = random.uniform(0.0,2.0*math.pi)                   # ecliptic longitude in Radian [0,2*pi]

# mysystem.Polarization               = random.uniform(0.0,2.0*math.pi)                   # polarization in Radian [0,2*pi]
mysystem.Polarization               = 0

mysystem.Distance                   = options.D                                         # distance in Parsec (from command-line parameter)
mysystem.PolarAngleOfSpin	        = math.acos(random.uniform(-1.0, 1.0))		        # polar angle of the spin in Radian [0,pi]
mysystem.AzimuthalAngleOfSpin	    = random.uniform(0.0, 2.0*math.pi)		            # polar angle of the spin in Radian [0,2*pi]

# source parameteres

mysystem.Spin			   	          = random.uniform(0.5, 0.7)				     # spin magnitude in M^2
mysystem.MassOfCompactObject          = random.uniform(9.5, 10.5)				     # mass of CO in solar mass
mysystem.MassOfSMBH		              = options.massSMBH * random.uniform(0.95,1.05) # mass of SMBH in solar mass

PhiAtPlunge             = random.uniform(0.0, math.pi*2.0)          # orbital azimuthal phase at plunge
GammaAtPlunge           = random.uniform(0.0, math.pi*2.0)          # gamma angle at plunge
AlphaAtPlunge           = random.uniform(0.0, math.pi*2.0)          # alpha angle at plunge
mysystem.LambdaAngle	= math.acos(random.uniform(-1.0, 1.0))      # angle between L and S


#mysystem.InitialAzimuthalOrbitalPhase = random.uniform(0.0, math.pi*2.0)			 
# initial azimuthal orbital phase in Rad
#mysystem.InitialTildeGamma 		      = random.uniform(0.0, math.pi*2.0)            
# initial position of pericenter, as angle between LxS and pericenter
#mysystem.InitialAlphaAngle            = random.uniform(0.0, math.pi*2.0)             
# initial azimuthal direction of L (in the orbital plane)

e_lso = random.uniform(0.15, 0.25)                           # estimated eccentricity at the plunge
Tend  = random.uniform(1.0, 2.0)*2**21*15.0                  # plungetime: between 1 and 2 years which is assumed to be 2**21*15 seconds

mysystem.IntegrationStep              = 15.0                                         # integration timestep in seconds
mysystem.IntegrationStep_Unit         = 'Second'

if options.RequestSN:
    mysystem.RequestSN = options.RequestSN
    mysystem.RequestSN_Unit = '1'

MBHmass = mysystem.MassOfSMBH
nu_lso =  math.pow((1.0-e_lso*e_lso)/(6.0+2.0*e_lso), 1.5)/(2.0*math.pi*MBHmass*4.92549095e-6)

#print "============== Chosen parameters ===================="
#print "e_lso = ", e_lso
#print "nu_lso = ", nu_lso
#print "phi_lso = ", PhiAtPlunge
#print "gamma_lso = ", GammaAtPlunge
#print "alpha_lso = ", AlphaAtPlunge
#print "Tend = ", Tend
#print 30*"="

# estimating initial (t=0) orbital elements
[nu_in, e_in, gam_in,
 phi_in, al_in] = EMRI.EMRIEstimateInitialOrbit(mysystem.Spin,mysystem.MassOfCompactObject,mysystem.MassOfSMBH,
                                                Tend,e_lso,GammaAtPlunge,PhiAtPlunge,AlphaAtPlunge,mysystem.LambdaAngle)

#print "============== Initial parameters ===================="
#print "e_in = ", e_in
#print "nu_in = ", nu_in
#print "phi_in = ", phi_in
#print "gamma_in = ", gam_in
#print "alpha_in = ", al_in
#print "Tend = ", Tend
#print 30*"="

mysystem.InitialAzimuthalOrbitalFrequency = nu_in   # initial azimuthal orbital frequecy in Hz
mysystem.InitialEccentricity		      = e_in    # initial orbital eccentricity
mysystem.InitialAzimuthalOrbitalPhase     = phi_in  # initial azimuthal orbital phase in Rad
mysystem.InitialTildeGamma 		          = gam_in  # initial position of pericenter, as angle between LxS and pericenter
mysystem.InitialAlphaAngle		          = al_in   # initial azimuthal direction of L (in the orbital plane)

# save also the final parameters (Units must be given for the parameters to be written to XML)

mysystem.PlungeTime                 = Tend         ; mysystem.PlungeTime_Unit                 = 'Second'                 
mysystem.FinalEccentricity          = e_in         ; mysystem.FinalEccentricity_Unit          = '1'
mysystem.FinalTildeGamma            = GammaAtPlunge; mysystem.FinalTildeGamma_Unit            = 'Radian'
mysystem.FinalAzimuthalOrbitalPhase = PhiAtPlunge  ; mysystem.FinalAzimuthalOrbitalPhase_Unit = 'Radian'
mysystem.FinalAlphaAngle            = AlphaAtPlunge; mysystem.FinalAlphaAngle_Unit            = 'Radian'

extraparcomment = """The parameters at plunge (PlungeTime, FinalEccentricity, FinalTildeGamma,
FinalAzimuthalOrbitalPhase, FinalAlphaAngle) are redundant and are provided
only for convenience in checking search codes against training sets."""

if options.verbose:
    for p in mysystem.parameters:
        print p, ':', mysystem.parstr(p)

# write out the XML file

outputXML = lisaxml.lisaXML(outXMLfile,author='Stas Babak')
outputXML.SourceData(mysystem,name=options.sourceName,comments=extraparcomment)
outputXML.close()
