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

# grab all sources with requested frequency; widen frequency window if we don't get enough

lowf   = 3.000e-3
window = 0.015e-3
oldwindow = window

oldcount = None

while True:
    gfile = open('Galaxy/Galaxy_%s.dat' % seed,'r')

    goodsources = []
    for line in gfile:
        pars = map(float,line.split())
    
        if pars[0] > lowf and pars[0] < (lowf + window):
            while pars[0] > lowf + oldwindow:
                pars[0] -= oldwindow

            goodsources.append(pars)

    gfile.close()

    if len(goodsources) > 60:
        break
    else:
        if len(goodsources) == oldcount:
            print "Cannot find enough galactic binaries of the right frequencies!"
            sys.exit(1)
        else:
            oldcount = len(goodsources)
            
            print "----> Can't find enough binaries, doubling frequency window"
            window *= 2

# choose between 40 and 60 sources

howmany = random.randint(40,60)
chosensources = []

for i in range(howmany):
    ind = random.randint(0,len(goodsources)-1)

    chosensources.append(goodsources[ind])
    del(goodsources[ind])

# now write them to disk!

written = 0
binaryseed = seed + 1

for source in chosensources:
    (freq, lat, lon, amp) = source[0:4]

    run('%(mydir)s/makesource-GalacticBinary.py --seed=%(binaryseed)s --amplitude=%(amp)s --latitude=%(lat)s --longitude=%(lon)s --centerf=%(freq)s --deltaf=0.0 --sourceName="GB #%(written)s" Source/GB1.1.4-%(written)s.xml' % globals())

    written += 1
    binaryseed += 1

run('rm ../MLDCwaveforms/Galaxy/Data/Galaxy_%(seed)s.dat ../MLDCwaveforms/Galaxy/Data/Galaxy_Bright_%(seed)s.dat ../MLDCwaveforms/Galaxy/Data/count_%(seed)s.dat')
run('rm Galaxy/*.xml Galaxy/*.dat')
