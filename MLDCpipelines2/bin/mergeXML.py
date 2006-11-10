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
                  help="output only SourceData information [off by default]")

parser.add_option("-n", "--noKey",
                  action="store_true", dest="nokey", default=False,
                  help="do not include SourceData information [off by default]")

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

if not options.nokey:
    sources = mergedtdifile.getLISASources()

if not options.keyonly:
    try:
        tdi = mergedtdifile.getTDIObservables()[0]
    except IndexError:
        tdi = None        

mergedtdifile.close()

for inputfile in inputfiles:
    inputtdifile = lisaxml.readXML(inputfile)
    
    if not options.nokey:
        sources = sources + inputtdifile.getLISASources()

    if not options.keyonly:
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
                elif tdi.DataType == 'EquivalentStrain':
                    tdi.t  += thistdi.t
                    tdi.Xs += thistdi.Xs
                    tdi.Ys += thistdi.Ys
                    tdi.Zs += thistdi.Zs
                else:
                    raise
            except:
                print "Script %s can't find standard TDI observables in file %s." % (sys.argv[0],xmlfile)
                sys.exit(1)

newmergedtdifile = lisaxml.lisaXML(mergedfile)

if not options.nokey:
    for source in sources:
        newmergedtdifile.SourceData(source)

if not options.keyonly:
    newmergedtdifile.TDIData(tdi)
    
mergedtdifile.close()

sys.exit(0) 
           
