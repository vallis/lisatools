#!/usr/bin/env python

__version__='$Id: $'

import sys
import os
import glob
import re
import time
import math

import lisaxml
import numpy

import lisacode

def makefromtemplate(output,template,**kwargs):
    fi = open(template,'r')
    fo = open(output,'w')
        
    for line in fi:
        repline = line
        
        for kw in kwargs:
            repline = re.sub('{' + kw + '}',str(kwargs[kw]),repline)
        
        print >> fo, repline,

def run(command):
    commandline = command % globals()
    print "--> %s" % commandline

    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="14.05.09, Antoine Petiteau from M. Vallisneri (challenge.py)")

# note that the default value is handled below
parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for TDI observables (s) [default 0.0]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z")

#parser.add_option("-O", "--observables",
#                  type="string", dest="observables", default='1.5',
#                  help="TDI observables: 1.5 [default], 2.0, Sagnac (not compatible with -r)")

#parser.add_option("-R", "--randomizeNoise",
#                  type="float", dest="randomizeNoise", default=0.0,
#                  help="randomize level of noises (e.g., 0.2 for 20% randomization)")

parser.add_option("-x", "--redAcceleration",
                  action="store_true", dest="redAcceleration", default=False,
                  help="disable pink proof-mass acceleration noise [enabled by default]")

parser.add_option("-P", "--positionNoise",
                  type="string", dest="positionNoise", default='Standard',
                  help="proof-mass noise level: Standard [default], <numerical value> [in m^2 Hz^-1]")

parser.add_option("-o", "--rescalePositionNoise",
                  action="store_true", dest="rescalePositionNoise", default=False,
                  help="rescale proof-mass noise level by armlength [off by default]")

#parser.add_option("-a", "--armlength",
#                  type="string", dest="armlength", default='Standard',
#                  help="LISA armlength (Static and Rigid LISA only): Standard [default, = 16.6782 s], <numerical value> [s]")

parser.add_option("-l", "--laserNoise",
                  type="string", dest="laserNoise", default='None',
                  help="laser noise level: None [default], Standard, <numerical value> (e.g., 0.2 for 20% of pm noise at 1 mHz)")

parser.add_option("-L", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")


parser.add_option("-A", "--keyOmitsLISA",
                  action="store_true", dest="keyOmitsLISA", default=False,
                  help="do not include LISA specification in key [included by default]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if options.duration < 10:
    options.duration = options.duration * 31457280

# for the moment, support a single input barycentric file

if len(args) < 1:
    parser.error("You must specify an output file!")

outputfile = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))

### Transfert options
noiseoptions = ''
if options.rawMeasurements == True:
    noiseoptions += '--rawMeasurements '
#if options.randomizeNoise > 0.0:
#    noiseoptions += ('--randomizeNoise=%s ' % options.randomizeNoise)
if options.redAcceleration == True:
    noiseoptions += '--redAcceleration'
if options.positionNoise != 'Standard':
    noiseoptions += ('--positionNoise=%s ' % options.positionNoise)
if options.rescalePositionNoise == True:
    noiseoptions += '--rescalePositionNoise'
if options.laserNoise != 'None':
    noiseoptions += ('--laserNoise=%s ' % options.laserNoise)
if options.LISAmodel != 'Eccentric':
    noiseoptions += '--LISA=%s ' % options.LISAmodel


### Define template options
if(re.search("\.xml", outputfile) != None):
    cname = outputfile[:-4]
else:
    cname = outputfile
timeoffset = 0.0
timestep = options.timestep
duration = options.duration
lcseed = options.seed
LISAmodel = options.LISAmodel
PhasemF = "On"


### Define phasemeters' output 
if(options.rawMeasurements == False):
	rawcname = "None"
	outsignals = "t,Xf,Yf,Zf"
else:
	rawcname = cname
	outsignals = "t,Xf,Yf,Zf,y231,y321,z231,z321,y312,y132,z312,z132,y123,y213,z123,z213"

### Make base XML input file
makefromtemplate('%s-LCbase.xml' % cname, '%s/../Template/LISACode2.xml' % execdir, challengename=cname, rawchallengename=rawcname, outputsignals=outsignals, cadence=timestep, duration=duration , timeoffset= timeoffset, randomseed=lcseed, orbit=LISAmodel, phasemeterfilter=PhasemF)


## Make lisacode noise (note that the random seed is really set above in the standard instruction set)
#run('makeTDInoise-synthlisa2.py --keyOnly --keyOmitsLISA --seed=123456 --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s SLnoise.xml')
#run('mergeXML.py %(cname)s-input.xml noise.xml')
#run('rm noise.xml')



#### Merge XML file
## Extract informations from LISACode XML input file
LCBaseXML = cname + '-LCbase.xml'
LCbasefile = lisaxml.readXML(LCBaseXML)
LClisa = LCbasefile.getLISAgeometry()
LCsources = LCbasefile.getLISASources()    
LCextrasecs = []
for sec in ['Simulate','LISACode','NoiseData']:
    if LCbasefile.getExtraSection(sec):
        LCextrasecs.append(LCbasefile.getExtraSection(sec))
author = "Antoine Petiteau and Michele Vallisneri"
comments = LCbasefile.Comment
LCbasefile.close()


## Make lisacode noise (note that the random seed is really set above in the standard instruction set)
run('makeTDInoise-synthlisa2.py --keyOnly --keyOmitsLISA --seed=123456 --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s SLnoise.xml')
## Extract informations from SyntheticLISA XML noise file
SLinputXML = 'SLnoise.xml'
SLbasefile = lisaxml.readXML(SLinputXML)
SLlisa = SLbasefile.getLISAgeometry()
SLsources = SLbasefile.getLISASources()    
SLextrasecs = []
for sec in ['Simulate','LISACode','NoiseData']:
    if SLbasefile.getExtraSection(sec):
        SLextrasecs.append(SLbasefile.getExtraSection(sec))
author = "Antoine Petiteau and Michele Vallisneri"
comments = SLbasefile.Comment
SLbasefile.close()

LCinputXML = cname + '-input.xml'
newbasefile = lisaxml.lisaXML(LCinputXML,author=author,comments=comments)
newbasefile.LISAData(LClisa)
for sec in LCextrasecs:
    newbasefile.ExtraSection(sec)
for sec in SLextrasecs:
    newbasefile.ExtraSection(sec)
newbasefile.close()

if options.verbose:
    run('%s %s-input.xml' % (lisacode.lisacode,cname))
else:
    run('%s %s-input.xml > LogLC-%s' % (lisacode.lisacode,cname,cname))
