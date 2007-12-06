#!/usr/bin/env python

import os
import sys
import random
from random import randint
from optparse import OptionParser

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

parser = OptionParser(usage="usage: %prog [options] SEED...",
                      version="$Id: $")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

(options, args) = parser.parse_args()

nproc = options.nproc
seed  = int(args[0])

run('%(mydir)s/challenge.py --seed=%(seed)s --duration=2097152 --timeStep=1 --nProc=%(nproc)s --synthlisa challenge3.5')
