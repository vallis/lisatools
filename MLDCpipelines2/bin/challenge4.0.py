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

# parse arguments

seed = int(sys.argv[1])
istraining = (sys.argv[2] == 'True')
nproc = int(sys.argv[3])

random.seed(seed)
numpy.random.seed(seed)

# --- DEFINITION OF THE CHALLENGE

# MBHs need better taper at the end to avoid hiccup; increase mass ratio to 1-10
# EMRIs could use taper, but is hidden in noise; have undisclosed number of EMRIs of each kind;
#   consider lowering plunge eccentricity (0.05), currently random.uniform(0.15, 0.25) 
# Neil: we produce two challenge 4 data sets. One with a full galaxy, and one
#   with a partially regressed galaxy (as we did for challenge 3.2).

# --- the galaxy!

run('%(mydir)s/makesource-Galaxy3.py --seed=%(seed)s -v Galaxy/TheGalaxy.xml' % globals())

seed = seed + 1

# --- black holes

# 1 - A loud (SNR ~ 2000) signal (with in-spiral "in band", that is from f = 0.1 mHz, lasting about 1 week) and coalescing 3 months +/- 1 month after the beginning of the observations
run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=2000 --coalescTime=90  --coalescRange=30 --massRatioRange=10 --sourceName="SMBH-1" Source/BH-1.xml' % globals())
seed = seed + 1

# 2 - A moderate-to-low SNR (~ 20) signal with coalescence between 25 and 26 months from the beginning of the observations
run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=20   --coalescTime=765 --coalescRange=15 --massRatioRange=10 --sourceName="SMBH-2" Source/BH-2.xml' % globals())
seed = seed + 1

chosen = random.sample([3,4,5,6],random.randint(2,4))

# 3 - A loud (SNR ~ 1000) signal coalescing between 6 and 24 months from beginning of observations
if 3 in chosen:
    run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=1000 --coalescTime=450 --coalescRange=270 --massRatioRange=10 --sourceName="SMBH-3" Source/BH-3.xml' % globals())
    seed = seed + 1
    
# 4 - A loud (SNR ~ 200) signal coalescing between 6 and 24 months from beginning of observations
if 4 in chosen:
    run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=200  --coalescTime=450 --coalescRange=270 --massRatioRange=10 --sourceName="SMBH-4" Source/BH-4.xml' % globals())
    seed = seed + 1

# 5 - A loud (SNR ~ 100) signal with coalescence taking place between 18 and 21 months from the start of the observations
if 5 in chosen:
    run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=100  --coalescTime=540 --coalescRange=45  --massRatioRange=10 --sourceName="SMBH-5" Source/BH-5.xml' % globals())
    seed = seed + 1

# 6 - A low SNR (~ 10) signal with coalescence taking place between 27 and 28 months from the beginning of the observations
if 6 in chosen:
    run('%(mydir)s/makesource-SpinBBH.py -F --seed=%(seed)s --distance=1e10 --requestSN=10   --coalescTime=825 --coalescRange=15  --massRatioRange=10 --sourceName="SMBH-6" Source/BH-6.xml' % globals())
    seed = seed + 1

# --- EMRIs!

for srcind in range(numpy.random.poisson(3)):
    requestsn = random.randint(25,50)
    run('%(mydir)s/makesource-EMRI.py -f --seed=%(seed)s --distance=1e9 --requestSN=%(requestsn)s --massSMBH=1.e7 --minEccentricity=0.05 --sourceName="EMRI-1-%(srcind)s" Source/EMRI-1-%(srcind)s.xml' % globals())
    seed = seed + 1

for srcind in range(numpy.random.poisson(2)):
    requestsn = random.randint(25,50)
    run('%(mydir)s/makesource-EMRI.py -f --seed=%(seed)s --distance=1e9 --requestSN=%(requestsn)s --massSMBH=5.e6 --minEccentricity=0.05 --sourceName="EMRI-2-%(srcind)s" Source/EMRI-2-%(srcind)s.xml' % globals())
    seed = seed + 1

for srcind in range(numpy.random.poisson(1)):
    requestsn = random.randint(25,50)
    run('%(mydir)s/makesource-EMRI.py -f --seed=%(seed)s --distance=1e9 --requestSN=%(requestsn)s --massSMBH=1.e6 --minEccentricity=0.05 --sourceName="EMRI-3-%(srcind)s" Source/EMRI-3-%(srcind)s.xml' % globals())
    seed = seed + 1

# --- cosmic strings

for srcind in range(numpy.random.poisson(20)):
    requestsn = random.randint(10,100)
    run('%(mydir)s/makesource-CosmicStringCusp.py --seed=%(seed)s -t 31457280 -r 31457280 -T 1966080 -S %(requestsn)s -n "CosmicString-%(srcind)02d" Source/Burst-%(srcind)02d' % globals())
    seed = seed + 1

# --- GWDB

pixelref = 2
run('%(mydir)s/makesource-StochasticBackground.py --seed=%(seed)s --PSD=1.0e-47 --randomizePSD=0.3 --pixelRefinement=%(pixelref)s --nProc=%(nproc)s Immediate/Background.xml' % globals())
run('%(mydir)s/makesource-StochasticBackground.py --seed=%(seed)s --PSD=1.0e-47 --randomizePSD=0.3 --pixelRefinement=%(pixelref)s LISACode/source-Background.xml' % globals())
seed = seed + 1
