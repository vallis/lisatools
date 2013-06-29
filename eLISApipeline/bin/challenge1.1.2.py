#!/usr/bin/env python

import os
import sys
import random
import numpy

##### Stas:  Here we will use ONLY verificatin binaries created somewhere else !!!!

def run(command,quiet = False):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        if not quiet:
            assert(os.system(commandline) == 0)
        else:
            assert(os.system(commandline + ' > /dev/null') == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

seed = int(sys.argv[1])

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

print "Stas: mydir = ", mydir

#if not os.path.exists("Galaxy"):
#   print "Cannot find Galaxy folder in ", mydir
#   sys.exit(1)

verfile = "Galaxy/Verification.dat"
if not os.path.isfile(verfile):
   print "cannot find the file:",  verfile
   sys.exit(1)

# then the desired Galaxy file is in Galaxy/Galaxy_{Seed}.dat
# the order of the parameters is 0 -> Frequency
#                                1 -> Freq deriv 
#                                2 -> EclipticLatitude
#                                3 -> EclipticLongitude
#                                4 -> Amplitude
#                                5 -> Inclination
#                                6 -> Polarization
#                                7 -> InitialPhase

gfile = open('Galaxy/Verification.dat', 'r')

written = 0
binaryseed = seed + 1

for line in gfile:
    pars = map(float,line.split())
    
    (freq, freqdot, lat, lon, amp, inc, pol, inph) = pars[:]

    swritten = '%02d' % written
    run('%(mydir)s/makesource-GalacticBinary.py --seed=%(binaryseed)s --amplitude=%(amp)s --latitude=%(lat)s --longitude=%(lon)s --centerf=%(freq)s --deltaf=0.0  --centerdotf=%(freqdot)s  --deltadotf=0.0 --inclination=%(inc)s --polarization=%(pol)s --initialPhase=%(inph)s  --sourceName="Verification binary #%(swritten)s" Source/GB1.1.2-%(swritten)s.xml' % globals())

    written += 1
    binaryseed += 1
    
gfile.close()
