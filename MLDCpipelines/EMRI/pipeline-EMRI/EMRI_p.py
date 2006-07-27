#!/usr/bin/env python

from synthlisa import *
from Numeric import *

import sys, getopt
import re

import math

# read from XML

#if len(sys.argv) < 2:
#    print "Usage: %s input.xml" % sys.argv[0]
#    print "       will generate input_Barycenter.xml"

#inputfile = sys.argv[1]

def usage():
     print >>sys.stderr, """
  Usage: BBH_p [options]

  -h, --help       display this message
  -f, --xmlFile    input XML file with source parameters

"""

shortop = "hf:"
longop = [
     "help",
     "xmlFile="]

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
     print >> sys.stderr, "You must specify the xml file"
     usage()
     sys.exit(1)

try:
   sfile = open(inputfile, 'r')
except IOError, args:
   print "cannot open source file ", inputile
   sys.exit(1)
sfile.close()


outputfile = re.sub('.xml$','_Barycenter.xml',inputfile)

# read the first source from the file

inputXML = readXML(inputfile)
mysystem = inputXML.getLISASources()[0]
inputXML.close()

# dump raw binary strains to array

stime = 15.0

inittime = -900.0
prebuffer = -inittime

# 2 years of data
samples = 2**22 + 2*int(prebuffer/stime)

hp = getobsc(samples,stime,mysystem.hp,inittime)
hc = getobsc(samples,stime,mysystem.hc,inittime)

# get polarization from the symbolic binary to create
# sampled wave compatible with LISA Simulator
# remember that SL polarization is - LS/MLDC polarization

#pol = float(mysystem.Polarization[0])

#hpp =  math.cos(2*pol) * hp + math.sin(2*pol) * hc
#hcp = -math.sin(2*pol) * hp + math.cos(2*pol) * hc

# create interpolated-wave object; use Lagrange-2 interpolation
# set polarization equal to zero since we've applied it already

sampledwave = SampledWave(hp,hc,samples,stime,prebuffer,
                          1.0,None,2,
                          mysystem.initargs[-3],
                          mysystem.initargs[-2],
                          0.0)

# output XML

outputXML = lisaXML(outputfile,author='Michele Vallisneri')
outputXML.SourceData(sampledwave,mysystem.name)
outputXML.close()
