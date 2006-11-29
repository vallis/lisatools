#!/usr/bin/env python

__version__='$Id: $'

import lisaxml
from lisaxml.convertunit import convertUnit

import synthlisa

import numpy
import numpy.oldnumeric as Numeric
import sys
import math

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] BARYCENTRIC.xml OUTPUT.xml",
                      version="$Id:  stasbabak $")

parser.add_option("-r", "--rescaleFactor",
                  type="float", dest="factor", default=None,
                  help="rescaling factor for waveform amplitude Anew = factor * A  [required]")
                                    
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

# for the moment, support a single input barycentric file

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args


if options.factor == None:
    parser.error("specify rescaling factor")

inputXML = lisaxml.readXML(inputfile)
waveforms = inputXML.getLISASources()[0]
inputXML.close()

if options.verbose:
    for p in waveforms.parameters:
        print p, ':', waveforms.parstr(p)

if not hasattr(waveforms,'TimeSeries'):
    print "Script %s needs to be fed barycentric-strain files!" % sys.argv[0]
    sys.exit(1)

ts = waveforms.TimeSeries

#print Numeric.shape(ts.Arrays)

#print (ts.Arrays[0])[:2], (ts.Arrays[1])[:2]
ts.Arrays[0] *= options.factor
ts.Arrays[1] *= options.factor
#print (ts.Arrays[0])[:2], (ts.Arrays[1])[:2]

# check that everything is in units that we can understand
if hasattr(waveforms,'Distance'):
    waveforms.Distance = waveforms.Distance/options.factor
else:
    print "Could not find the distance filed in xml file"
    sys.exit(1)

outputXML = lisaxml.lisaXML(outputfile,author='Michele Vallisneri')
outputXML.SourceData(waveforms)
outputXML.close()

