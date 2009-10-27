#!/usr/bin/env python

__version__ = '$Id$'

# some definitions...

import sys
import os
import shutil
import glob
import re
import time
import subprocess
import tempfile
from distutils.dep_util import newer, newer_group

import lisaxml
import numpy

def makefromtemplate(output,template,**kwargs):
    fi = open(template,'r')
    fo = open(output,'w')
        
    for line in fi:
        repline = line
        
        for kw in kwargs:
            repline = re.sub('{' + kw + '}',str(kwargs[kw]),repline)
        
        print >> fo, repline,

def timestring(lapse):
    hrs  = int(lapse/3600)
    mins = int((lapse - hrs*3600)/60)
    secs = int((lapse - hrs*3600 - mins*60))
    
    return "%sh%sm%ss" % (hrs,mins,secs)

def run(command,quiet = False):
    # if "run" is moved to a module, can use sys.modules['__main__'].__dict__ instead of globals()
    commandline = command % globals()
    
    outfilename = tempfile.mktemp(suffix='.out',prefix='',dir=workdir+'/Log')
    errfilename = tempfile.mktemp(suffix='.err',prefix='',dir=workdir+'/Log')
    
    if not quiet:
        print "--> %s (out,err: %s,%s)" % (commandline,os.path.basename(outfilename),os.path.basename(errfilename))
    
    outfile = open(outfilename,'w')
    errfile = open(errfilename,'w')
    
    print >> outfile, "Command: " + commandline
    print >> outfile, "Started on " + time.asctime(time.localtime()) + "."
    print >> outfile, "---"
    outfile.flush()
    
    exe = subprocess.Popen(commandline,shell=True,stdin=None,stdout=outfile,stderr=errfile)
    ret = exe.wait()
    out = exe.communicate()
    
    outfile.flush()
    print >> outfile, "---"
    print >> outfile, "Finished on " + time.asctime(time.localtime()) + "."
    
    outfile.close()
    errfile.close()
    
    if ret != 0:
        print '!!!! Script %s failed at command "%s" with errorcode %s.' % (sys.argv[0],commandline,ret)
        print 'Out, err files:', outfilename, errfilename
        
        print '---- Standard output: ----'; print open(outfilename,'r').read()
        print '---- Standard error:  ----'; print open(errfilename,'r').read()
        
        sys.exit(ret)


# TO DO: "quiet" does not seem to be used anymore
class parallelrun(object):
    def __init__(self,np=1):
        self.nproc = np
        
        self.queue = []
        self.slots = [None] * self.nproc
    
    def submit(self,command,quiet=True):
        self.queue.append((command % globals(),quiet))
        pass
    
    def run(self):
        jobs = len(self.queue)
        fail = 0
        
        while True:
            idle = 0
            
            for cpu in range(self.nproc):
                # first clear up ended processes if any
                if self.slots[cpu]:
                    proc, quiet, command, outfile, errfile = self.slots[cpu]
                    ret = proc.poll()
                    
                    if ret != None:
                        out = proc.communicate()
                        
                        outfile.flush()
                        print >> outfile, "---"
                        print >> outfile, "Finished on " + time.asctime(time.localtime()) + "."
                        
                        outfilename = outfile.name
                        errfilename = errfile.name
                        
                        outfile.close()
                        errfile.close()
                        
                        if ret != 0:
                            print '!!!! Script %s failed at command "%s" with errorcode %s.' % (sys.argv[0],command,ret)
                            print 'Out, err files:', outfilename, errfilename
                            
                            print '---- Standard output: ----'; print open(outfilename,'r').read()
                            print '---- Standard error:  ----'; print open(errfilename,'r').read()
                            
                            fail += 1
                        
                        self.slots[cpu] = None
                
                # now run another process if we have a free slot        
                if not self.slots[cpu]:
                    if self.queue:
                        command, quiet = self.queue[0]
                        
                        outfilename = tempfile.mktemp(suffix='.out',prefix='',dir=workdir+'/Log')
                        errfilename = tempfile.mktemp(suffix='.err',prefix='',dir=workdir+'/Log')
                        
                        outfile = open(outfilename,'w')
                        errfile = open(errfilename,'w')
                        
                        print >> outfile, "Command: " + command
                        print >> outfile, "Started on " + time.asctime(time.localtime()) + "."
                        print >> outfile, "---"
                        outfile.flush()
                                            
                        try:
                            self.slots[cpu] = (subprocess.Popen(command,stdin=None,stdout=outfile,stderr=errfile,shell=True),quiet,command,outfile,errfile)
                        except:
                            print '--:> Script %s failed spawning parallel command "%s".' % (sys.argv[0],command)
                            outfile.close()
                            errfile.close()
                            fail += 1
                        else:
                            print "--:> CPU [%d]: %s (out,err: %s,%s)" % (cpu,command,os.path.basename(outfile.name),os.path.basename(errfile.name))
                        
                        del self.queue[0]
                    else:
                        idle += 1
            
            if idle == self.nproc:
                if fail == 0:
                    print "--:> Parallel run completed (%d jobs)." % jobs
                    return
                else:
                    print "--:> Parallel run reported %d/%d failures!" % (fail,jobs)
                    sys.exit(1)
            else:
                time.sleep(1.0)
    


step0time = time.time()

# ---------------------------------------
# STEP 0: parse parameters, set constants
# ---------------------------------------

oneyear = 31457280

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME...",
                      version="$Id$")

parser.add_option("-t", "--training",
                  action="store_true", dest="istraining", default=False,
                  help="include source information in output file [off by default]")

# note that the default value is handled below
parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=None,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for TDI observable sampling (s) [default 15]")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-n", "--seedNoise",
                  type="int", dest="seednoise", default=None,
                  help="noise-specific seed (int) [will use global seed (-s/--seed) if not given]")

parser.add_option("-N","--noNoise",
                  action="store_true", dest="nonoise", default=False,
                  help="do not include noise [default False]")

parser.add_option("-m", "--make",
                  action="store_true", dest="makemode", default=False,
                  help="run in make mode (use already-generated source key files and intermediate products; needs same seeds)")

parser.add_option("-S", "--synthlisa",
                  action="store_true", dest="synthlisaonly", default=False,
                  help="run only Synthetic LISA")

# note the letter mismatch with the makeTDIsignal/noise-synthlisa2.py option
parser.add_option("-I", "--LISA",
                  type="string", dest="LISAmodel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")

parser.add_option("-L", "--lisasim",
                  action="store_true", dest="lisasimonly", default=False,
                  help="run only the LISA Simulator")

parser.add_option("-C", "--lisacode",
                  action="store_true", dest="lisacodeonly", default=False,
                  help="run only lisacode, if challenge supports it [off by default]")

parser.add_option("-F", "--nohighfreq",
                  action="store_true", dest="nohighfreq", default=False,
                  help="do not generate high-frequency datasets [generate by default]")

parser.add_option("-f", "--keyFile",
                  type="string", dest="sourcekeyfile", default=None,
                  help="get source descriptions from key file, not CHALLENGE.py script")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z [synthlisa and lisacode only]")

parser.add_option("-R", "--randomizeNoise",
                  type="float", dest="randomizeNoise", default=0.0,
                  help="randomize level of noises [e.g., 0.2 for 20% randomization; defaults to 0; synthlisa and lisacode only]")

parser.add_option("-l", "--laserNoise",
                  type="string", dest="laserNoise", default='None',
                  help="laser noise level: None, Standard, <numerical value> [e.g., 0.2 for 20% of pm noise at 1 mHz; defaults to None; synthlisa and lisacode only]")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined SNR = sqrt(2)*max{SNR_x, SNR_y, SNR_z} to rescale signals [off by default]")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

parser.add_option("-o", "--outputfile",
                  type="string", dest="outputfile", default=None,
                  help="basename for the datasets [defaults to the CHALLENGENAME]")


(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name!")

challengename = args[0]

if options.outputfile:
    outputfile = options.outputfile
else:
    outputfile = challengename

if options.seed == None:
    parser.error("You must specify the seed!")

# this sets the default dataset duration if not specified with special cases:
# - one year for challenge 1 except 1.3.X EMRIs (two years)
# - 24 days for challenges 3.4 and 3.5

if options.duration == None:
    if ('challenge1' in challengename) and (not 'challenge1.3' in challengename):
        print("--> It looks like you're generating a challenge-1 dataset, so I will set the duration to 2^21 * 15 (you can override this with -T).")
        options.duration = oneyear
    elif ('challenge3.4' in challengename) or ('challenge3.5' in challengename):
        options.duration = 2**21
        options.timestep = 1.0
    else:
        options.duration = 2.0*oneyear

# decide which simulators to use

dosynthlisa = not (                         options.lisasimonly or options.lisacodeonly) or options.synthlisaonly
dolisasim   = not (options.synthlisaonly                        or options.lisacodeonly) or options.lisasimonly
dolisacode  = not (options.synthlisaonly or options.lisasimonly                        ) or options.lisacodeonly

if dolisasim:
    # see if the duration is allowed by lisasim
    import lisasimulator
    
    if options.duration == 31457280 and options.timestep == 15.0:
        pass
    elif options.duration == 62914560 and (options.timestep == 15.0 or options.timestep == 1.875):
        pass
    else:
        parser.error("I can only run the LISA Simulator for one year at 15 s, or two years at 15 or 1.875 s!")
    
    if options.randomizeNoise > 0.0:
        parser.error("Option --randomizeNoise is not supported with the LISA Simulator.")
    if options.laserNoise != 'None':
        parser.error("Option --laserNoise is not supported with the LISA Simulator.")
    if options.rawMeasurements == True:
        parser.error("Option --rawMeasurement is not supported with The LISA Simulator.")

seed = options.seed

if options.seednoise == None:
    seednoise = seed
else:
    seednoise = options.seednoise

# setup for parallel run

if options.nproc > 1:
    pengine = parallelrun(options.nproc)
    prun  = pengine.submit
    pwait = pengine.run
else:
    prun = run
    pwait = lambda : None

timestep = options.timestep
duration = options.duration
makemode = options.makemode
istraining = options.istraining
donoise = not options.nonoise

sourcekeyfile = options.sourcekeyfile

# -----------------------------------------------------------------------------------
# STEP 0: see where we're running from, and create directories if they're not present
# -----------------------------------------------------------------------------------

execdir = os.path.dirname(os.path.abspath(sys.argv[0]))
workdir = os.path.abspath(os.path.curdir)

for folder in ('Barycentric','Dataset','Galaxy','Source','TDI','Template','Immediate','LISACode','Log'):
    if not os.path.exists(folder):
        os.mkdir(folder)

for doc in ('Template/lisa-xml.css','Template/lisa-xml.xsl','Template/StandardLISA.xml','Template/LISACode.xml'):
    if not os.path.exists(doc):
        shutil.copyfile(execdir + '/../' + doc,doc)

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

step1time = time.time()

# first empty the Source and Galaxy directories
# makemode won't regenerate sources, so at least step 1 must be completed
# before running in make mode

if (not makemode) and (not sourcekeyfile):
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml Galaxy/*.dat')
    run('rm -f Immediate/*.xml')
    run('rm -f LISACode/*.xml LISACode/*.bin')
    run('rm -f Log/*.out Log/*.err')

    # to run CHALLENGE, a file source-parameter generation script CHALLENGE.py must sit in bin/
    # it must take a single argument (the seed) and put its results in the Source subdirectory
    # if makesource-Galaxy.py is called, it must be with the UNALTERED seed, which is used
    # later to call makeTDIsignals-Galaxy.py

    # look for the script relative to challenge.py

    sourcescript = execdir + '/' + challengename + '.py'
    if 'challenge1C' in challengename and not os.path.isfile(sourcescript):
        sourcescript = re.sub('challenge1C','challenge1B',sourcescript)
        print "--> I'm using script %s for %s" % (sourcescript,challengename)

    if not os.path.isfile(sourcescript):
        parser.error("I need the challenge script %s!" % sourcescript)
    else:
        run('python %s %s %s %s' % (sourcescript,seed,istraining,options.nproc))
elif (not makemode) and sourcekeyfile:
    # convoluted logic, to make sure that the keyfile given is not in Source/ and therefore deleted
    # TO DO: improve

    run('mv %s %s-backup' % (sourcekeyfile,sourcekeyfile))
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml Galaxy/*.dat')
    run('mv %s-backup %s' % (sourcekeyfile,sourcekeyfile))

    run('cp %s Source/%s' % (sourcekeyfile,os.path.basename(sourcekeyfile)))

step2time = time.time()

# check if any of the source files requests an SN; in this case we'll need to run synthlisa!

if os.system('grep -qs RequestSN Source/*.xml') == 0 and dosynthlisa == False:
    parser.error("Ah, at least one of your Source files has a RequestSN field; then I MUST run synthlisa to adjust SNRs.")

# ---------------------------------------
# STEP 2: create barycentric strain files
# ---------------------------------------

# first empty the Barycentric directory
if (not makemode):
    run('rm -f Barycentric/*.xml Barycentric/*.bin')

# then run makebarycentric over all the files in the Source directory
for xmlfile in glob.glob('Source/*.xml'):
    baryfile = 'Barycentric/' + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    if (not makemode) or newer(xmlfile,baryfile):
        # TO DO - more kludging... would be better to do RequestTimeStep 
        if 'challenge4' in challengename and 'Burst' in baryfile:
            prun('%(execdir)s/makebarycentric.py --duration=%(duration)s --timeStep=1.875 %(xmlfile)s %(baryfile)s')
        else:
            prun('%(execdir)s/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

pwait()

step3time = time.time()

# --------------------------
# STEP 3: run Synthetic LISA
# --------------------------

# first empty the TDI directory
if (not makemode):
    run('rm -f TDI/*.xml TDI/*.bin TDI/Binary TDI/*.txt')

# make noise options (will be used also by LISACode later)

noiseoptions = ''
if options.randomizeNoise > 0.0:
    noiseoptions += ('--randomizeNoise=%s ' % options.randomizeNoise)
if options.laserNoise != 'None':
    noiseoptions += ('--laserNoise=%s ' % options.laserNoise)
if options.rawMeasurements == True:
    noiseoptions += '--rawMeasurements '
if options.LISAmodel != 'Eccentric':
    noiseoptions += '--LISA=%s ' % options.LISAmodel

runoptions = ''    
if options.rawMeasurements == True:
    runoptions += '--rawMeasurements '
if options.combinedSNR == True:
    runoptions += '--combinedSNR '
if options.LISAmodel != 'Eccentric':
    runoptions += '--LISA=%s ' % options.LISAmodel
if not glob.glob('Galaxy/*.xml'):
    # if we are not generating a Galaxy, don't include its confusion noise in the evaluation of SNR
    runoptions += '--noiseOnly '

if dosynthlisa:
    # then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
    # the results are files of TDI time series that include the source info
    # these calls have also the result of rescaling the barycentric files to satisfy any RequestSN that
    # they may carry
        
    # for safety: use new makeTDIsignal-synthlisa.py only if we have immediate sources or nonstandard
    if glob.glob('Immediate/*.xml') or (options.rawMeasurements == True) or (options.LISAmodel != 'Eccentric') or ('challenge4' in challengename):
        runfile = 'makeTDIsignal-synthlisa2.py'
    else:
        runfile = 'makeTDIsignal-synthlisa.py'
    
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml') + glob.glob('Immediate/*.xml'):
        if 'barycentric.xml' in xmlfile:
            tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
        else:
            tdifile = 'TDI/' + re.sub('\.xml$','-tdi-frequency.xml',os.path.basename(xmlfile))

        if (not makemode) or newer(xmlfile,tdifile):
            # TO DO - more kludging... would be better to do RequestTimeStep
            if 'challenge4' in challengename and 'Burst' in xmlfile:
                prun('%(execdir)s/makeTDIsignal-synthlisa2.py %(runoptions)s --duration=%(duration)s --timeStep=1.875 %(xmlfile)s %(tdifile)s')
            else:
                prun('%(execdir)s/%(runfile)s %(runoptions)s --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

    if noiseoptions or (timestep != 15.0):
        runfile = 'makeTDInoise-synthlisa2.py'
    else:
        runfile = 'makeTDInoise-synthlisa.py'

    # now run noise generation... note that we're not checking whether the seed is the correct one
    noisefile = 'TDI/tdi-frequency-noise.xml'
        
    if donoise and ((not makemode) or (not os.path.isfile(noisefile))):        
        if 'challenge4' in challengename:
            prun('%(execdir)s/makeTDInoise-synthlisa2.py --seed=%(seednoise)s --duration=%(duration)s --timeStep=1.875 %(noiseoptions)s %(noisefile)s')        
        else:
            prun('%(execdir)s/%(runfile)s --seed=%(seednoise)s --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s %(noisefile)s')

    pwait()

step4time = time.time()

# --------------------------
# STEP 4: run LISA Simulator
# --------------------------

if dolisasim:
    # TO DO: deal with stochastic source files here, by making temporary barycentric and then deleting?
    #        a separate command-line utility may be needed
    for xmlfile in glob.glob('Immediate/*.xml'):
        print "--> !!! Ignoring immediate synthlisa file %s" % xmlfile

    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        tdifile = 'TDI/' + re.sub('\-barycentric.xml','-tdi-strain.xml',os.path.basename(xmlfile))

        if (not makemode) or newer(xmlfile,tdifile):
            # remember that we're not doing any SNR adjusting here...
            
            if 'challenge4' in challengename and 'Burst' in xmlfile:
                prun('%(execdir)s/makeTDIsignal-lisasim.py --duration=%(duration)s --timeStep=1.875 --extend %(xmlfile)s %(tdifile)s')
            else:
                prun('%(execdir)s/makeTDIsignal-lisasim.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')
    
    slnoisefile = 'TDI/tdi-strain-noise.xml'

    if donoise and ((not makemode) or (not os.path.isfile(slnoisefile))):
        # TO DO: adjust the timestep for MLDC4
        prun('%(execdir)s/makeTDInoise-lisasim.py --duration=%(duration)s --timeStep=%(timestep)s --seedNoise=%(seednoise)s %(slnoisefile)s')
    
    pwait()

step4btime = time.time()

# ---------------------
# STEP 4b: run LISAcode
# ---------------------
dolcsigseparate = True
if dolisacode:
    # Use new lisacode pipeline for challenge4
    if 'challenge4' in challengename:

        os.chdir('LISACode')

        # Make TDI for sources

        dosmallstep = False
        if dolcsigseparate:
            # Compute signals separately
            runfile = 'makeTDIsignal-lisacode.py'
            for xmlfile in glob.glob('../Barycentric/*-barycentric.xml') + glob.glob('*Background.xml'):
                if 'barycentric.xml' in xmlfile:
                    tdifile = re.sub('barycentric\.xml$','tdi-lisacode.xml',os.path.basename(xmlfile))
                else:
                    tdifile = re.sub('\.xml$','-tdi-lisacode.xml',os.path.basename(xmlfile))
                if (not makemode) or newer(xmlfile,tdifile):
                    if 'challenge4' in challengename and 'Burst' in xmlfile:
                        dosmallstep = True
                        prun('%(execdir)s/%(runfile)s %(runoptions)s --duration=%(duration)s --timeStep=1.875 %(xmlfile)s %(tdifile)s')
                    else:
                        prun('%(execdir)s/%(runfile)s %(runoptions)s --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')            
        else:
            # Compute signal all at once
            runfile = 'makeTDI-lisacode.py'
            lcsigfileinput = ''
            lcsigfile = 'Signals-tdi-lisacode.xml'
            dosignal = False
            for xmlfile in glob.glob('../Barycentric/*-barycentric.xml'):
                dosignal = True
                lcsigfileinput = lcsigfileinput + ' ' + xmlfile
                if 'challenge4' in challengename and ('Burst' in xmlfile):
                    dosmallstep = True
            if dosignal:
                if dosmallstep:
                    prun('%(execdir)s/%(runfile)s -w -N %(runoptions)s --duration=%(duration)s --timeStep=1.875 -e %(lcsigfile)s %(lcsigfileinput)s')
                else:
                    prun('%(execdir)s/%(runfile)s -w -N %(runoptions)s --duration=%(duration)s --timeStep=15.0 -e %(lcsigfile)s %(lcsigfileinput)s')
            # Compute stochastic-background separatly
            dosignal = False
            lcsigfileinput = ''
            lcsigfile = 'SignalsBack-tdi-lisacode.xml'
            for xmlfile in glob.glob('*Background.xml'):
                dosignal = True
                lcsigfileinput = lcsigfileinput + ' ' + xmlfile
            if dosignal:
                prun('%(execdir)s/%(runfile)s -w -N %(runoptions)s --duration=%(duration)s --timeStep=15.0 -e %(lcsigfile)s %(lcsigfileinput)s')
                

        # Make TDI for noises

        runfile = 'makeTDInoise-lisacode.py'
        lcnoisefile = 'tdi-lisacode-noise.xml'
        if dosmallstep:
            prun('%(execdir)s/%(runfile)s --seed=%(seednoise)s --duration=%(duration)s --timeStep=1.875 %(noiseoptions)s %(lcnoisefile)s')
        else:
            prun('%(execdir)s/%(runfile)s --seed=%(seednoise)s --duration=%(duration)s --timeStep=15.0 %(noiseoptions)s %(lcnoisefile)s')
        
        pwait()
        
        os.chdir('..')
        run("mv LISACode/*lisacode.xml TDI/.")
        run("mv LISACode/*lisacode-noise.xml TDI/.")
        run("mv LISACode/*TDI.bin TDI/.")
 
    else:
        # Use of LISACode before challenge4 : challenge3.5
        if dolisacode and glob.glob('LISACode/source-*.xml'):

            # make the standard lisacode instruction set
            cname = outputfile

            if istraining:  cname += '-training'
            if not donoise: cname += '-nonoise'

            # (use same seed as synthlisa if we're training)
            lcseednoise = istraining and seednoise or (seednoise + 1)

            makefromtemplate('LISACode/%s-lisacode-input.xml' % cname,'%s/../Template/LISACode.xml' % execdir,
                             challengename=cname,
                             cadence=timestep,
                             duration=duration,
                             randomseed=lcseednoise,
                             orbit=options.LISAmodel)

            if donoise:
                # make lisacode noise (note that the random seed is really set above in the standard instruction set)
                run('%(execdir)s/makeTDInoise-synthlisa2.py --keyOnly --keyOmitsLISA --seed=%(lcseednoise)s --duration=%(duration)s --timeStep=%(timestep)s %(noiseoptions)s LISACode/noise.xml')

                # merge with source data into a single lisacode input file
                run('%(execdir)s/mergeXML.py LISACode/%(cname)s-lisacode-input.xml LISACode/noise.xml LISACode/source-*.xml')
                run('rm LISACode/noise.xml')
            else:
                run('%(execdir)s/mergeXML.py LISACode/%(cname)s-lisacode-input.xml LISACode/source-*.xml')

            run('rm LISACode/source-*.xml')

            os.chdir('LISACode')

            import lisacode    
            run('%s %s-lisacode-input.xml' % (lisacode.lisacode,cname))

            os.chdir('..')


step5time = time.time()


# -----------------------
# STEP 5: run Fast Galaxy
# -----------------------

# should avoid running both SL and LS if only one is requested...

if glob.glob('Galaxy/*.xml'):
    for xmlfile in glob.glob('Galaxy/*.xml'):
        sltdifile = 'TDI/' + re.sub('.xml','-tdi-frequency.xml',os.path.basename(xmlfile))
        lstdifile = 'TDI/' + re.sub('.xml','-tdi-strain.xml',os.path.basename(xmlfile))

        if (not makemode) or (newer(xmlfile,sltdifile) or newer(xmlfile,lstdifile)):
            prun('%s/makeTDIsignals-Galaxy3.py %s %s %s' % (execdir,xmlfile,sltdifile,lstdifile))
    
    pwait()

    for xmlfile in glob.glob('Galaxy/*.xml'):
        sltdifile = 'TDI/' + re.sub('.xml','-tdi-frequency.xml',os.path.basename(xmlfile))
        lctdifile = 'TDI/' + re.sub('.xml','-tdi-lisacode.xml',os.path.basename(xmlfile))
        if (not makemode) or (newer(xmlfile,sltdifile) or newer(xmlfile,lstdifile)):
            lisaxml.lisaXML(lctdifile,
                        author="MLDC Task Force",
                        comments='Galaxy dataset for %s (lisacode version)' % (challengename)).close()
            run('%(execdir)s/mergeXML.py  %(lctdifile)s %(sltdifile)s')
            

# in challenge 3.2, the Galaxy is noise, so merge it with the instrument noise
# this is a hack that should be later changed with something more systematic

if 'challenge3.2' in challengename:
    if dosynthlisa:
        run('%s/mergeXML.py %s %s' % (execdir,noisefile  ,'TDI/TheGalaxy-tdi-frequency.xml'))
        run('rm TDI/TheGalaxy-tdi-frequency.xml TDI/TheGalaxy-tdi-frequency-0.bin')

    if dolisasim:
        run('%s/mergeXML.py %s %s' % (execdir,slnoisefile,'TDI/TheGalaxy-tdi-strain.xml'))
        run('rm TDI/TheGalaxy-tdi-strain.xml TDI/TheGalaxy-tdi-strain-0.bin')

step6time = time.time()

# -------------------------
# STEP 6: assemble datasets
# -------------------------

# run('rm -f Dataset/*.xml Dataset/*.bin Dataset/*.txt')

# improve dataset metadata here

# do synthlisa first

run('cp Template/lisa-xml.xsl Dataset/.')
run('cp Template/lisa-xml.css Dataset/.')

import lisatoolsrevision

# global variables used: outputfile, challengename, istraining, execdir
def makedataset(simulator,dokey=True,hifreq=False):
    theoutputfile = 'Dataset/' + outputfile
    comment = '%s (%s)' % (challengename,simulator)
    
    if hifreq:
        theoutputfile += '-hifreq'
        comment += ', high-frequency version'
    
    if istraining:
        theoutputfile += '-training'   # include seeds only if we're training
        comment += ', source seed = %s, noise seed = %s' % (seed,seednoise)
    
    comment += ', LISAtools SVN revision %s' % lisatoolsrevision.lisatoolsrevision
    
    merger = execdir + '/mergeXML.py'
    if not istraining:
        merger += ' --noKey'    # include keys only if we're training
    if hifreq:
        # --conserveMemory is still experimental, disable it by default
        merger += ' --upsample' # instruct mergeXML.py to upsample for hi-freq dataset
        if 'challenge4.0' in challengename:
            merger += ' --cadence=1.875'
    
    thenoisefile = 'TDI/tdi-' + simulator + '-noise.xml'
    tdifiles = 'TDI/*-tdi-' + simulator + '.xml'
    
    # make the noiseless dataset
    noiselessdataset = theoutputfile + '-nonoise-' + simulator + '.xml'
    lisaxml.lisaXML(noiselessdataset,author="MLDC Task Force",comments=comment).close()
    if glob.glob(tdifiles):
        run('%s --tdiName=%s %s %s' % (merger,challengename,noiselessdataset,tdifiles))
    
    # make the noisy dataset
    if donoise:
        noisydataset = theoutputfile + '-' + simulator + '.xml'
        lisaxml.lisaXML(noisydataset,author="MLDC Task Force",comments=comment).close()
        run('%s --tdiName=%s %s %s %s' % (merger,challengename,noisydataset,noiselessdataset,thenoisefile))
    
    # make the key
    if dokey:
        keyfile = theoutputfile + '-key-' + simulator + '.xml'
        lisaxml.lisaXML(keyfile,author="MLDC Task Force",comments=comment).close()
        # is there something strange here with the 3.2 key, which used to be done in one step, TDI + noise?
        # yes, I think the point was that the source info would be only picked up from the first file?
        run('%s/mergeXML.py --keyOnly %s %s' % (execdir,keyfile,thenoisefile))
        if glob.glob(tdifiles):
            run('%s/mergeXML.py --keyOnly %s %s' % (execdir,keyfile,tdifiles))
        
    # tar up the noiseless dataset (or destroy if not training)
    if istraining:
        run('tar zcf %s %s %s Dataset/lisa-xml.*' % (re.sub('\.xml','.tar.gz',noiselessdataset),
                                                     noiselessdataset,
                                                     re.sub('\.xml','-[0-9].bin',noiselessdataset)))
    else:
        run('%s/rmXML.py %s' % (execdir,noiselessdataset))
    
    # tar up the noisy dataset
    if donoise:
        run('tar zcf %s %s %s Dataset/lisa-xml.*' % (re.sub('\.xml','.tar.gz',noisydataset),
                                                     noisydataset,
                                                     re.sub('\.xml','-[0-9].bin',noisydataset)))

if dosynthlisa:
    makedataset('frequency')
    if 'challenge4' in challengename and not options.nohighfreq:
        makedataset('frequency',dokey=False,hifreq=True)

if dolisasim:
    makedataset('strain')
    if 'challenge4' in challengename and not options.nohighfreq:
        makedataset('strain',dokey=False,hifreq=True)

if dolisacode:
    if 'challenge3' in challengename:
        # would it be possible to refactor this special case within makedataset?
        for xmlfile in glob.glob('LISACode/*-lisacode.xml'):
            basefile = re.sub('LISACode/','',re.sub('\.xml','',xmlfile))
            
            withnoisefile = 'Dataset/' + basefile + '.xml'
            inputfile = 'LISACode/' + basefile + '-input.xml'
            
            comment = 'Full dataset for %s (lisacode)' % challengename
            if istraining:
                comment += ', source seed = %s, noise seed = %s' % (seed,seednoise)
            
            # prepare empty file
            lisaxml.lisaXML(withnoisefile,author="MLDC Task Force",comments=comment).close()
            
            # merge dataset with key (if training), otherwise copy dataset, renaming binaries
            if istraining:
                run('%(execdir)s/mergeXML.py %(withnoisefile)s %(xmlfile)s %(inputfile)s')
            else:
                run('%(execdir)s/mergeXML.py %(withnoisefile)s %(xmlfile)s')
                
            # also, get key file
            run('cp %s %s' % (inputfile,'Dataset/' + basefile + '-key-lisacode.xml'))
            
            # make tar
            os.chdir('Dataset')
            
            withnoisefile = os.path.basename(withnoisefile)
            withnoisetar = re.sub('.xml','.tar.gz',withnoisefile)
            
            run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-[0-9].bin',withnoisefile)))
            
            os.chdir('..')
    else:
        makedataset('lisacode')
        if 'challenge4' in challengename and not options.nohighfreq:
            makedataset('lisacode',dokey=False,hifreq=True)

# make sure the keys point to real Table txt files, not symlinks
# build symlinks from training dataset tables to their key

os.chdir('Dataset')

for txtfile in glob.glob('*.txt'):
    if os.path.islink(txtfile):
        if 'key' in txtfile:
            run('cp %s %s-tmp' % (txtfile,txtfile))     # this has the effect of converting a link to a real file...
            run('mv %s-tmp %s' % (txtfile,txtfile))
        else:
            keyfile = re.sub('training','training-key',txtfile)
            keyfile = re.sub('nonoise-','',            keyfile)
            keyfile = re.sub('hifreq-', '',            keyfile)
            
            run('rm %s' % txtfile)
            run('ln -s %s %s' % (keyfile,txtfile))

os.chdir('..')

# run('rm Dataset/lisa-xml.xsl')
# run('rm Dataset/lisa-xml.css')

step7time = time.time()

# ------------------------
# STEP 7: package datasets
# ------------------------

endtime = time.time()

print
print "--> Completed generating %s" % challengename 
print "    Total time         : %s" % timestring(endtime    - step0time)
print "    Sources time       : %s" % timestring(step3time  - step1time)
print "    Synthetic LISA time: %s" % timestring(step4time  - step3time)
print "    LISA Simulator time: %s" % timestring(step4btime - step4time)
print "    LISACode time      : %s" % timestring(step5time  - step4btime)
print "    Fast Galaxy time   : %s" % timestring(step6time  - step5time)

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
