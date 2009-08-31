#!/usr/bin/env python

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

__version__='$Id$'

import lisaxml2
lisaxml2.Table.DisableRemoteTableLoad = True
# lisaxml2.Stream.MapStreamsFromDisk = True

import sys, string, tempfile
import numpy

from optparse import OptionParser

# TO DO: keep in mind that the alignment may be off if mod(ts.TimeOffset - begtime,ts.Cadence) != 0
def extend(tdi,begtime,endtime):
    ts = tdi.TimeSeries
    
    if ts.TimeOffset > begtime or endtime > ts.TimeOffset + ts.Cadence * ts.Array.Length:
        # figure out the new length, and its boundaries in the old array
        newlength = int( (endtime - begtime) / ts.Cadence )
        
        begindex  = int( (ts.TimeOffset - begtime) / ts.Cadence )
        endindex  = int( (ts.TimeOffset + ts.Cadence * ts.Array.Length - begtime) / ts.Cadence )
        
        # allocate new arrays
        if lisaxml2.Stream.MapStreamsFromDisk:
            obsarray = numpy.memmap(filename=tempfile.TemporaryFile(),dtype='d',mode='w+',shape=(newlength,ts.Array.Records))
        else:
            obsarray = numpy.zeros((newlength,ts.Array.Records),'d')
        
        # loop over observables, redo time axis, copy other observables
        for ind,obs in enumerate(map(string.strip,ts.name.split(','))):
            if obs == 't':
                obsarray[:,ind] = numpy.linspace(begtime,endtime-ts.Cadence,newlength)
            else:
                obsarray[begindex:endindex,ind] = getattr(ts,obs)
        
        # create a new TimeSeries with the extended observables, and replace it in the Observable
        newts = lisaxml2.TimeSeries(obsarray,ts.name,Cadence=ts.Cadence,TimeOffset=begtime)    
        newts.checkContent()
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
        
        assert sys.getrefcount(ts) == 2
        del ts


# TO DO: is keeping the same TimeOffset appropriate for lisasim datasets?
def upsample(tdi,mincadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence > mincadence:
        if ts.Cadence % mincadence != 0:
            print "Cannot upsample cadence %s to noncommensurable cadence %s." % (ts.Cadence,mincadence)
            sys.exit(1)
        
        # figure out the new length (and it better be even)
        newlength = int(ts.Array.Length * (ts.Cadence / mincadence))
        
        if newlength % 2 != 0 or ts.Array.Length % 2 != 0:
            print "mergeXML.py (upsample): Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        # allocate new arrays
        if lisaxml2.Stream.MapStreamsFromDisk:
            obsarray = numpy.memmap(filename=tempfile.TemporaryFile(),dtype='d',mode='w+',shape=(newlength,ts.Array.Records))
        else:
            obsarray = numpy.zeros((newlength,ts.Array.Records),'d')
        
        # loop over observables, redo time axis, upsample other observables   
        for ind,obs in enumerate(map(string.strip,ts.name.split(','))):
            if obs == 't':
                obsarray[:,ind] = numpy.linspace(ts.TimeOffset,ts.TimeOffset + ts.Duration - mincadence,newlength)
            else:
                # FFT upsample
                newfft = numpy.zeros(newlength/2 + 1,'complex128')
                newfft[0:(ts.Array.Length/2 + 1)] = numpy.fft.rfft(getattr(ts,obs))
                newfft[ts.Array.Length/2] *= 0.5
                
                newfft[:] *= (ts.Cadence / mincadence)
                obsarray[:,ind] = numpy.fft.irfft(newfft)
        
        # create a new TimeSeries with the upsampled observables, and replace it in the Observable
        newts = lisaxml2.TimeSeries(obsarray,ts.name,Cadence=mincadence,TimeOffset=ts.TimeOffset)    
        newts.checkContent()
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
                
        assert sys.getrefcount(ts) == 2
        del ts


# TO DO: why not merge the code with upsample?
def downsample(tdi,maxcadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence < maxcadence:
        if maxcadence % ts.Cadence != 0:
            print "Cannot downsample cadence %s to noncommensurable cadence %s." % (ts.Cadence,maxcadence)
            sys.exit(1)
        
        # figure out the new length (maxcadence is larger, so we're reducing it)
        newlength = int(ts.Array.Length / (maxcadence / ts.Cadence))
        newlength += (newlength % 2)    # force even number of points
        
        if ts.Array.Length % 2 != 0:
            print "mergeXML.py (downsample): Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        # allocate new arrays
        obsarray = numpy.zeros((newlength,ts.Array.Records),'d')
        
        for ind,obs in enumerate(map(string.strip,ts.name.split(','))):
            if obs == 't':
                obsarray[:,ind] = numpy.linspace(ts.TimeOffset,ts.TimeOffset + ts.Duration - maxcadence,newlength)
            else:
                # FFT downsample
                newfft = numpy.zeros(newlength/2 + 1,'complex128')                
                rfft = numpy.fft.rfft(getattr(ts,obs))
                newfft[:] = rfft[0:(newlength/2 + 1)]
                newfft[-1] *= 2.0
                
                # the normalization always follows from multiplying by the Delta t
                obsarray[:,ind] = numpy.fft.irfft(newfft) / (maxcadence / ts.Cadence)
            
        # create a new TimeSeries with the upsampled observables, and replace it in the Observable
        newts = lisaxml2.TimeSeries(obsarray,ts.name,Cadence=maxcadence,TimeOffset=ts.TimeOffset)    
        newts.checkContent()
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
        
        assert sys.getrefcount(ts) == 2
        del ts


# set up command-line options

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

# parser.add_option("-C", "--cadence",
#                   type="float", dest="cadence", default=None,
#                   help="forced Cadence of final output [off by default]")

parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default=None,
                  help="outputfile [normally just the same as MERGED.xml]")

parser.add_option("-U", "--upsample",
                  action="store_true", dest="upsample", default=False,
                  help="upsample all time series to the fastest one [default is to downsample to the slowest]")

(options, args) = parser.parse_args()

# TO DO: is this true? Currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least one output file and one input file!")

if options.nokey and options.keyonly:
    parser.error("Conflicting options --keyOnly and --noKey.")

mergedfile = args[0]
inputfiles = args[1:]

# note that currently mergedfile must already exist!

mergedtdifile = lisaxml2.readXML(mergedfile)

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
types = []
for sec in mergedtdifile:
    if sec.Type in ['NoiseData','Simulate','LISACode'] and sec.Type not in types:
        extrasecs.append(sec)
        types.append(sec.Type)

# take author and comments, if any, from MERGED.xml

author = getattr(mergedtdifile,'Author',None)
if not author:
    author = 'Michele Vallisneri (through mergeXML.py)'

# it would be nice also to collect comments from the additional files...
# but lisaxml2.py does not currently support this!
comments = mergedtdifile.Comment

mergedtdifile.close()

if options.outputfile:
    newmergedtdifile = lisaxml2.lisaXML(options.outputfile,author=author,comments=comments)
else:
    newmergedtdifile = lisaxml2.lisaXML(mergedfile,author=author,comments=comments)

if not options.nokey:
    for source in sources:
        newmergedtdifile.SourceData(source)

for inputfile in inputfiles:
    # print "Dealing with", inputfile
    inputtdifile = lisaxml2.readXML(inputfile)

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
            obsname = thistdi.TimeSeries.name
            
            if not (obsname in tdin):
                tdid[obsname] = thistdi
                tdin.append(obsname)    # to keep them in order...
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
                maxend    = max(thistdi.TimeSeries.TimeOffset + thistdi.TimeSeries.Cadence * thistdi.TimeSeries.Array.Length,
                                tdi.TimeSeries.TimeOffset + tdi.TimeSeries.Cadence * tdi.TimeSeries.Array.Length)
                
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

    for sec in inputtdifile:
        if sec.Type in ['NoiseData','Simulate','LISACode'] and sec.Type not in types:
            extrasecs.append(sec)
            types.append(sec.Type)
    
    inputtdifile.close()

# do all the other sections

if lisa:
    newmergedtdifile.LISAData(lisa)

for name in tdin:
    if options.tdiName:
        tdid[name].Name = options.tdiName
    
    newmergedtdifile.TDIData(tdid[name])

# add the first instance of every extra section
if not options.nokey:
    for sec in extrasecs:
        # TO DO: lisaxml2 is quirky, I know... this should be fixed
        if sec.Type == 'NoiseData':
            if len(sec) == 0:
                continue
            else:
                newmergedtdifile.__dict__['NoiseData'] = sec
        newmergedtdifile.append(sec)

newmergedtdifile.close()

sys.exit(0)
