#!/usr/bin/env python

import os
import sys
import string
import random
from random import seed, randint
from optparse import OptionParser

def run(command):
    commandline = command % globals()
    print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

challengestring = "1.1a,1.1b,1.1c,1.2,1.3,1.4,1.5,2.1,2.2,3.1,3.2,3.3,3.4,3.5"

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

parser = OptionParser(usage="usage: %prog [options] SEED...",
                      version="$Id: $")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

parser.add_option("-R", "--runOnly",
                  type="string", dest="runOnly", default=challengestring,
                  help="which challenges to run (use a comma-separated list like 3.1,3.2) [default: all of them]")

parser.add_option("-T", "--trainingOnly",
                  action="store_true", dest="trainingOnly", default=False,
                  help="run only training [default: False]")

parser.add_option("-B", "--blindOnly",
                  action="store_true", dest="blindOnly", default=False,
                  help="run only blind [default: False]")

(options, args) = parser.parse_args()

nproc = options.nproc

if len(args) < 1:
    parser.error("You need to provide the SEED!")

seed(args[0])

def twoseeds():
    return randint(1,10000000), randint(1,10000000)

subchallenges = challengestring.split(',')
runonly = map(string.strip,options.runOnly.split(','))

for subchallenge in subchallenges:
    seeds, seedt = twoseeds()
    
    if subchallenge in runonly:
        duration = 31457280
        
        if not options.trainingOnly:
            run('%(mydir)s/challenge.py -P %(nproc)s            --duration=%(duration)s --seed=%(seeds)s  challenge1C.%(subchallenge)s' % globals())
        if not options.blindOnly:                    
            run('%(mydir)s/challenge.py -P %(nproc)s --training --duration=%(duration)s --seed=%(seedt)s  challenge1C.%(subchallenge)s' % globals())
