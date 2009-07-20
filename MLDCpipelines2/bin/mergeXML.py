#!/usr/bin/env python

__version__='$Id$'

import lisaxml
import sys, string
import numpy

def extend(tdi,begtime,endtime):
    ts = tdi.TimeSeries
    
    # TO DO: keep in mind that the alignment maybe off if mod(ts.TimeOffset - begtime,ts.Cadence) != 0
    #        should warn about this?
    if ts.TimeOffset > begtime or endtime > ts.TimeOffset + ts.Cadence * ts.Length:
        newlength = int( (endtime - begtime) / ts.Cadence )
        
        begindex  = int( (ts.TimeOffset - begtime) / ts.Cadence )
        endindex  = int( (ts.TimeOffset + ts.Cadence * ts.Length - begtime) / ts.Cadence )
        
        ts.Arrays = []
        for obs in map(string.strip,ts.name.split(',')):            
            if obs == 't':
                newobs = numpy.linspace(begtime,endtime-ts.Cadence,newlength)
            else:
                newobs = numpy.zeros(newlength,'d')
                newobs[begindex:endindex] = getattr(ts,obs)
            
            # the following is appropriate for lisaxml (not lisaxml2)
            setattr(tdi,obs,newobs)
            setattr(ts,obs,newobs)
            ts.Arrays.append(newobs)
        
        ts.TimeOffset = begtime
        ts.Length = newlength
        ts.Duration = ts.Length * ts.Cadence

def upsample(tdi,mincadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence > mincadence:
        if ts.Cadence % mincadence != 0:
            print "Cannot upsample cadence %s to noncommensurable cadence %s." % (ts.Cadence,mincadence)
            sys.exit(1)
        
        newlength = int(ts.Length * (ts.Cadence / mincadence))
        
        if newlength % 2 != 0 or ts.Length % 2 != 0:
            print "Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        ts.Arrays = []
        for obs in map(string.strip,ts.name.split(',')):
            if obs == 't':
                newobs = numpy.linspace(ts.TimeOffset,ts.TimeOffset + ts.Duration - mincadence,newlength)
            else:
                newfft = numpy.zeros(newlength/2 + 1,'complex128')
                
                # FFT upsample
                newfft[0:(ts.Length/2 + 1)] = numpy.fft.rfft(getattr(ts,obs))
                newfft[ts.Length/2] *= 0.5
                newobs = numpy.fft.irfft(newfft) * (ts.Cadence / mincadence)
            
            # the following is appropriate for lisaxml (not lisaxml2)
            setattr(tdi,obs,newobs)
            setattr(ts,obs,newobs)
            ts.Arrays.append(newobs)
        
        ts.Cadence = mincadence
        ts.Length = newlength
        ts.Duration = ts.Length * ts.Cadence

def downsample(tdi,maxcadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence > maxcadence:
        if maxcadence % ts.Cadence != 0:
            print "Cannot downsample cadence %s to noncommensurable cadence %s." % (ts.Cadence,maxcadence)
            sys.exit(1)
        
        newlength = int(ts.Length / (ts.Cadence / maxcadence))
        
        if newlength % 2 != 0 or ts.Length % 2 != 0:
            print "Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        ts.Arrays = []
        for obs in map(string.strip,ts.name.split(',')):
            if obs == 't':
                newobs = numpy.linspace(ts.TimeOffset,ts.TimeOffset + ts.Duration - maxcadence,newlength)
            else:
                newfft = numpy.zeros(newlength/2 + 1,'complex128')
                
                # FFT downsample
                rfft = numpy.fft.rfft(getattr(ts,obs))
                newfft[:] = rfft[0:(newlength/2 + 1)]
                newfft[-1] *= 2.0
                newobs = numpy.fft.irfft(newfft) / (ts.Cadence / maxcadence)
            
            # the following is appropriate for lisaxml (not lisaxml2)
            setattr(tdi,obs,newobs)
            setattr(ts,obs,newobs)
            ts.Arrays.append(newobs)
        
        ts.Cadence = maxcadence
        ts.Length = newlength
        ts.Duration = ts.Length * ts.Cadence

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

parser.add_option("-N", "--tdiName",
                  type="string", dest="tdiName", default=None,
                  help="use this string as the name of TDIobservable sections [off by default]")

parser.add_option("-C", "--cadence",
                  type="float", dest="cadence", default=None,
                  help="forced Cadence of final output [off by default]")

parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default=None,
                  help="outputfile [normally just the same as MERGED.xml]")

parser.add_option("-U", "--upsample",
                  action="store_true", dest="upsample", default=False,
                  help="upsample all time series to the fastest one [default is to downsample to the slowest]")

(options, args) = parser.parse_args()

# currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least one output file and one input file!")

if options.nokey and options.keyonly:
    parser.error("Conflicting options --keyOnly and --noKey.")

mergedfile = args[0]
inputfiles = args[1:]

# note that currently mergedfile must already exist!

mergedtdifile = lisaxml.readXML(mergedfile)

lisa = mergedtdifile.getLISAgeometry()

if not options.nokey:
    sources = mergedtdifile.getLISASources()

tdid = {}
tdin = []

alltdi = mergedtdifile.getTDIObservables()

for thistdi in alltdi:
    obsname = thistdi.TimeSeries.name
    if not (obsname in tdin):
        tdid[obsname] = thistdi
        tdin.append(obsname)

extrasecs = []
for sec in ['NoiseData','Simulate','LISACode']:
    if mergedtdifile.getExtraSection(sec):
        extrasecs.append(mergedtdifile.getExtraSection(sec))

# take author and comments, if any, from MERGED.xml

author = mergedtdifile.Author
if not author:
    author = 'Michele Vallisneri (through mergeXML.py)'

# it would be nice also to collect comments from the additional files...
# but lisaxml.py does not currently support this!
comments = mergedtdifile.Comment

mergedtdifile.close()

if options.outputfile:
    newmergedtdifile = lisaxml.lisaXML(options.outputfile,author=author,comments=comments)
else:
    newmergedtdifile = lisaxml.lisaXML(mergedfile,author=author,comments=comments)

if not options.nokey:
    for source in sources:
        newmergedtdifile.SourceData(source)

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
        ## Problem is here
        alltdi = inputtdifile.getTDIObservables()

        for thistdi in alltdi:
            obsname = thistdi.TimeSeries.name
            
            if not (obsname in tdin):
                tdid[obsname] = thistdi
                # to keep them in order...
                tdin.append(obsname)
            else:
                tdi = tdid[obsname]
                
                if tdi.DataType != thistdi.DataType:
                    print "Script %s finds mismatched DataType TDI TimeSeries in file %s." % (sys.argv[0],inputfile)
                    sys.exit(1)
                
                if tdi.TimeSeries.name != thistdi.TimeSeries.name:
                    print "Script %s finds mismatched observables in file %s." % (sys.argv[0],inputfile)
                    sys.exit(1)
                
                # upsample or downsample
                
                if options.upsample:
                    mincadence = min(thistdi.TimeSeries.Cadence,tdi.TimeSeries.Cadence)
                    
                    upsample(tdi,    mincadence)
                    upsample(thistdi,mincadence)
                else:
                    maxcadence = max(thistdi.TimeSeries.Cadence,tdi.TimeSeries.Cadence)
                    
                    downsample(tdi,    maxcadence)
                    downsample(thistdi,maxcadence)    
                
                # extend datasets as needed to sum them properly
                
                minoffset = min(thistdi.TimeSeries.TimeOffset,
                                tdi.TimeSeries.TimeOffset)
                maxend    = max(thistdi.TimeSeries.TimeOffset + thistdi.TimeSeries.Cadence * thistdi.TimeSeries.Length,
                                tdi.TimeSeries.TimeOffset + tdi.TimeSeries.Cadence * tdi.TimeSeries.Length)
                
                extend(tdi,    minoffset,maxend)
                extend(thistdi,minoffset,maxend)
                
                # add tdi observables to accumulator arrays
                
                for name in obsname.split(','):
                    obs = name.strip()
                    
                    if obs != 't':
                        tdio = getattr(tdi,obs)
                        thistdio = getattr(thistdi,obs)
                        
                        if options.subtract:
                            tdio -= thistdio
                        else:
                            tdio += thistdio
    
    for sec in ['Simulate','LISACode','NoiseData']:
        if inputtdifile.getExtraSection(sec):
            extrasecs.append(inputtdifile.getExtraSection(sec))
    
    inputtdifile.close()

# do all the other sections

if lisa:
    newmergedtdifile.LISAData(lisa)

for name in tdin:
    if options.tdiName:
        newmergedtdifile.TDIData(tdid[name],name=options.tdiName)
    else:
        newmergedtdifile.TDIData(tdid[name])

if not options.nokey:
    for sec in extrasecs:
        newmergedtdifile.ExtraSection(sec)

newmergedtdifile.close()

sys.exit(0) 
           
