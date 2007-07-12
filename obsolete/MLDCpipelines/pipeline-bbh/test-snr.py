#!/usr/bin/env python

from synthlisa import *
from Numeric import *

import math
import sys, getopt

#def usage():
#     print >>sys.stderr, """
#  Usage: BBH_p [options]
#
#  -h, --help       display this message
#  -t, --duration   duration of the data (in years)
#  -f, --xmlFile    input XML file(s) with source parameters
#
#"""
"""
shortop = "hf:t:"
longop = [
     "help",
     "xmlFile=",
     "duration="]

try:
    opts, args = getopt.getopt(sys.argv[1:], shortop, longop)
except getopt.GetoptError:
    usage()
    sys.exit(1)
    
inputfile = None

for o,a in opts:
    if o in ("-h", "--help"):
        usage()
	sys.exit(0)
    elif o in ("-f", "--xmlFile"):
  	inputfile = a

if not inputfile:
     print >> sys.stderr, "You must specify the xml file(s)"
     usage()
     sys.exit(1)


print inputfile
sys.exit(0)
"""
if len(sys.argv) < 2:
    print "Usage: %s <sourcefile.xml> [<sourcefile.xml>]" % sys.argv[0]
    sys.exit(1)

# make lisa


lisa = EccentricInclined(0.0,1.5*math.pi,-1)

stime = 15
samples = 2**21
patches = 256

# make noise

proofnoise = [SumSignal(stdproofnoise(lisa,100,2.5e-48,2),
                        stdproofnoisepink(lisa,1000,2.5e-48,1.0e-4,2))
              for i in range(6)]
shotnoise =  [stdopticalnoise(lisa,15,1.8e-37,4) for i in range(6)]
lasernoise = [NoSignal() for i in range(6)]

tdinoise = TDInoise(lisa,proofnoise,shotnoise,lasernoise)

[alphan,betan,gamman] = transpose(getobsc(samples,stime,[tdinoise.alpha,
                                                         tdinoise.beta,
                                                         tdinoise.gamma]))

noiseA = (gamman - alphan) / math.sqrt(2.0)
noiseE = (alphan - 2.0*betan + gamman) / math.sqrt(6.0)
noiseT = (alphan + betan + gamman) / math.sqrt(3.0)

# make sources

for filename in sys.argv[1:]:
    inputXML = readXML(filename)
    sources = inputXML.getLISASources()
    inputXML.close()
    
    for source in sources:
        tdisource = TDIsignal(lisa,source)
            
        # get signals
        
        [alphas,betas,gammas] = transpose(getobsc(samples,stime,[tdisource.alpha,
                                                                 tdisource.beta,
                                                                 tdisource.gamma]))
                
        # form the optimal TDI observables
        
        signalA = (gammas - alphas) / math.sqrt(2.0)
        signalE = (alphas - 2.0*betas + gammas) / math.sqrt(6.0)
        signalT = (alphas + betas + gammas) / math.sqrt(3.0)
                
        # compute the optimal signal to noise (for short time series, this is somewhat
        # sensitive to the fluctuations in the noise, which is used to compute S_n)
        
        totalsn = math.sqrt(sn(signalA,noiseA,stime,patches)**2 + 
                            sn(signalE,noiseE,stime,patches)**2 + 
                            sn(signalT,noiseT,stime,patches)**2)/math.sqrt(2.)
        
        print "For source %s in sourcefile %s, S/N = %s" % (source.name,filename,totalsn)
