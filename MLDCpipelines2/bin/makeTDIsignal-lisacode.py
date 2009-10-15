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

parser = OptionParser(usage="usage: %prog [options] BARYCENTRIC.xml OUTPUT.xml",
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

parser.add_option("-L", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")

parser.add_option("-O", "--observables",
                  type="string", dest="observables", default='1.5',
                  help="TDI observables: 1.5 [default], 2.0, Sagnac (experimental; not compatible with -r) : Not use by LISACode at the moment")

parser.add_option("-n", "--noiseOnly",
                  action="store_true", dest="noiseOnly", default=False,
                  help="compute SNR using instrumental noise only [off by default] : Not use by LISACode at the moment")

parser.add_option("-D", "--debugSNR",
                  action="store_true", dest="debugSNR", default=False,
                  help="show more SNR data [off by default] : Not use by LISACode at the moment")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined snr = sqrt(2)*max{SNR_x, SNR_y, SNR_z} as SNR constrain [off by default] : Not use by LISACode at the moment")

parser.add_option("-A", "--keyOmitsLISA",
                  action="store_true", dest="keyOmitsLISA", default=False,
                  help="do not include LISA specification in key [included by default]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default] : Not use by LISACode at the moment")

(options, args) = parser.parse_args()

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args


detailXMLout = True

## Find directory which contains LISACode template in lisatools package
#run("echo $SVNDIR/MLDCpipelines2/Template > tmpdirlogfile")
#logfiletmp = open("tmpdirlogfile", 'r')
#ind = 0
#for line in logfiletmp:
#    rgms =  re.split('\s+', line)
#    if ind == 0:
#        dirTemplate = rgms[0]
#    ind = ind + 1
#print "Directory of template :", dirTemplate
#run("rm tmpdirlogfile") 
execdir = os.path.dirname(os.path.abspath(sys.argv[0]))




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
PhasemF = "Off"

### Define phasemeters' output 
if(options.rawMeasurements == False):
	rawcname = "None"
	outsignals = "t,Xf,Yf,Zf"
else:
	rawcname = cname
	outsignals = "t,Xf,Yf,Zf,y231,y321,z231,z321,y312,y132,z312,z132,y123,y213,z123,z213"


inputsrc = lisaxml.readXML(inputfile)
sources = inputsrc.getLISASources()

#print "Duration = " , duration
#print "TimeOffset = ", timeoffset

isrc = 0
for source in sources:
    isrc = isrc + 1
    if hasattr(source,'RequestTimeOffset') and hasattr(source,'RequestDuration'):
        #print "Request Duration = " , source.RequestDuration
        #print "Request TimeOffset = ", source.RequestTimeOffset
        if((timeoffset > source.RequestTimeOffset) or (isrc == 1)):
            timeoffset = source.RequestTimeOffset
        if((duration < source.RequestDuration) or (isrc == 1)):
            duration = source.RequestDuration

#print "Duration = " , duration
#print "TimeOffset = ", timeoffset


### Make base XML input file
makefromtemplate('%s-input.xml' % cname, '%s/../Template/LISACode2.xml' % execdir, challengename=cname, rawchallengename=rawcname, outputsignals=outsignals, cadence=timestep, duration=duration , timeoffset= timeoffset, randomseed=lcseed, orbit=LISAmodel, phasemeterfilter=PhasemF)


LCinputXML = cname + '-input.xml'
LCOutputXML = cname + '.xml'
## Merge with source data into a single lisacode input file
basefile = lisaxml.readXML(LCinputXML)
lisa = basefile.getLISAgeometry()
sourcesLC = basefile.getLISASources()    
extrasecs = []
for sec in ['Simulate','LISACode','NoiseData']:
    if basefile.getExtraSection(sec):
        extrasecs.append(basefile.getExtraSection(sec))
author = "Antoine Petiteau and Michele Vallisneri"
comments = basefile.Comment
basefile.close()

newbasefile = lisaxml.lisaXML(LCinputXML,author=author,comments=comments)
for source in sourcesLC:
    if hasattr(source,'TimeSeries') and hasattr(source,'Polarization'):
        source.Polarization = 0.0
    newbasefile.SourceData(source)
for source in sources:
    if hasattr(source,'TimeSeries') and hasattr(source,'Polarization'):
        source.Polarization = 0.0
    newbasefile.SourceData(source)
if not lisa:
    lisa = inputsrc.getLISAgeometry()
newbasefile.LISAData(lisa)
for sec in extrasecs:
    newbasefile.ExtraSection(sec)
newbasefile.close()


if options.verbose:
    run('%s %s-input.xml' % (lisacode.lisacode,cname))
else:
    run('%s %s-input.xml > LogLC-%s' % (lisacode.lisacode,cname,cname))


## Include informations in XML output file
if detailXMLout:
    outputXML = lisaxml.readXML(LCOutputXML)
    if outputXML.getExtraSection('TDIData'):
        datasec = outputXML.getExtraSection('TDIData')
    else:
        print "Error: No data in", LCOutputXML
        sys.exit(1)
    outputXML.close()
    
    finalXML = lisaxml.lisaXML(LCOutputXML,author=author,comments=comments)
    for source in sourcesLC:
        if hasattr(source,'TimeSeries'):
            del source.TimeSeries
        finalXML.SourceData(source)
    for source in sources:
        if hasattr(source,'TimeSeries'):
            del source.TimeSeries
        finalXML.SourceData(source)
    if options.keyOmitsLISA == False :
        for sec in extrasecs:
            finalXML.ExtraSection(sec)
        finalXML.LISAData(lisa)
    finalXML.ExtraSection(datasec)
    finalXML.close()
    
