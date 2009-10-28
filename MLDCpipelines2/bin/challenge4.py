#!/usr/bin/env python

import os
import sys
import string
import random
import re
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

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

parser = OptionParser(usage="usage: %prog [options] SEED [CHALLENGENAME]...",
                      version="$Id$")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

parser.add_option("-T", "--trainingOnly",
                  action="store_true", dest="trainingOnly", default=False,
                  help="run only training [default: False]")

parser.add_option("-I", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")

parser.add_option("-B", "--blindOnly",
                  action="store_true", dest="blindOnly", default=False,
                  help="run only blind [default: False]")

parser.add_option("-S", "--simulators",
                  type="string", dest="simulators", default="sl,ls,lc",
                  help="which simulators to run (a comma separated list including sl, ls, and lc) [default: all of them]")

(options, args) = parser.parse_args()

nproc = options.nproc

if len(args) < 1:
    parser.error("You need to provide the SEED!")

seed(args[0])

def twoseeds():
    return randint(1,10000000), randint(1,10000000)

seed4,seed4t = twoseeds()

options4 = '--duration=62914560 --timeStep=15 --LISA=' + options.LISAmodel

if 'sl' in options.simulators:
    options4 += ' --synthlisa'
if 'ls' in options.simulators:
    options4 += ' --lisasim'
if 'lc' in options.simulators:
    options4 += ' --lisacode'

challengename = (len(args) > 1) and args[1] or 'challenge4.0'

if not options.trainingOnly:                                                                  
    run('%(mydir)s/challenge.py -P %(nproc)s %(options4)s            --seed=%(seed4)s  --combinedSNR %(challengename)s' % globals())
if not options.blindOnly:
    run('%(mydir)s/challenge.py -P %(nproc)s %(options4)s --training --seed=%(seed4t)s --combinedSNR %(challengename)s' % globals())
