#!/usr/bin/env python

__version__='$Id: $'

import sys
import glob
import os
import tempfile
import re
import math
from distutils.dep_util import newer

import lisaxml

def filterfile(infilename,outfilename,dictionary):
    outfile = open(outfilename,'w')
    
    for line in open(infilename,'r'):
        for key in dictionary:
            line = re.sub('<' + key + '>',str(dictionary[key]),line)
        
        outfile.write(line)
    
    outfile.close()


def run(command,quiet = False):
    commandline = command % globals()
    
    try:
        if not quiet:
            print "----> %s" % commandline
            assert(os.system(commandline) == 0)
        else:
            assert(os.system(commandline + ' > /dev/null') == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)


def copylisasim(lisasimdir,workdir):
    if not os.path.isdir(lisasimdir):
        print "makeTDIsignal-lisasim.py: cannot find lisasim directory %s!" % lisasimdir
        sys.exit(1)
    
    here = os.getcwd()
    os.chdir(workdir)
    
    execfiles = ['GWconverter','InstrumentNoise','Noise_Maker','Package',
                 'Vertex1Noise','Vertex1Signals','Vertex1Times',
                 'Vertex2Noise','Vertex2Signals','Vertex2Times',
    			 'Vertex3Noise','Vertex3Signals','Vertex3Times']
    
    for f in execfiles:
        run('ln -s %s/%s ./%s' % (lisasimdir,f,f),quiet=True)
    
    run('mkdir Binary',quiet=True)
    run('mkdir Data',quiet=True)
    run('mkdir XML',quiet=True)
    
    binfiles = ( glob.glob('%s/Binary/Vert[1-3]_*.dat' % lisasimdir) +
                 glob.glob('%s/Binary/UnVc[XYZ]*.dat' % lisasimdir) +
                 ['%s/Binary/Setup.dat' % lisasimdir] )
    
    for b in binfiles:
        run('ln -s %s Binary/%s' % (b,os.path.basename(b)),quiet=True)
    
    os.chdir(here)


execdir = os.path.dirname(os.path.abspath(sys.argv[0]))

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] BARYCENTRIC.xml OUTPUT.xml",
                      version="$Id: $")

parser.add_option("-D", "--lisasimDir",
                  type="string", dest="lisasimdir", default=None,
                  help="location of the LISA Simulator executable for the correct signal length [will try to guess]")

parser.add_option("-g", "--gsldir",
                  type="string", dest="gsldir", default=None,
                  help="location of GSL [will try to guess]")

parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560,
                  help="total time for TDI observables (s), will be overridden by LISA Simulator installation if given [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s), will be overridden by LISA Simulator installation if given [default 15]")

parser.add_option("-e", "--extend",
                  action="store_true", dest="extend", default=False,
                  help="try to extend barycentric file before using it [default False]")

parser.add_option("-i", "--immediate",
                  action="store_true", dest="immediate", default=False,
                  help="use makebarycentric to make sources [default False]")

parser.add_option("-v", "--debug",
                  action="store_true", dest="debug", default=False,
                  help="print debugging information [default False]")

(options, args) = parser.parse_args()

# try to find the LISA Simulator...

if options.lisasimdir == None:
    try:
        import lisasimulator
    except:
        parser.error("You must provide the location of the LISA Simulator!")
    
    if options.duration == 31457280 and options.timestep == 15.0:
        lisasimdir = lisasimulator.lisasim1yr
    elif options.duration == 62914560 and options.timestep == 15.0:
        lisasimdir = lisasimulator.lisasim2yr
    else:
        lisasimdir = None
else:
    lisasimdir = os.path.abspath(options.lisasimdir)

if options.gsldir == None:
    try:
        options.gsldir = re.match('(.*)/lib/.*',lisaxml.__file__).group(1)
    except:
        pass

if len(args) != 2:
    parser.error("You must specify an input file and an output file!")

(inputfile,outputfile) = args

if (inputfile[-4:] != '.xml') or (outputfile[-4:] != '.xml'):
    parser.error("Sorry to be pedantic, but the files should end in .xml!")

# make a temporary directory
here = os.getcwd()
workdir = os.path.abspath(tempfile.mkdtemp(dir='.'))

# XML and binary filenames for the input (source or barycentric) file
xmlfile = os.path.abspath(inputfile)
binfile = re.sub('.xml','-0.bin',xmlfile)

# XML filename for output (TDI) file; create empty
tdifile = os.path.abspath(outputfile)
sourcefileobj = lisaxml.lisaXML(tdifile).close()

if options.immediate:   # process sources in source file, one by one
    copylisasim(lisasimdir,workdir) # make virtual LISAsim environment
    
    inputxml = lisaxml.readXML(xmlfile)
    
    for src in inputxml.getLISASources():
        sourcefilename = workdir + '/XML/tmpsource.xml'
        baryfilename   = workdir + '/XML/tmpbary_Barycenter.xml'
        
        sourcefile = lisaxml.lisaXML(sourcefilename)
        sourcefile.SourceData(src)
        sourcefile.close()
        
        if options.debug: print "---> Generating barycentric file for source", src.name 
        run('%s/makebarycentric.py -T %s -d %s %s %s' % (execdir,options.duration,options.timestep,sourcefilename,baryfilename))
        
        os.chdir(workdir)
        
        if options.debug: print "---> Generating TDI file for source", src.name 
        run('./GWconverter tmpbary')
        
        run('./Vertex1Signals')
        run('./Vertex2Signals')
        run('./Vertex3Signals')
        
        run('echo "tmpbary" > sources.txt')
        run('./Package tmpbary sources.txt 0')
        
        if options.debug: print "---> Merging source", src.name, "into accumulator" 
        run('%s/mergeXML.py %s XML/tmpbary.xml' % (execdir,tdifile),quiet=True)
else:
    inputxml = lisaxml.readXML(xmlfile)
    
    srcs = inputxml.getLISASources()
    if len(srcs) > 1:
        print "makeTDIsignal-lisasim.py: WARNING, I will use only the first source in the input file."
    
    src = srcs[0]
    
    if hasattr(src,'RequestTimeOffset') and hasattr(src,'RequestDuration'):
        t0 = src.RequestTimeOffset
        dt = src.TimeSeries.Cadence
        samples = int(src.RequestDuration / dt)
        npow = int(math.log(samples,2))
        T = samples * dt
        
        if 2**npow != samples:
            print "makeTDIsignal-lisasim.py: I can only call lisasim with a power-of-two number of samples."
            
        lisasimtar = execdir + '/../../Packages/Simulator-2.1.1c.tar.gz'
        
        assert(0 == os.system('tar zxf %s -C %s' % (lisasimtar,workdir)))
        
        # annoying, the tar unpacks in its own directory; copy files over
        assert(0 == os.system('cp -R %s/Simulator-2.1.1c/* %s' % (workdir,workdir)))
        assert(0 == os.system('rm -rf %s/Simulator-2.1.1c' % workdir))
        
        filterfile(execdir + '/../../Packages/LISASimulator/Compile',workdir + '/Compile',{})
        filterfile(execdir + '/../../Packages/LISASimulator/LISAconstants-generic.h',workdir + '/LISAconstants.h',
                   {'npow': npow,'NFFT': samples,'T0': t0,'T': T,'dt': dt})
        
        os.chdir(workdir)
                
        if options.gsldir != None:
            assert(0 == os.system('./Compile --gsl=' + options.gsldir))
        else:
            assert(0 == os.system('./Compile'))
        
        assert(0 == os.system('./Setup'))
        os.chdir(here)
    else:
        copylisasim(lisasimdir,workdir) # make virtual LISAsim environment
    
    if '-barycentric.xml' in xmlfile:
        xmlname = re.sub('\-barycentric.xml','',os.path.basename(xmlfile))
    else:
        xmlname = re.sub('.xml','',os.path.basename(xmlfile))
    
    xmldest = workdir + '/XML/' + xmlname + '_Barycenter.xml'
    bindest = workdir + '/XML/' + os.path.basename(binfile)
    
    run('cp %s %s' % (xmlfile,xmldest),quiet=True)
    run('cp %s %s' % (binfile,bindest),quiet=True)
    
    os.chdir(workdir)
    
    # run LISA simulator...
    
    run('./GWconverter %s' % xmlname)
    
    run('./Vertex1Signals')
    run('./Vertex2Signals')
    run('./Vertex3Signals')
    
    run('echo "%s" > sources.txt' % xmlname)
    run('./Package %s sources.txt 0' % xmlname)
    
    outxml = 'XML/' + xmlname + '.xml'
    outbin = 'XML/' + xmlname + '.bin'
    
    # then add the TDI data from the LISA Simulator output
    run('%s/mergeXML.py %s %s %s' % (execdir,tdifile,execdir + '/../Template/StandardLISA.xml',outxml),quiet=True)

# then add the modified Source data included in the Barycentric file
run('%s/mergeXML.py -k %s %s' % (execdir,tdifile,xmlfile),quiet=True)

os.chdir(here)

# ...remove the tmp dir
run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
