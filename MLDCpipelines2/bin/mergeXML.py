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

parser.add_option("-s", "--subtract",
                  action="store_true", dest="subtract", default=False,
                  help="subtract (not add) TDI columns [off by default]")

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

# take author and comments, if any, from MERGED.xml

author = mergedtdifile.Author
if not author:
    author = 'Michele Vallisneri (through mergeXML.py)'

# it would be nice also to collect comments from the additional files...
# but lisaxml.py does not currently support this!
comments = mergedtdifile.Comment

mergedtdifile.close()

newmergedtdifile = lisaxml.lisaXML(mergedfile,author=author,comments=comments)

tdid = {}
tdin = []

for inputfile in inputfiles:
    inputtdifile = lisaxml.readXML(inputfile)

    # will use the first LISA that it finds anywhere...

    if not lisa:
        lisa = inputtdifile.getLISAgeometry()
    
    if not options.nokey:
        sources = inputtdifile.getLISASources()

        for source in sources:
            if hasattr(source,'TimeSeries'):
                del source.TimeSeries

            if options.subtract:
                newmergedtdifile.SourceData(source,comments='Subtracted source')
            else:
                newmergedtdifile.SourceData(source)
    
    if not options.keyonly:
        alltdi = inputtdifile.getTDIObservables()

        for thistdi in alltdi:
            if not (thistdi.name in tdin):
                tdid[thistdi.name] = thistdi
                # to keep them in order...
                tdin.append(thistdi.name)
            else:
                tdi = tdid[thistdi.name]

                try:
                    assert tdi.DataType              == thistdi.DataType
                    assert tdi.TimeSeries.Length     == thistdi.TimeSeries.Length
                    assert tdi.TimeSeries.Cadence    == thistdi.TimeSeries.Cadence
                    assert tdi.TimeSeries.TimeOffset == thistdi.TimeSeries.TimeOffset
                except:
                    print "Script %s finds mismatched DataType, Length, or Cadence for TDI TimeSeries in file %s." % (sys.argv[0],inputfile)
                    sys.exit(1)

                # add tdi observables to accumulator arrays

                for obsname in tdi.name.split(','):
                    obs = obsname.strip()

                    if obs != 't':
                        tdio = getattr(tdi,obs)
                        thistdio = getattr(thistdi,obs)

                        if options.subtract:
                            tdio -= thistdio
                        else:
                            tdio += thistdio

if lisa:
    newmergedtdifile.LISAData(lisa)

if not options.keyonly:
    for name in tdin:
        newmergedtdifile.TDIData(tdid[name])
    
newmergedtdifile.close()

sys.exit(0) 
           
