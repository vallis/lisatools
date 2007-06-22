#!/usr/bin/env python

__version__='$Id: mergeXML.py 401 2007-05-23 20:22:23Z vallisneri $'

import lisaxml
import sys
import numpy

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] MERGED.xml TDIFILE-1.xml TDIFILE-2.xml ...",
                      version="$Id: mergeXML.py 401 2007-05-23 20:22:23Z vallisneri $")

(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least the output file and one input file!")

inputfile  = args[0]
outputfile = args[1]

inputtdifile = lisaxml.readXML(inputfile)

lisa = inputtdifile.getLISAgeometry()

sources = inputtdifile.getLISASources()

tdi = inputtdifile.getTDIObservables()[0]

# take author and comments, if any, from MERGED.xml

author = inputtdifile.Author
if not author:
    author = 'Michele Vallisneri (through mergeXML.py)'

comments = inputtdifile.Comment

inputtdifile.close()

outputtdifile = lisaxml.lisaXML(outputfile,author=author,comments=comments)

if lisa:
    outputtdifile.LISAData(lisa)

if sources:
    for source in sources:
        outputtdifile.SourceData(source)

# this is rough, and probably not very general...

datalen = len(tdi.t)

t = tdi.TimeSeries.Cadence * numpy.arange(0,datalen)

Xf = numpy.zeros(datalen,'d')
Yf = numpy.zeros(datalen,'d')
Zf = numpy.zeros(datalen,'d')

norm = (1e10 / 299792458.0) / 15.0

Xf[1:datalen] = norm * (tdi.Xs[1:datalen] - tdi.Xs[0:(datalen-1)])
Yf[1:datalen] = norm * (tdi.Ys[1:datalen] - tdi.Ys[0:(datalen-1)])
Zf[1:datalen] = norm * (tdi.Zs[1:datalen] - tdi.Zs[0:(datalen-1)])

Xf[0] = Xf[1]
Yf[0] = Yf[1]
Zf[0] = Zf[1]

if tdi.DataType == 'Strain':
    tdinew = lisaxml.Observable('t,Xf,Yf,Zf')

    tdinew.DataType = 'FractionalFrequency'

    tdinew.TimeSeries = lisaxml.TimeSeries([t,Xf,Yf,Zf],'t,Xf,Yf,Zf')
    tdinew.TimeSeries.Cadence = tdi.TimeSeries.Cadence
    tdinew.TimeSeries.TimeOffset = tdi.TimeSeries.TimeOffset

    outputtdifile.TDIData(tdinew)
else:
    outputtdifile.TDIData(tdi)

outputtdifile.close()
    
sys.exit(0) 
           
