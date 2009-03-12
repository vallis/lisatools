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

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

# only one argument, and it's the seed...
seed = int(sys.argv[1])

random.seed(seed)

requestsn = random.randint(10,20)

seed += 1

run('%(mydir)s/makesource-GalacticBinary.py --seed=%(seed)s --amplitude=1e-21 --requestSN=%(requestsn)s --centerf=3.0e-3 --deltaf=0.1e-3 --sourceName="GB-1.1.1b" Source/GB-1.1.1b.xml' % globals())
