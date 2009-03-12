#!/usr/bin/env python

import os
import sys
import random
import math
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

run('%s/makesource-Galaxy2.py -v -s %s Galaxy/Galaxy_%s.xml' % (mydir,seed,seed))

# then the desired Galaxy file is in Galaxy/Galaxy_{Seed}.dat
# the order of the parameters is 0 -> Frequency
#                                1 -> EclipticLatitude
#                                2 -> EclipticLongitude
#                                3 -> Amplitude
#                                4 -> Inclination
#                                5 -> Polarization
#                                6 -> InitialPhase

# count the lines first...
gfile = open('Galaxy/Galaxy_%s.dat' % seed,'r')

sourcecount = 0
for line in gfile:
    sourcecount += 1

# choose 20 indices out of sourcecount; skip the verification binaries

whichbinaries = []

while True:
    ind = random.randint(26,sourcecount - 1)
    if ind not in whichbinaries:
        whichbinaries.append(ind)
    if len(whichbinaries) == 20:
        break

# reopen the file...
gfile.close()
gfile = open('Galaxy/Galaxy_%s.dat' % seed,'r')

linenum = 0
written = 0
binaryseed = seed + 1

# for 1.1.4 and 1.1.5, cannot count on the sourcefile being sorted...
# will have to load the binaries that fit the criteria, and work from there

for line in gfile:
    if linenum in whichbinaries:
        print "----> Using line %s" % linenum
        pars = map(float,line.split())
    
        (freq, lat, lon, amp) = pars[0:4]

        snr = math.e**random.uniform(0,math.log(40))
        
        swritten = '%02d' % written
        run('%(mydir)s/makesource-GalacticBinary.py --seed=%(binaryseed)s --requestSN=%(snr)s --latitude=%(lat)s --longitude=%(lon)s --centerf=%(freq)s --deltaf=0.0 --sourceName="GB #%(swritten)s" Source/GB1.1.3-%(swritten)s.xml' % globals())

        written += 1
        binaryseed += 1
    
    # if written all, drop out
    if written == 20:
        break
    
    linenum += 1

run('rm Galaxy/*.xml Galaxy/*.dat')
