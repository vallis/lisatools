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

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))

# only one argument, and it's the seed...
seed = int(sys.argv[1])

random.seed(seed)

# Challenge 1.2.1: SMBH with time to coalescence 6 +- 1 months
#                            masses m1: 1-5 10^6, m1/m2: 1-4
#                            SNR: 450-500

requestsn = random.randint(100,200)

seed += 1

run('%(execdir)s/makesource-BBH.py --seed=%(seed)s --distance=1e10 --requestSN=%(requestsn)s --coalescTime=180 --coalescRange=30 --sourceName="SMBH-1.2.1" Source/BH-1.2.1.xml' % globals())
