#!/usr/bin/env python

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

__version__='$Id$'

import lisaxml2

lisaxml2.Table.DisableRemoteTableLoad = True

import sys, os, string, tempfile
import numpy

from optparse import OptionParser

def obsnames(name):
    return map(string.strip,name.split(','))

def firstTDI(ts):
    if obsnames(ts.name)[0] == 't':
        return 1
    else:
        return 0


def chunk(arraylen,chunklen,begin=0):
    """This iterator returns the array slices necessary to break up
    a copy or arithmetic operation of length arraylen into chunks
    of length chunklen."""
    
    iterate, remain = divmod(arraylen,chunklen)
    
    for i in range(iterate):
        yield slice(begin+i*chunklen,begin+(i+1)*chunklen)
    
    if remain > 0:
        yield slice(begin + arraylen - remain,begin + arraylen)    

def chunks(arraylen,chunklen,begin1=0,begin2=0):
    """Same as chunk, but supports different starting indices in the
    left- and right-hand arrays."""
    
    iterate, remain = divmod(arraylen,chunklen)
    
    for i in range(iterate):
        yield slice(begin1+i*chunklen,begin1+(i+1)*chunklen), slice(begin2+i*chunklen,begin2+(i+1)*chunklen)
    
    if remain > 0:
        yield (slice(begin1 + arraylen - remain,begin1 + arraylen),
               slice(begin2 + arraylen - remain,begin2 + arraylen))


def setslice(a1,a2,col):
    """If a1 is a regular array, sets column col of a1 to the contents of a2.
    If it is a memmap array, does the same by working in chunks of
    lisaxml2.Stream.BufferLength, and repeated flushing."""
    
    if lisaxml2.Stream.MapStreamsFromDisk and isinstance(a1,numpy.memmap):
        for s in chunk(len(a2),lisaxml2.Stream.BufferLength):
            a1[s,col] = a2[s]
            a1.flush()
    else:
        a1[:,col] = a2[:]

def getslice(a1,col):
    """For a regular numpy array a1, returns a view corresponding to column col.
    For a memmap array, returns a copy of the column created by accessing the
    memmapped file in chunks of lisaxml2.Stream.BufferLength, and repeatedly
    flushing."""
    
    if lisaxml2.Stream.MapStreamsFromDisk and isinstance(a1,numpy.memmap):
        tmp = numpy.zeros(len(a1),'d')    
        
        for s in chunk(len(a1),lisaxml2.Stream.BufferLength):
            tmp[s] = a1[s,col]
            a1.flush()
        
        return tmp
    else:
        return a1[:,col]


def makeTimeSeries(name,cadence,begtime,length,records,datatype):
    if lisaxml2.Stream.MapStreamsFromDisk:
        obsarray = numpy.memmap(filename=tempfile.TemporaryFile(),dtype='d',mode='w+',shape=(length,records))
    else:
        obsarray = numpy.zeros((length,records),'d')
    
    stepoffset = cadence * ((datatype == 'Strain') and 0.5 or 0)
    endtime = begtime + cadence * length
    
    # fill the time axis if present (works only in the first position)
    if obsnames(name)[0] == 't':
        if lisaxml2.Stream.MapStreamsFromDisk:
            for s in chunk(length,lisaxml2.Stream.BufferLength):
                bt = begtime +  s.start     * cadence + stepoffset
                et = begtime + (s.stop - 1) * cadence + stepoffset
                
                obsarray[s,0] = numpy.linspace(bt,et,lisaxml2.Stream.BufferLength)
                obsarray.flush()
        else:
            obsarray[:,0] = numpy.linspace(begtime + stepoffset,endtime - cadence + stepoffset,length)
    
    ts = lisaxml2.TimeSeries(obsarray,name,Cadence=cadence,TimeOffset=begtime)    
    ts.checkContent()
    
    return ts

def copyTimeSeries(ts1,ts2,begin1=0,end1=None,begin2=0,end2=None,add=False,sub=False):
    a1, a2 = ts1.Array.Stream.Data, ts2.Array.Stream.Data
    
    if end1 == None: end1 = len(a1)
    if end2 == None: end2 = len(a2)
    
    if ts1.name != ts2.name:
        raise ValueError, "copyTimeSeries(): trying to copy different observables"
    
    if (end1 - begin1) != (end2 - begin2) or a1.shape[1] != a2.shape[1]:
        raise IndexError, "copyTimeSeries(): trying to copy mismatched arrays."
    
    if add and sub:
        raise AttributeError, "copyTimeSeries(): adding and subtracting TimeSeries are incompatible operations."
    
    skip = firstTDI(ts1)
    
    if lisaxml2.Stream.MapStreamsFromDisk and (isinstance(a1,numpy.memmap) or isinstance(a2,numpy.memmap)):
        for s1,s2 in chunks(end2 - begin2,lisaxml2.Stream.BufferLength,begin1,begin2):
            if add:
                a1[s1,skip:] += a2[s2,skip:]
            elif sub:
                a1[s1,skip:] -= a2[s2,skip:]
            else:
                a1[s1,skip:]  = a2[s2,skip:]
            
            if isinstance(a1,numpy.memmap): a1.flush()
            if isinstance(a2,numpy.memmap): a2.flush()
    else:
        if add:
            a1[begin1:end1,skip:] += a2[begin2:end2,skip:]
        elif sub:
            a1[begin1:end1,skip:] -= a2[begin2:end2,skip:]
        else:
            a1[begin1:end1,skip:]  = a2[begin2:end2,skip:]


def extend(tdi,begtime,endtime):
    """Extend TimeSeries array to have limits [begtime,endtime).
    Assumes begtime <= ts->begtime, endtime >= ts->endtime."""
    
    ts = tdi.TimeSeries
    
    ts_begtime = ts.TimeOffset
    ts_endtime = ts.TimeOffset + ts.Cadence * ts.Array.Length
        
    if begtime < ts_begtime or endtime > ts_endtime:
        if options.debug: print "---> Extending time axis: (%s,%s] -> (%s,%s]" % (ts_begtime,ts_endtime,begtime,endtime)
        
        if (ts.TimeOffset - begtime) % ts.Cadence > 1e-6:
            print "mergeXML.py (extend): WARNING! I'm fudging the time axis (t0: %s -> %s s, offset by %s)." % (ts_begtime,begtime,(ts.TimeOffset - begtime) % ts.Cadence)
        
        # figure out the new length and the boundaries of the old array in the new one
        newlength = int( (endtime - begtime) / ts.Cadence )
        
        begindex  = int( (ts_begtime - begtime) / ts.Cadence )
        endindex  = int( (ts_endtime - begtime) / ts.Cadence )
        
        # fill the new array with zeros and with the correct time axis
        newts = makeTimeSeries(ts.name,ts.Cadence,begtime,newlength,ts.Array.Records,tdi.DataType)
        
        # copy old array, and replace it in the TimeSeries
        copyTimeSeries(newts,ts,begin1=begindex,end1=endindex)
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
        
        del ts

def upsample(tdi,mincadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence > mincadence:
        if ts.Cadence % mincadence != 0:
            print "mergeXML.py (upsample): Cannot upsample %s-s data to %s-s cadence." % (ts.Cadence,mincadence)
            sys.exit(1)
        
        # figure out the new length (and it better be even)
        newlength = int(ts.Array.Length * (ts.Cadence / mincadence))
        
        if newlength % 2 != 0 or ts.Array.Length % 2 != 0:
            print "mergeXML.py (upsample): Don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        newts = makeTimeSeries(ts.name,mincadence,ts.TimeOffset,newlength,ts.Array.Records,tdi.DataType)
        
        newfft = numpy.zeros(newlength/2 + 1,'complex128')
        
        for c in range(firstTDI(ts),ts.Array.Records):
            # FFT upsample
            newfft[0:(ts.Array.Length/2 + 1)] = numpy.fft.rfft(getslice(ts.Array.Stream.Data,c))
            newfft[ts.Array.Length/2] *= 0.5  
            newfft[(ts.Array.Length/2 + 1):] = 0.0
            
            newfft[:] *= (ts.Cadence / mincadence)
            
            setslice(newts.Array.Stream.Data,numpy.fft.irfft(newfft),c)
                
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
                
        del ts

def downsample(tdi,maxcadence):
    ts = tdi.TimeSeries
    
    if ts.Cadence < maxcadence:
        if options.debug: print "---> Downsampling: %s -> %s" % (ts.Cadence,maxcadence)        
        
        if maxcadence % ts.Cadence != 0:
            print "Cannot downsample cadence %s to noncommensurable cadence %s." % (ts.Cadence,maxcadence)
            sys.exit(1)
        
        # figure out the new length (maxcadence is larger, so we're reducing it)
        newlength = int(ts.Array.Length / (maxcadence / ts.Cadence))
        newlength += (newlength % 2)    # force even number of points
        
        if ts.Array.Length % 2 != 0:
            print "mergeXML.py (downsample): Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        newts = makeTimeSeries(ts.name,maxcadence,ts.TimeOffset,newlength,ts.Array.Records,tdi.DataType)
        
        # allocate new arrays
        obsarray = numpy.zeros((newlength,ts.Array.Records),'d')
        
        newfft = numpy.zeros(newlength/2 + 1,'complex128')
        
        for c in range(firstTDI(ts),ts.Array.Records):
            # FFT downsample
            rfft = numpy.fft.rfft(getslice(ts.Array.Stream.Data,c))
            newfft[:] = rfft[0:(newlength/2 + 1)]
            newfft[-1] *= 2.0
            
            # the normalization always follows from multiplying by the Delta t
            newfft[:] /= (maxcadence / ts.Cadence)
            
            setslice(newts.Array.Stream.Data,numpy.fft.irfft(newfft),c)
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        tdi.checkContent()
        
        del ts


# set up command-line options

parser = OptionParser(usage="usage: %prog [options] MERGED.xml TDIFILE-1.xml TDIFILE-2.xml ...",
                      version="$Id$")

parser.add_option("-k", "--keyOnly",
                  action="store_true", dest="keyonly", default=False,
                  help="get only SourceData information from TDIFILE-X.xml [off by default]")

parser.add_option("-n", "--noKey",
                  action="store_true", dest="nokey", default=False,
                  help="remove all SourceData information [off by default]")

parser.add_option("-s", "--subtract",
                  action="store_true", dest="subtract", default=False,
                  help="subtract (not add) TDI columns [off by default]")

parser.add_option("-N", "--tdiName",
                  type="string", dest="tdiName", default=None,
                  help="use this string as the name of TDIobservable sections [off by default]")

parser.add_option("-M", "--conserveMemory",
                  action="store_true", dest="conserveMemory", default=False,
                  help="attempt to conserveMemory by buffering TimeSeries on disk [off by default]")

parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default=None,
                  help="outputfile [normally just the same as MERGED.xml]")

parser.add_option("-U", "--upsample",
                  action="store_true", dest="upsample", default=False,
                  help="upsample all time series to the fastest one [default is to downsample to the slowest]")

parser.add_option("-c", "--cadence",
                  type="float", dest="cadence", default=None,
                  help="if used in conjunction with upsample, enforce this Cadence [default is None]")

parser.add_option("-d", "--debug",
                  action="store_true", dest="debug", default=False,
                  help="toggle debugging information [off by default]")

(options, args) = parser.parse_args()

# TO DO: is this true? Currently we support only a single source parameter file

if len(args) < 2:
    parser.error("You must specify at least one output file and one input file!")

if options.nokey and options.keyonly:
    parser.error("Conflicting options --keyOnly and --noKey.")

if options.conserveMemory:
    lisaxml2.Stream.MapStreamsFromDisk = True
    lisaxml2.Stream.BufferLength = 4194304    

mergedfile = args[0]
inputfiles = args[1:]

# note that currently mergedfile must already exist!

if options.debug: print "---> Processing", mergedfile
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

if not author and 'USER' in os.environ:
    author = os.environ['USER']
else:
    author = 'mergeXML.py'

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
    if options.debug: print "---> Processing", inputfile
    inputtdifile = lisaxml2.readXML(inputfile)

    # will use the first LISA that it finds anywhere...

    if not lisa:
        lisa = inputtdifile.getLISAgeometry()
    
    if not options.nokey:
        sources = inputtdifile.getLISASources()
        
        for source in sources:
            if hasattr(source,'TimeSeries'):
                source.remove(source.TimeSeries)
                del source.TimeSeries
            
            if options.subtract:
                newmergedtdifile.SourceData(source,comments='Subtracted source')
            else:
                newmergedtdifile.SourceData(source)
    
    if not options.keyonly:
        alltdi = inputtdifile.getTDIObservables()
        
        for thistdi in alltdi:
            obsname = thistdi.TimeSeries.name
            
            if not (obsname in tdin):   # create an accumulator TimeSeries
                tdin.append(obsname)
                tdid[obsname] = lisaxml2.Observable(name=thistdi.name,datatype=thistdi.DataType,
                                                    timeseries=makeTimeSeries(obsname,
                                                                              thistdi.TimeSeries.Cadence,thistdi.TimeSeries.TimeOffset,
                                                                              thistdi.TimeSeries.Array.Length,thistdi.TimeSeries.Array.Records,
                                                                              thistdi.DataType))                
            
            tdi = tdid[obsname]
            
            if tdi.DataType != thistdi.DataType:
                print "Script %s finds mismatched DataType TDI TimeSeries in file %s." % (sys.argv[0],inputfile)
                sys.exit(1)
            
            if tdi.TimeSeries.name != thistdi.TimeSeries.name:
                print tdi.TimeSeries.name, thistdi.TimeSeries.name
                print "Script %s finds mismatched observables in file %s." % (sys.argv[0],inputfile)
                sys.exit(1)
            
            # upsample or downsample
            
            if options.upsample:
                mincadence = (options.cadence != None) and options.cadence or min(thistdi.TimeSeries.Cadence,tdi.TimeSeries.Cadence)
                
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
                        
            # add or subtract tdi observables to accumulator arrays
            if options.subtract:
                copyTimeSeries(tdi.TimeSeries,thistdi.TimeSeries,begin1=0,end1=None,begin2=0,end2=None,sub=True)
            else:
                copyTimeSeries(tdi.TimeSeries,thistdi.TimeSeries,begin1=0,end1=None,begin2=0,end2=None,add=True)

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
