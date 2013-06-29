#!/usr/bin/env python

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

__version__='$Id: mergeXML.py 1032 2009-08-26 06:27:07Z m.vallis@gmail.com $'

import lisaxml2
import sys, string
import numpy

from optparse import OptionParser

def addobservables(tdi,thistdi,filename):
    if tdi.DataType != thistdi.DataType:
        print "addXML.py: mismatched DataType for observables %s in file %s." % (thistdi.TimeSeries.name,filename)
        sys.exit(1)
                
    if tdi.TimeSeries.name != thistdi.TimeSeries.name:
        print "addXML.py: mismatched observables %s in file %s." % (thistdi.TimeSeries.name,filename)
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
            
            tdio[:] += thistdio[:]


# TO DO: keep in mind that the alignment may be off if mod(ts.TimeOffset - begtime,ts.Cadence) != 0
def extend(tdi,begtime,endtime):
    ts = tdi.TimeSeries
    
    if ts.TimeOffset > begtime or endtime > ts.TimeOffset + ts.Cadence * ts.Array.Length:
        # figure out the new length, and its boundaries in the old array
        newlength = int( (endtime - begtime) / ts.Cadence )
        
        begindex  = int( (ts.TimeOffset - begtime) / ts.Cadence )
        endindex  = int( (ts.TimeOffset + ts.Cadence * ts.Array.Length - begtime) / ts.Cadence )
        
        # allocate new arrays
        obsarray = numpy.zeros((newlength,ts.Array.Records),'d')
        
        # loop over observables, redo time axis, copy other observables
        for ind,obs in enumerate(map(string.strip,ts.name.split(','))):
            if obs == 't':
                obsarray[:,ind] = numpy.linspace(begtime,endtime-ts.Cadence,newlength)
            else:
                obsarray[:,begindex:endindex] = getattr(ts,obs)
        
        # create a new TimeSeries with the extended observables, and replace it in the Observable
        newts = lisaxml.TimeSeries(obsarray,ts.name,Cadence=ts.Cadence,TimeOffset=begtime)    
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        
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
            print "Sorry, I don't know how to deal with odd-point FFTs."
            sys.exit(1)
        
        # allocate new arrays
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
                
                obsarray[:,ind] = numpy.fft.irfft(newfft) * (ts.Cadence / mincadence)
        
        # create a new TimeSeries with the upsampled observables, and replace it in the Observable
        newts = lisaxml.TimeSeries(obsarray,ts.name,Cadence=mincadence,TimeOffset=begtime)    
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        
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
        
        if newlength % 2 != 0 or ts.Array.Length % 2 != 0:
            print "Sorry, I don't know how to deal with odd-point FFTs."
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
        newts = lisaxml.TimeSeries(obsarray,ts.name,Cadence=maxcadence,TimeOffset=begtime)    
        
        tdi.TimeSeries = newts  # this will also remove ts and add newts from/to tdi[]
        
        assert sys.getrefcount(ts) == 2
        del ts


# set up command-line options

parser = OptionParser(usage="Usage: %prog [options] [-o MERGED.xml] TDIFILE-1.xml TDIFILE-2.xml ...\n\n" +
                            "If -o given, sets  MERGED.xml = TDIFILE-1.xml + TDIFILE-2.xml + ...\n" +
                            "otherwise performs TDIFILE-1.xml += TDIFILE-2.xml + ...\n" +
                            "Merges SourceData and TDIData, and uses the first instance of all other sections.",
                      version="$Id: mergeXML.py 1032 2009-08-26 06:27:07Z m.vallis@gmail.com $")

parser.add_option("-k", "--keyOnly",
                  action="store_true", dest="keyonly", default=False,
                  help="ignore sections other than SourceData in TDIFILE-2.xml and on [off by default]")

parser.add_option("-n", "--noKey",
                  action="store_true", dest="nokey", default=False,
                  help="remove all SourceData information [off by default]")

parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default=None,
                  help="outputfile [first TDIFILE-1.xml will be used otherwise]")

parser.add_option("-U", "--upsample",
                  action="store_true", dest="upsample", default=False,
                  help="upsample all time series to the fastest one [default is to downsample to the slowest]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need at least one file to work with.")

if options.nokey and options.keyonly:
    parser.error("Conflicting options --keyOnly and --noKey.")

outputfile = options.outputfile and options.outputfile or args[0]
inputfiles = args[0:]

observables = {}; obslist = []
sources     = []
uniquesecs  = {}; seclist = []

comments = ''

for i,filename in enumerate(inputfiles):
    xml = lisaxml2.lisaXML(filename,'r')
    
    if xml.Comment:                                 # TODO: better provenance tracking
        comments += (comments and '\n' or '')       # add a newline if we have something already
        comments += '(From %s.)' % f
        comments += xml.Comment                     # accumulate comments
    
    for sec in xml:
        if len(sec) > 0:
            if sec.Type == 'TDIData' and not (options.keyonly and i > 0):
                for obs in sec:
                    obsname = obs.TimeSeries.name
                    if obsname not in obslist:      # collect the first instance of every observable set
                        observables[obsname] = obs
                        obslist.append(obsname)
                    else:
                        addobservables(observables[obsname],obs,filename)       # and add subsequent instances
            elif sec.Type == 'SourceData':
                for src in sec:
                    if hasattr(src,'TimeSeries'):
                        del source.TimeSeries
                    sources.append(src)             # accumulate sources (but not their TimeSeries)
            elif sec.Type not in seclist:
                uniquesecs[sec.Type] = sec          # collect the first instance of every other section
                seclist.append(sec.Type)
    
    xml.close()

# open output file
out = lisaxml2.lisaXML(outputfile,'w',author='addXML.py',comments=comments)

# note/TODO: Comment fields within the sections but outside the section children are lost for LISA, Noise, Source, TDI Data
# note/TODO: Could use better handling of the --noKey censored data

# add unique sections
if not options.nokey:
    seclist.reverse()
    for sectype in seclist:
        if sectype in ('LISAData','NoiseData'):     # not ideal, but lisaxml2 always begins with empty
            outsec = getattr(out,sectype)           # LISAData and NoiseData, so we fill them out with content
            outsec += uniquesecs[sectype][:]
        else:
            out.insert(0,uniquesecs[sectype])       # otherwise we prepend the sections in the XSIL file
                                                    # because of the reverse() above, they end up in the order of appearance

# add sources
if not options.nokey:
    for src in sources:
        out.SourceData.append(src)

# add TDI observables
for obsname in obslist:
    out.TDIData.append(observables[obsname])

out.close()

sys.exit(0) 
           
