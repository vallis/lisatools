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

# just the Galaxy! (for testing, seed must be 1)
# run('bin/makesource-Galaxy.py -t %s' % seed)

run('bin/makesource-Galaxy.py %s' % seed)
