#!/usr/bin/env python

import sys
import os
import glob
import re
import time
import math

import lisaxml
import numpy

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

parser = OptionParser(usage="usage: %prog [options] XMLSourceFiles ...",
                      version="14.05.09, Antoine Petiteau from M. Vallisneri (challenge.py)")

# note that the default value is handled below
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int); None time based seed [default None]")

parser.add_option("-N","--noNoise",
                  action="store_true", dest="nonoise", default=False,
                  help="do not include noise [default False]")

# note the letter mismatch with the makeTDIsignal/noise-synthlisa2.py option
parser.add_option("-I", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default Eccentric]")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z [default False]")

parser.add_option("-R", "--randomizeNoise",
                  type="float", dest="randomizeNoise", default=0.0,
                  help="randomize level of noises [e.g., 0.2 for 20% randomization; defaults to 0]")

parser.add_option("-l", "--laserNoise",
                  type="string", dest="laserNoise", default='None',
                  help="laser noise level: None, Standard, <numerical value> [e.g., 0.2 for 20% of pm noise at 1 mHz; defaults to None]")


parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default="LC",
                  help="basename for the datasets [defaults to LC]")


(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need one (or more) XML barycentric source data file!")

XMLSrc = ""
for ia in range(len(args)):
    XMLSrc = XMLSrc + args[ia] + " "

## Find directory which contains LISACode template in lisatools package
run("echo $SVNDIR/MLDCpipelines2/Template > tmpdirlogfile")
logfiletmp = open("tmpdirlogfile", 'r')
ind = 0
for line in logfiletmp:
    rgms =  re.split('\s+', line)
    if ind == 0:
        dirTemplate = rgms[0]
    ind = ind + 1
print "Directory of template :", dirTemplate
run("rm tmpdirlogfile") 

### Make noise options
donoise = not options.nonoise
noiseoptions = ''
if options.randomizeNoise > 0.0:
    noiseoptions += ('--randomizeNoise=%s ' % options.randomizeNoise)
if options.laserNoise != 'None':
    noiseoptions += ('--laserNoise=%s ' % options.laserNoise)
if options.rawMeasurements == True:
    noiseoptions += '--rawMeasurements '
if options.LISAmodel != 'Eccentric':
    noiseoptions += '--LISA=%s ' % options.LISAmodel


### Define template options
cname = options.outputfile
timestep = options.timestep
duration = options.duration
lcseed = options.seed
LISAmodel = options.LISAmodel
if donoise:
    PhasemF = "On"
else:
    PhasemF = "Off"

### Define phasemeters' output 
if(options.rawMeasurements == False):
	rawcname = "None"
	outsignals = "t,Xf,Yf,Zf"
else:
	rawcname = cname
	outsignals = "t,Xf,Yf,Zf,y231,y321,z231,z321,y312,y132,z312,z132,y123,y213,z123,z213"

### Make base XML input file
makefromtemplate('%s-lisacode-input.xml' % cname,'%s/LISACode2.xml' %dirTemplate, challengename=cname, rawchallengename=rawcname, outputsignals=outsignals, cadence=timestep, duration=duration ,randomseed=lcseed, orbit=LISAmodel, phasemeterfilter=PhasemF)

### Make GW+noise XML input file
if donoise:
    ## Make lisacode noise (note that the random seed is really set above in the standard instruction set)
    run('makeTDInoise-synthlisa2.py --keyOnly --keyOmitsLISA --seed=123456 --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s noise.xml')
    run('mergeXML.py %(cname)s-lisacode-input.xml noise.xml')
    run('rm noise.xml')

LCinputXML = cname + '-lisacode-input.xml'
## Merge with source data into a single lisacode input file
basefile = lisaxml.readXML(LCinputXML)
lisa = basefile.getLISAgeometry()
sources = basefile.getLISASources()    
extrasecs = []
for sec in ['Simulate','LISACode','NoiseData']:
    if basefile.getExtraSection(sec):
        extrasecs.append(basefile.getExtraSection(sec))
author = "Antoine Petiteau and Michele Vallisneri"
comments = basefile.Comment
basefile.close()

newbasefile = lisaxml.lisaXML(LCinputXML,author=author,comments=comments)
for inputfilename in args:
    inputfile = lisaxml.readXML(inputfilename)
    sources = inputfile.getLISASources()
    if not lisa:
        lisa = inputfile.getLISAgeometry()
    for source in sources:
        newbasefile.SourceData(source)
newbasefile.LISAData(lisa)
for sec in extrasecs:
    newbasefile.ExtraSection(sec)
newbasefile.close()

run('LISACode %s-lisacode-input.xml' % (cname))
