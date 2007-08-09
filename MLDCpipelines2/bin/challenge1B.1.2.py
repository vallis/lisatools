#!/usr/bin/env python

import os
import sys
import random
import numpy

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

run('%s/makesource-Galaxy.py %s' % (mydir,seed))
# then the desired Galaxy file is in Galaxy/Galaxy_{Seed}.dat
# the order of the parameters is 0 -> Frequency
#                                1 -> EclipticLatitude
#                                2 -> EclipticLongitude
#                                3 -> Amplitude
#                                4 -> Inclination
#                                5 -> Polarization
#                                6 -> InitialPhase

gfile = open('Galaxy/Galaxy_%s.dat' % seed)

written = 0
binaryseed = seed + 1

for line in gfile:
    pars = map(float,line.split())
    
    (freq, lat, lon, amp) = pars[0:4]

    swritten = '%02d' % written
    run('%(mydir)s/makesource-GalacticBinary.py --seed=%(binaryseed)s --amplitude=%(amp)s --latitude=%(lat)s --longitude=%(lon)s --centerf=%(freq)s --deltaf=0.0 --sourceName="Verification binary #%(swritten)s" Source/GB1.1.2-%(swritten)s.xml' % globals())

    written += 1
    binaryseed += 1
    
    # write 25 verification binaries
    if written == 25:
        break

run('rm ../MLDCwaveforms/Galaxy/Data/Galaxy_%(seed)s.dat ../MLDCwaveforms/Galaxy/Data/Galaxy_Bright_%(seed)s.dat ../MLDCwaveforms/Galaxy/Data/count_%(seed)s.dat')
run('rm Galaxy/*.xml Galaxy/*.dat')
