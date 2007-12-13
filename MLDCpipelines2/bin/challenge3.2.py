#!/usr/bin/env python

import os
import sys
import random

import numpy

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

# only one argument, and it's the seed...
seed = int(sys.argv[1])

random.seed(seed)

# the obligatory black holes...

# 1 - A loud (SNR ~ 2000) signal (with in-spiral "in band", that is from f = 0.1 mHz, lasting about 1 week) and coalescing 3 months +/- 1 month after the beginning of the observations
run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=2000 --coalescTime=90  --coalescRange=30 --sourceName="SMBH-1" Source/BH-1.xml' % (mydir,seed+1))

# 2 - A moderate-to-low SNR (~ 20) signal with coalescence between 25 and 26 months from the beginning of the observations
run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=20   --coalescTime=765 --coalescRange=15 --sourceName="SMBH-2" Source/BH-2.xml' % (mydir,seed+2))

# the optional black holes

chosen = random.sample([3,4,5,6],random.randint(2,4))

# 3 - A loud (SNR ~ 1000) signal coalescing between 6 and 24 months from beginning of observations
if 3 in chosen:
    run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=1000 --coalescTime=450 --coalescRange=270 --sourceName="SMBH-3" Source/BH-3.xml' % (mydir,seed+3))

# 4 - A loud (SNR ~ 200) signal coalescing between 6 and 24 months from beginning of observations
if 4 in chosen:
    run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=200  --coalescTime=450 --coalescRange=270 --sourceName="SMBH-4" Source/BH-4.xml' % (mydir,seed+4))

# 5 - A loud (SNR ~ 100) signal with coalescence taking place between 18 and 21 months from the start of the observations
if 5 in chosen:
    run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=100  --coalescTime=540 --coalescRange=45  --sourceName="SMBH-5" Source/BH-5.xml' % (mydir,seed+5))

# 6 - A low SNR (~ 10) signal with coalescence taking place between 27 and 28 months from the beginning of the observations
if 6 in chosen:
    run('%s/makesource-SpinBBH.py -F --seed=%s --distance=1e10 --requestSN=10   --coalescTime=825 --coalescRange=15  --sourceName="SMBH-6" Source/BH-6.xml' % (mydir,seed+6))

seed = seed + 7

run('%(mydir)s/makesource-Galaxy3.py --seed=%(seed)s --confusion -v Galaxy/TheGalaxy.xml' % globals())
