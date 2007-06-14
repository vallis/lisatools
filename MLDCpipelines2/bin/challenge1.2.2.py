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

random.seed(seed)

# Challenge 1.2.2: SMBH with time to coalescence 400 +- 40 months
#                            masses m1: 1-5 10^6, m1/m2: 1-4
#                            SNR: 20-100

requestsn = random.randint(20,100)

seed += 1

run('bin/makesource-BBH.py --seed=%(seed)s --distance=1e10 --requestSN=%(requestsn)s --coalescTime=400 --coalescRange=40 --sourceName="SMBH-1.2.2" Source/BH-1.2.2.xml' % globals())
