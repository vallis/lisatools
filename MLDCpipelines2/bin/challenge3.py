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

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

parser = OptionParser(usage="usage: %prog [options] SEED...",
                      version="$Id: $")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

parser.add_option("-R", "--runOnly",
                  type="string", dest="runOnly", default="3.1,3.2,3.3,3.4,3.5,3.5e",
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

seed31,seed31t = twoseeds()
seed32,seed32t = twoseeds()
seed33,seed33t = twoseeds()
seed34,seed34t = twoseeds()
seed35,seed35t = twoseeds()
seed35e,seed35et = twoseeds()

runonly = map(string.strip,options.runOnly.split(','))

if '3.1' in runonly:
    if not options.trainingOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s            --seed=%(seed31)s  challenge3.1' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --training --seed=%(seed31t)s challenge3.1' % globals())

if '3.2' in runonly:
    if not options.trainingOnly:                                                                  
        run('%(mydir)s/challenge.py -P %(nproc)s            --seed=%(seed32)s  --combinedSNR challenge3.2' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --training --seed=%(seed32t)s --combinedSNR challenge3.2' % globals())

if '3.3' in runonly:
    if not options.trainingOnly:                                                                  
        run('%(mydir)s/challenge.py -P %(nproc)s            --seed=%(seed33)s  --combinedSNR challenge3.3' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --training --seed=%(seed33t)s --combinedSNR challenge3.3' % globals())

options34 = '--rawMeasurements --randomizeNoise=0.2 --laserNoise=10'
if '3.4' in runonly:
    if not options.trainingOnly:                                                                  
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa            --duration=2097152 --timeStep=1 --seed=%(seed34)s  --combinedSNR %(options34)s challenge3.4' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa --training --duration=2097152 --timeStep=1 --seed=%(seed34t)s --combinedSNR %(options34)s challenge3.4' % globals())

options35 = '--rawMeasurements --randomizeNoise=0.2 --laserNoise=10 --LISA=Rigid'
if '3.5' in runonly:
    if not options.trainingOnly:                                                                  
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa --lisacode            --duration=2097152 --timeStep=2 --seed=%(seed35)s  %(options35)s challenge3.5' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa --lisacode --training --duration=2097152 --timeStep=2 --seed=%(seed35t)s %(options35)s challenge3.5' % globals())
        run('%(mydir)s/challenge.py -P %(nproc)s --noNoise   --lisacode --training --duration=2097152 --timeStep=2 --seed=%(seed35t)s %(options35)s challenge3.5' % globals())

options35e = '--rawMeasurements --randomizeNoise=0.2 --laserNoise=10 --LISA=Eccentric'
if '3.5e' in runonly:
    if not options.trainingOnly:                                                                  
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa --lisacode            --duration=2097152 --timeStep=2 --seed=%(seed35e)s  %(options35e)s challenge3.5e' % globals())
    if not options.blindOnly:
        run('%(mydir)s/challenge.py -P %(nproc)s --synthlisa --lisacode --training --duration=2097152 --timeStep=2 --seed=%(seed35et)s %(options35e)s challenge3.5e' % globals())
        run('%(mydir)s/challenge.py -P %(nproc)s --noNoise   --lisacode --training --duration=2097152 --timeStep=2 --seed=%(seed35et)s %(options35e)s challenge3.5e' % globals())
