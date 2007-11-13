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

# only one argument, and it's the seed...
seed = int(sys.argv[1])

random.seed(seed)

# Challenge 3.4: Poisson-distributed, 6 events
#                define month as 2**21 samples, 1 Hz (really 24 days)

srcnum = numpy.random.poisson(5)

for srcind in range(srcnum):
    seed = seed + 1
    requestsn = random.randint(10,100)

    run('bin/makesource-CosmicStringCusp.py --seed=%(seed)s -t 1048576 -r 1048576 -S %(requestsn)s -n "Cosmic String %(srcind)s" Source/Burst-3.4.%(srcind)s' % globals())
