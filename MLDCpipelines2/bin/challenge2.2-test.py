#!/usr/bin/env python

import os
import sys
import random

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

# only one argument, and it's the seed...
seed = int(sys.argv[1])

# just one BH and one EMRI

# 1 - A loud (SNR ~ 2000) signal (with in-spiral "in band", that is from f = 0.1 mHz, lasting about 1 week) and coalescing 3 months +/- 1 month after the beginning of the observations
run('bin/makesource-BBH.py --seed=%s --distance=1e10 --requestSN=2000 --coalescTime=90  --coalescRange=30 Source/BH-1.xml' % (seed+1))

# requestsn = random.randint(30,100)
# run('bin/makesource-EMRI.py --seed=%s --distance=1e6 --requestSN=%s --massSMBH=1e6 Source/EMRI-1.xml' % (seed+7,requestsn))

# now the Galaxy! (for testing, seed must be 1)
# run('bin/makesource-Galaxy.py -t %s' % seed)

# run('bin/makesource-Galaxy.py %s' % seed)
