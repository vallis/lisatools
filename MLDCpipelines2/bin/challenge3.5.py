#!/usr/bin/env python

import os
import sys
import random
import re

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

# two arguments, the seed and the number of processors
# (the second argument is an "istraining" flag)

seed = int(sys.argv[1])
istraining = (sys.argv[2] == 'True')
nproc = int(sys.argv[3])

random.seed(seed)

# to be run for one month, at one second sampling...

run('%(mydir)s/makesource-StochasticBackground.py --seed=%(seed)s --PSD=5.0e-48 --randomizePSD=0.3 --pixelRefinement=2 --nProc=%(nproc)s Immediate/Background.xml' % globals())

# now for LISACode

# use the same seed for training sets so we're sure we've got the same sources
lcseed = istraining and seed or random.randint(1,10000000)

run('%(mydir)s/makesource-StochasticBackground.py --seed=%(lcseed)s --PSD=5.0e-48 --randomizePSD=0.3 --pixelRefinement=2 LISACode/source-Background.xml' % globals())
