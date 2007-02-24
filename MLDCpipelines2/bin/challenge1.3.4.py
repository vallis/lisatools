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

#Challenge 1.3.4: One EMRI. MBH mass = 1.e6, snr random between 70 and 110

requestsn = random.randint(70,110)
run('bin/makesource-EMRI.py --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1.e6 --sourceName="EMRI-1.3.4" Source/EMRI-1.3.4.xml' % (seed,requestsn))


