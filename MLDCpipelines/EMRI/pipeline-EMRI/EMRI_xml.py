#!/usr/bin/env python

import sys, getopt
from synthlisa import *
from Numeric import *
import lisawp_emri

import math
import random


def usage():

  msg = """\

 Usage: EMRI_xml [options]

 -h, --help        dislpay this message
 -s, --seed        seed for the random number generator
 -d, --distance    distance at which source to be placed in pc: [1e9(loud), 4e9(quite)]/(2years of data)
 -s, --sourceType  should be "loud" or "quiet", specifies whether we use 
                   narrow or not parameter range
 -t, --time    	   observation time (years), must be more than 1 year
 -c, --cadence     cadence (1/"sampling rate"), choose it so that together with time, the 
                   total number of points is power of 2 and 2/cadence >= max. frequency of
		   GW signal. Note, 1 year = 2^21 * 15 (sec).
 -f, --outXMLfile  name of the output xml file

"""
  print >> sys.stderr, msg

#### Parse the command line options ####

shortop = "hs:d:s:t:c:f:"
longop = [
       "help",
       "seed=",
       "distance=",
       "sourceType=",
       "time=",
       "cadence=",
       "outXMLfile="]

try:
  opts, args = getopt.getopt(sys.argv[1:], shortop, longop)
except getopt.GetoptError:
  usage()
  sys.exit(1)

seed = 0
D = 0.0
duration = 0.0
dt = 0.0
outXMLfile = None
sType = None

for o,a in opts:
	if o in ("-h", "--help"):
		usage()
		sys.exit(0)
	elif o in ("-s", "--seed"):
		seed = int(a)
	elif o in ("-d", "--distance"):
		D = float(a)
	elif o in ("-c", "--cadence"):
		dt = float(a)
	elif o in ("-s", "--sourceType"):
		sType = a
	elif o in ("-t", "--time"):
		duration = float(a)
        elif o in ("-f", "--outXMLfile"):
		outXMLfile = a


if not seed:
	print >> sys.stderr, "You must specify seed"
	usage()
	sys.exit(1)
if (D == 0.0):
	print >> sys.stderr, "You must specify distance"
	usage()
	sys.exit(1)
if (duration == 0.0):
	print >> sys.stderr, "You must specify duration"
	usage()
	sys.exit(1)
if (dt == 0.0):
	print >> sys.stderr, "You must specify cadence"
	usage()
	sys.exit(1)
if ( not outXMLfile):
	print >> sys.stderr, "You must specify the output file!"
	usage()
	sys.exit(1)
if (sType not in ("loud", "quiet")):
	print >> sys.stderr, "Specify source type: loud or quiet"
	usage()
	sys.exit(1)
	

#dt = 15.0 
oneyear = 2**21*15.0
random.seed(seed)
dist = D 
timeoffset = -900.0

if (sType == "loud" ):
   mu = random.uniform(5.0, 15.0) 
   print "Loud GW signal: using broad parameter range"
else:
   mu = random.uniform(9.5, 10.5) 
   print "Quiet GW signal: using narrow parameter range"
if (sType == "loud" ):
   MBHmass = 1.0e6*random.uniform(0.5,2.0)    
else:
   MBHmass = 1.0e6*random.uniform(0.95,1.05)  
if (sType == "loud" ):
   spin = random.uniform(0.2, 0.8)
else:
   spin = random.uniform(0.5, 0.7)
   
gamma0 = random.uniform(0.0, math.pi*2.0)
al0 = random.uniform(0.0, math.pi*2.0);   
lam = math.acos(random.uniform(-1.0, 1.0))            
e_lso = random.uniform(0.15, 0.25)                #estimated eccentricity at plunge

elat = math.acos(random.uniform(-1.0, 1.0))           #thetaS
elon = random.uniform(0.0, math.pi*2.0)   	      #phiS
thK = math.acos(random.uniform(-1.0, 1.0))            #thetaK
phK = random.uniform(0.0, math.pi*2.0)    #phiK
pol = random.uniform(0.0, math.pi*2.0)    #polarization
Phi0 = random.uniform(0.0, math.pi*2.0)   #initial orbital phase
Tend = random.uniform(1.6, 1.8)          #estimated Tend            
Tend = Tend*oneyear
duration = Tend - timeoffset

nu_lso =  math.pow((1.0-e_lso*e_lso)/(6.0+2.0*e_lso), 1.5)/(2.0*math.pi*MBHmass*4.92549095e-6)
#ein = lisawp_emri.floatp()
#nuin = lisawp_emri.floatp()
#e_in = ein.value()
#nu_in = nuin.value()

[e_in, nu_in] = lisawp_emri.EMRIEstimateInitialOrbit(spin,mu,MBHmass,Tend,e_lso,nu_lso)

print "Estimated eccentricity at plunge %f" % e_lso
print "Estimated frequency at plunge %f (Hz)" % nu_lso
print "initial eccentricity %f" % e_in
print "initial orb. freq %f (Hz)" % nu_in
print "expected duration %f (years)" % (Tend/oneyear)
print 40*"-*-"

print "Computing waveform...."


mysystem = lisawp_emri.EMRI(spin,e_in,mu,MBHmass,Phi0,dist,gamma0,al0,lam,\
           elat,elon,pol,thK,phK, nu_in, duration, timeoffset,dt)

print "source parameters will be recorded to ", outXMLfile
outputXML = lisaXML(outXMLfile,author='Stas Babak')
outputXML.SourceData(mysystem,name='Extreme Mass Ratio Inspiral')
outputXML.close()
