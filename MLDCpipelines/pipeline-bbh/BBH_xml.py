#!/usr/bin/env python

import sys, getopt
from synthlisa import *
from Numeric import *
import lisawp

import math
import random


def usage():
    print >> sys.stderr, """
 Usage: BBH_xml [options]

 -h, --help        display this message
 -s, --seed        seed for random number generator
 -d, --distance    distance (pc) at which source is to be placed
                   (recommended 1.e10) 
 -t, --coalesTime  instance of colescence from t=0 in days
 -f, --outXMLfile  name of the output xml file

"""
  
#### Parse the command line options ####

shortop = "hs:d:t:f:"
longop = [
       "help",
       "seed=",
       "distance=",
       "coalesTime=",
       "outXMLfile="]

try:
    opts, args = getopt.getopt(sys.argv[1:], shortop, longop)
except getopt.GetoptError:
    usage()
    sys.exit(1)

seed = 0
D = 0.0
Tc = 0.0
outXMLfile = None

for o,a in opts:
	if o in ("-h", "--help"):
		usage()
		sys.exit(0)
	elif o in ("-s", "--seed"):
		seed = int(a)
	elif o in ("-d", "--distance"):
		D = float(a)
	elif o in ("-t", "--coalesTime"):
		Tc = float(a)
	elif o in ("-f", "--outXMLfile"):
		outXMLfile = a

if not seed:
	print >> sys.stderr, "You must specify the seed!"
	usage()
	sys.exit(1)
if (D == 0.0):
	print >> sys.stderr, "You must specify the distance!"
	usage()
	sys.exit(1)
if (Tc == 0.0):
	print >> sys.stderr, "You must specify coalescence time"
	usage()
	sys.exit(1)
if ( not outXMLfile):
	print >> sys.stderr, "You must specify the output file!"
	usage()
	sys.exit(1)
	

random.seed(seed)
taper = 8.0
dt = 15.0 # time step seconds
truncateTime = 0.0
duration = 31457280.0


m1   = 1.e6*random.uniform(1.0, 5.0) 
m2   = m1/random.uniform(1.0,4.0)    
cS   = random.uniform(-1.0, 1.0)           # cos(thetaS)
phS  = random.uniform(0.0, math.pi*2.0);   # phiS
ci   = random.uniform(-1.0, 1.0);          # cos(inclination)
psi  = random.uniform(0.0, math.pi*2.0);   # polarization
Phi0 = random.uniform(0.0, math.pi*2.0);   # initial orbital phase
u    = random.uniform(-20., 20.);
tc   = (Tc + u)*24.*3600.;                 # coalescence time in sec

#print "otput file = ", outXMLfile
#print "TC =  %f (days)" % Tc
#print "distance = ", D
#print "seed = ", seed


# estimate initial orbital angular freq
omega0 = lisawp.PNBinaryInitialFrequency(m1, m2, tc) # initial orb. ang. freq.
print "Initial GW freq = ", omega0/math.pi
f0 = omega0/math.pi    # initial freq of GW signal

timeoffset = -900
theta = math.acos(ci)
elat = math.acos(cS)
elon = phS
pol = psi
mysystem = lisawp.PNBinary(m1,m2,omega0,Phi0,D,theta,elat,elon,pol,
                           timeoffset,dt,truncateTime,taper,duration)

outputXML = lisaXML(outXMLfile,author='Stas Babak')
outputXML.SourceData(mysystem,name='2PN BBH binary')
outputXML.close()
