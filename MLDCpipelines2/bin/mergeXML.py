#!/usr/bin/env python

__version__='$Id$'

import lisaxml
import sys

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] MERGED.xml TDIFILE-1.xml TDIFILE-2.xml ...",
                      version="$Id$")

parser.add_option("-k", "--keyOnly",
                  action="store_true", dest="keyonly", default=False,
                  help="get only SourceData information from TDIFILE-X.xml [off by default]")
                  # but preserve and TDIData in MERGED.xml

parser.add_option("-n", "--noKey",
                  action="store_true", dest="nokey", default=False,
                  help="remove all SourceData information [off by default]")

(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least the output file and one input file!")

if options.nokey and options.keyonly:
    parser.error("Conflicting options --keyOnly and --noKey.")

mergedfile = args[0]
inputfiles = args[1:]

# note that currently mergedfile must already exist!

mergedtdifile = lisaxml.readXML(mergedfile)

lisa = mergedtdifile.getLISAgeometry()

if not options.nokey:
    sources = mergedtdifile.getLISASources()

try:
    tdi = mergedtdifile.getTDIObservables()[0]
except IndexError:
    tdi = None        

mergedtdifile.close()

for inputfile in inputfiles:
    inputtdifile = lisaxml.readXML(inputfile)

    # will use the first LISA that it finds anywhere...

    if not lisa:
        lisa = inputtdifile.getLISAgeometry()
    
    if not options.nokey:
        sources = sources + inputtdifile.getLISASources()

    if not options.keyonly:
        try:
            thistdi = inputtdifile.getTDIObservables()[0]

            if tdi == None:
                tdi = thistdi
            else:
                try:
                    assert tdi.DataType == thistdi.DataType
                    assert tdi.TimeSeries.Length     == thistdi.TimeSeries.Length
                    assert tdi.TimeSeries.Cadence    == thistdi.TimeSeries.Cadence
                    assert tdi.TimeSeries.TimeOffset == thistdi.TimeSeries.TimeOffset
                except:
                    print "Script %s finds mismatched DataType, Length, or Cadence for TDI TimeSeries in file %s." % (sys.argv[0],inputfile)
                    sys.exit(1)

                # add tdi observables to accumulator arrays

                try:
                    if tdi.DataType == 'FractionalFrequency':
                        tdi.t  += thistdi.t
                        tdi.Xf += thistdi.Xf
                        tdi.Yf += thistdi.Yf
                        tdi.Zf += thistdi.Zf
                    elif tdi.DataType == 'Strain':
                        tdi.t  += thistdi.t
                        tdi.Xs += thistdi.Xs
                        tdi.Ys += thistdi.Ys
                        tdi.Zs += thistdi.Zs
                    else:
                        raise
                except:
                    print "Script %s can't find standard TDI observables in file %s." % (sys.argv[0],xmlfile)
                    sys.exit(1)
        except:
            pass

newmergedtdifile = lisaxml.lisaXML(mergedfile)

if lisa:
    newmergedtdifile.LISAData(lisa)

if not options.nokey:
    for source in sources:
        newmergedtdifile.SourceData(source)

if tdi:
    newmergedtdifile.TDIData(tdi)
    
mergedtdifile.close()

sys.exit(0) 
           
