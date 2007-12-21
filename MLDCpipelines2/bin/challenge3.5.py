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

def makefromtemplate(output,template,**kwargs):
    fi = open(template,'r')
    fo = open(output,'w')
        
    for line in fi:
        repline = line
        
        for kw in kwargs:
            repline = re.sub('{' + kw + '}',str(kwargs[kw]),repline)
        
        print >> fo, repline,


mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

# two arguments, the seed and the number of processors
# (the second argument is an "istraining" flag)

seed = int(sys.argv[1])
istraining = sys.argv[2]
nproc = int(sys.argv[3])

random.seed(seed)

# to be run for one month, at one second sampling...

run('%(mydir)s/makesource-StochasticBackground.py --seed=%(seed)s --PSD=1.0e-48 --randomizePSD=0.3 --pixelRefinement=2 --nProc=%(nproc)s Immediate/Background.xml' % globals())

# for LISACode

if len(sys.argv) > 4 and sys.argv[4] == '--dolisacode':
    lisacodeseed = random.randint(1,10000000)

    run('%(mydir)s/makesource-StochasticBackground.py --seed=%(seed)s --PSD=1.0e-48 --randomizePSD=0.3 --pixelRefinement=2 LISACode/Background-sources.xml' % globals())
    run('%(mydir)s/makeTDInoise-synthlisa2.py --keyOnly --seed=%(seed)s --randomizeNoise=0.2 --laserNoise=10 LISACode/Background-noise.xml' % globals())

    if istraining == 'True':
        makefromtemplate('LISACode/Background.xml','Template/LISACode.xml',challengename='challenge3.5-training',randomseed=lisacodeseed)
    else:
        makefromtemplate('LISACode/Background.xml','Template/LISACode.xml',challengename='challenge3.5',randomseed=lisacodeseed)

    run('%(mydir)s/mergeXML.py LISACode/Background.xml LISACode/Background-noise.xml LISACode/Background-sources.xml' % globals())
    run('rm LISACode/Background-noise.xml LISACode/Background-sources.xml')
