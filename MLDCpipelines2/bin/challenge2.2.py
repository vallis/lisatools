#!/usr/bin/env python

import os
import sys

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

# standard BBH sources...
run('bin/makesource-BBH.py --seed=%s --distance=1e6 --coalescTime=150 Source/BH-1.xml' % (seed+1))
run('bin/makesource-BBH.py --seed=%s --distance=2e6 --coalescTime=120 Source/BH-2.xml' % (seed+2))

# standard EMRI source...
run('bin/makesource-EMRI.py --seed=%s --distance=1e6 Source/EMRI-1.xml' % (seed+3))

# now the Galaxy!
# run('bin/makesource-Galaxy.py %s' % seed)
