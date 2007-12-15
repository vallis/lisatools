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

requestsn = random.randint(10,50)
run('%s/makesource-EMRI.py -f --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1.e7 --sourceName="EMRI-3.3.1" Source/EMRI-3.3.1.xml' % (mydir,seed+1,requestsn))

requestsn = random.randint(10,50)
run('%s/makesource-EMRI.py -f --seed=%s --distance=1e9 --requestSN=%s --massSMBH=5.e6 --sourceName="EMRI-3.3.2" Source/EMRI-3.3.2.xml' % (mydir,seed+2,requestsn))

requestsn = random.randint(10,50)
run('%s/makesource-EMRI.py -f --seed=%s --distance=1e9 --requestSN=%s --massSMBH=5.e6 --sourceName="EMRI-3.3.3" Source/EMRI-3.3.3.xml' % (mydir,seed+3,requestsn))

requestsn = random.randint(10,50)
run('%s/makesource-EMRI.py -f --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1.e6 --sourceName="EMRI-3.3.4" Source/EMRI-3.3.4.xml' % (mydir,seed+4,requestsn))

requestsn = random.randint(10,50)
run('%s/makesource-EMRI.py -f --seed=%s --distance=1e9 --requestSN=%s --massSMBH=1.e6 --sourceName="EMRI-3.3.5" Source/EMRI-3.3.5.xml' % (mydir,seed+5,requestsn))
