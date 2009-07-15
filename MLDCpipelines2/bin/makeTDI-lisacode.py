#!/usr/bin/env python

import sys
import os
import glob
import re
import time
import math

import lisaxml
import numpy

import FastBBH
import CosmicStringCusp
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

parser = OptionParser(usage="usage: %prog [options] XMLSourceFiles ...",
                      version="14.05.09, Antoine Petiteau from M. Vallisneri (challenge.py)")

parser.add_option("-e", "--outputfile",
                  type="string", dest="outputfile", default="LC",
                  help="basename for the datasets [defaults to LC]")

parser.add_option("-w","--LCwaveform",
                  action="store_true", dest="LCwaveform", default=False,
                  help="If it's possible use LISACode waveform instead of file data [default False]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int); None time based seed [default None]")

## Time
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15,
                  help="timestep for TDI observable sampling (s) [default 15]")

## Orbits
parser.add_option("-I", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default Eccentric]")

parser.add_option("-a", "--armlength",
                  type="string", dest="armlength", default='Standard',
                  help="LISA armlength (Static and Rigid LISA only): Standard [default, = 16.6782 s], <numerical value> [s] : Not use by LISACode at the moment")

## Output
parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z [default False]")

parser.add_option("-O", "--observables",
                  type="string", dest="observables", default='1.5',
                  help="TDI observables: 1.5 [default], 2.0, Sagnac (experimental; not compatible with -r) : Not use by LISACode at the moment")

## Noise
parser.add_option("-N","--noNoise",
                  action="store_true", dest="nonoise", default=False,
                  help="do not include noise [default False]")

parser.add_option("-R", "--randomizeNoise",
                  type="float", dest="randomizeNoise", default=0.0,
                  help="randomize level of noises [e.g., 0.2 for 20% randomization; defaults to 0] : Not use by LISACode at the moment")

parser.add_option("-l", "--laserNoise",
                  type="string", dest="laserNoise", default='None',
                  help="laser noise level: None, Standard, <numerical value> [e.g., 0.2 for 20% of pm noise at 1 mHz; defaults to None]")

parser.add_option("-x", "--redAcceleration",
                  action="store_true", dest="redAcceleration", default=False,
                  help="disable pink proof-mass acceleration noise [enabled by default]")

parser.add_option("-P", "--positionNoise",
                  type="string", dest="positionNoise", default='Standard',
                  help="proof-mass noise level: Standard [default], <numerical value> [in m^2 Hz^-1]")

parser.add_option("-o", "--rescalePositionNoise",
                  action="store_true", dest="rescalePositionNoise", default=False,
                  help="rescale proof-mass noise level by armlength [off by default]")

## For SNR
parser.add_option("-n", "--noiseOnly",
                  action="store_true", dest="noiseOnly", default=False,
                  help="compute SNR using instrumental noise only [off by default] : Not use by LISACode at the moment")

parser.add_option("-D", "--debugSNR",
                  action="store_true", dest="debugSNR", default=False,
                  help="show more SNR data [off by default] : Not use by LISACode at the moment")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined snr = sqrt(2)*max{SNR_x, SNR_y, SNR_z} as SNR constrain [off by default] : Not use by LISACode at the moment")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default] : Not use by LISACode at the moment")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need one (or more) XML barycentric source data file!")

XMLSrc = ""
for ia in range(len(args)):
    XMLSrc = XMLSrc + args[ia] + " "

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))



### Make noise options
donoise = not options.nonoise
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
if(re.search("\.xml", options.outputfile) != None):
    cname = options.outputfile[:-4]
else:
    cname = options.outputfile
timeoffset = 0.0
timestep = options.timestep
duration = options.duration
lcseed = options.seed
LISAmodel = options.LISAmodel
PhasemF = "Off"
#if donoise:
#    PhasemF = "On"
#else:
#    PhasemF = "Off"


### Define phasemeters' output 
if(options.rawMeasurements == False):
	rawcname = "None"
	outsignals = "t,Xf,Yf,Zf"
else:
	rawcname = cname
	outsignals = "t,Xf,Yf,Zf,y231,y321,z231,z321,y312,y132,z312,z132,y123,y213,z123,z213"


### Make base XML input file
makefromtemplate('%s-input.xml' % cname, '%s/../Template/LISACode2.xml' % execdir, challengename=cname, rawchallengename=rawcname, outputsignals=outsignals, cadence=timestep, duration=duration , timeoffset= timeoffset, randomseed=lcseed, orbit=LISAmodel, phasemeterfilter=PhasemF)


### Make GW+noise XML input file
if donoise:
    ## Make lisacode noise (note that the random seed is really set above in the standard instruction set)
    run('makeTDInoise-synthlisa2.py --keyOnly --keyOmitsLISA --seed=123456 --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s noise.xml')
    run('mergeXML.py %(cname)s-input.xml noise.xml')
    run('rm noise.xml')

LCinputXML = cname + '-input.xml'
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
        if (options.LCwaveform) and (source.xmltype == 'FastSpinBlackHoleBinary'):
            print source.xmltype , " --> Use LISACode waveform"
            mysystem = FastBBH.FastSpinBlackHoleBinary('FastSpinBlackHoleBinary')
            mysystem.EclipticLatitude       = source.EclipticLatitude
            mysystem.EclipticLongitude      = source.EclipticLongitude
            mysystem.PolarAngleOfSpin1      = source.PolarAngleOfSpin1
            mysystem.PolarAngleOfSpin2	    = source.PolarAngleOfSpin2
            mysystem.AzimuthalAngleOfSpin1  = source.AzimuthalAngleOfSpin1
            mysystem.AzimuthalAngleOfSpin2  = source.AzimuthalAngleOfSpin2
            mysystem.Spin1                  = source.Spin1
            mysystem.Spin2                  = source.Spin2
            mysystem.Mass1                  = source.Mass1
            mysystem.Mass2                  = source.Mass2
            mysystem.CoalescenceTime        = source.CoalescenceTime
            mysystem.PhaseAtCoalescence     = source.PhaseAtCoalescence
            mysystem.InitialPolarAngleL     = source.InitialPolarAngleL
            mysystem.InitialAzimuthalAngleL = source.InitialAzimuthalAngleL
            mysystem.Distance               = source.Distance
            mysystem.AmplPNorder            = source.AmplPNorder
            mysystem.Polarization           = source.Polarization
            mysystem.Polarization_Unit      = 'Radian'
        elif (options.LCwaveform) and (source.xmltype == 'CosmicStringCusp' ):
            print source.xmltype , " --> Use LISACode waveform"
            mysystem = CosmicStringCusp.CosmicStringCusp('CosmicStringCusp')
            mysystem.EclipticLatitude   = source.EclipticLatitude
            mysystem.EclipticLongitude  = source.EclipticLongitude
            mysystem.Polarization       = source.Polarization
            mysystem.Amplitude          = source.Amplitude
            mysystem.CentralTime        = source.CentralTime
            mysystem.MaximumFrequency   = source.MaximumFrequency
            if hasattr(source,'RequestTimeOffset'):
                mysystem.RequestTimeOffset = source.RequestTimeOffset
                mysystem.RequestTimeOffset_Unit = source.RequestTimeOffset_Unit
            if hasattr(source,'RequestDuration'):
                mysystem.RequestDuration = source.RequestDuration
                mysystem.RequestDuration_Unit = source.RequestDuration_Unit
        else:
            print source.xmltype , " --> Use barycentric data file waveform (polarization=0, already included in data)"
            if hasattr(source,'TimeSeries') and hasattr(source,'Polarization'):
                source.Polarization = 0.0
            mysystem = source
        if options.verbose:
            for p in mysystem.parameters:
                print p, ':', mysystem.parstr(p)
        newbasefile.SourceData(mysystem)
newbasefile.LISAData(lisa)
for sec in extrasecs:
    newbasefile.ExtraSection(sec)
newbasefile.close()

if options.verbose:
    run('%s %s-input.xml' % (lisacode.lisacode,cname))
else:
    run('%s %s-input.xml > LogLC-%s' % (lisacode.lisacode,cname,cname))
