#!/usr/bin/env python

__version__ = '$Id: challenge2.py 415 2007-06-14 22:47:09Z vallisneri $'

# some definitions...

import sys
import os
import glob
import re
import time
from distutils.dep_util import newer, newer_group

import lisaxml
import numpy

def timestring(lapse):
    hrs  = int(lapse/3600)
    mins = int((lapse - hrs*3600)/60)
    secs = int((lapse - hrs*3600 - mins*60))
    
    return "%sh%sm%ss" % (hrs,mins,secs)

def run(command,quiet = False):
    commandline = command % globals()
    
    try:
        if not quiet:
            print "--> %s" % commandline
            assert(os.system(commandline) == 0)
        else:
            assert(os.system(commandline + ' > /dev/null') == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

try:
    import subprocess
    
    class parallelrun(object):
        def __init__(self,np=1):
            self.nproc = np
            
            self.queue = []
            self.slots = [None] * 4
        
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
                        proc, quiet, command = self.slots[cpu]
                        ret = proc.poll()
                        
                        if ret != None:
                            if ret == 0:
                                if not quiet:
                                    print "--:> CPU [%d] finished." % cpu
                            else:
                                print 'Script %s failed on return from parallel command "%s".' % (sys.argv[0],command)
                                fail += 1
                            
                            self.slots[cpu] = None
                    
                    # now run another process if we have a free slot        
                    if not self.slots[cpu]:
                        if self.queue:
                            command, quiet = self.queue[0]
                            
                            try:
                                # stdout=subprocess.PIPE
                                if quiet:
                                    self.slots[cpu] = (subprocess.Popen(command,stdin=None,stdout=open('/dev/null','w'),stderr=subprocess.STDOUT,shell=True),quiet,command)    
                                else:
                                    self.slots[cpu] = (subprocess.Popen(command,stdin=None,stdout=None,stderr=subprocess.STDOUT,shell=True),quiet,command)
                            except:
                                print 'Script %s failed running parallel command "%s".' % (sys.argv[0],command)
                                fail += 1
                            else:
                                print "--:> CPU [%d]: %s" % (cpu,command)
                            
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
        
    
    candopar = True
except:
    candopar = False

step0time = time.time()

# ---------------------------------------
# STEP 0: parse parameters, set constants
# ---------------------------------------

oneyear = 31457280

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME...",
                      version="$Id: challenge2.py 415 2007-06-14 22:47:09Z vallisneri $")

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

parser.add_option("-L", "--lisasim",
                  action="store_true", dest="lisasimonly", default=False,
                  help="run only the LISA Simulator")

parser.add_option("-f", "--keyFile",
                  type="string", dest="sourcekeyfile", default=None,
                  help="get source descriptions from key file, not CHALLENGE.py script")

parser.add_option("-P", "--nProc",
                  type="int", dest="nproc", default=1,
                  help="run in parallel on nproc CPUs [default 1]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name!")

challengename = args[0]

if options.seed == None:
    parser.error("You must specify the seed!")

# this sets the default dataset duration if not specified, with a special case for apparent challenge-1 datasets
# (but not for 1.3.X EMRIs, which are two years)
if options.duration == None:
    if 'challenge1' in challengename and (not 'challenge1.3' in challengename):
        print("--> It looks like you're generating a challenge-1 dataset, so I will set the duration to 2^21 * 15 (you can override this with -T).")
        options.duration = oneyear
    else:
        options.duration = 2.0*oneyear

if options.synthlisaonly:
    lisasimdir = None
else:
    import lisasimulator

    if options.duration == 31457280:
        lisasimdir = lisasimulator.lisasim1yr
    elif options.duration == 62914560:
        lisasimdir = lisasimulator.lisasim2yr
    else:
        parser.error("I can only run the LISA Simulator for one or two years (2^21 or 2^22 s)!")

if options.lisasimonly:
    dosynthlisa = False
else:
    dosynthlisa = True

seed = options.seed

if options.seednoise == None:
    seednoise = seed
else:
    seednoise = options.seednoise

# setup for parallel run

if options.nproc > 1 and candopar:
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

step1time = time.time()

# --------------------------------------------------------------------------------------
# STEP 1: create source XML parameter files, and collect all of them in Source directory
# --------------------------------------------------------------------------------------

# first empty the Source and Galaxy directories
# makemode won't regenerate sources, so at least step 1 must be completed
# before running in make mode

if (not makemode) and (not sourcekeyfile):
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml Galaxy/*.dat')

    # to run CHALLENGE, a file source-parameter generation script CHALLENGE.py must sit in bin/
    # it must take a single argument (the seed) and put its results in the Source subdirectory
    # if makesource-Galaxy.py is called, it must be with the UNALTERED seed, which is used
    # later to call makeTDIsignals-Galaxy.py

    sourcescript = 'bin/' + challengename + '.py'
    if not os.path.isfile(sourcescript):
        parser.error("I need the challenge script %s!" % sourcescript)
    else:
        run('python %s %s %s' % (sourcescript,seed,istraining))
elif (not makemode) and sourcekeyfile:
    # convoluted logic, to make sure that the keyfile given is not in Source/ and therefore
    # deleted
    run('mv %s %s-backup' % (sourcekeyfile,sourcekeyfile))
    run('rm -f Source/*.xml')
    run('rm -f Galaxy/*.xml Galaxy/*.dat')
    run('mv %s-backup %s' % (sourcekeyfile,sourcekeyfile))
    
    run('cp %s Source/%s' % (sourcekeyfile,os.path.basename(sourcekeyfile)))

step2time = time.time()

# check if any of the source files requests an SN; in this case we'll need to run synthlisa!

if os.system('grep -q RequestSN Source/*.xml') == 0 and dosynthlisa == False:
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
        prun('bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')

pwait()

step3time = time.time()

# --------------------------
# STEP 3: run Synthetic LISA
# --------------------------

# first empty the TDI directory
if (not makemode):
    run('rm -f TDI/*.xml TDI/*.bin TDI/Binary')

if dosynthlisa:
    # then run makeTDI-synthlisa over all the barycentric files in the Barycentric directory
    # the results are files of TDI time series that include the source info
    # these calls have also the result of rescaling the barycentric files to satisfy any RequestSN that
    # they may carry
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        tdifile = 'TDI/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))

        if (not makemode) or newer(xmlfile,tdifile):
            # if we are not generating a Galaxy, don't include its confusion noise in the evaluation of SNR
            if glob.glob('Galaxy/*.xml'):
                prun('bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')
            else:
                prun('bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')

    # now run noise generation...
    noisefile = 'TDI/tdi-frequency-noise.xml'
    # note that we're not checking whether the seed is the correct one
    if donoise and ((not makemode) or (not os.path.isfile(noisefile))):
        prun('bin/makeTDInoise-synthlisa.py --seed=%(seednoise)s --duration=%(duration)s --timeStep=%(timestep)s %(noisefile)s')

    pwait()

step4time = time.time()

# --------------------------
# STEP 4: run LISA Simulator
# --------------------------

if lisasimdir:
    for xmlfile in glob.glob('Barycentric/*-barycentric.xml'):
        tdifile = 'TDI/' + re.sub('\-barycentric.xml','-tdi-strain.xml',os.path.basename(xmlfile))

        if (not makemode) or newer(xmlfile,tdifile):
            # remember that we're not doing any SNR adjusting here...
            prun('bin/makeTDIsignal-lisasim.py --lisasimDir=%(lisasimdir)s %(xmlfile)s %(tdifile)s')
    
    slnoisefile = 'TDI/tdi-strain-noise.xml'

    if donoise and ((not makemode) or (not os.path.isfile(slnoisefile))):
        prun('bin/makeTDInoise-lisasim.py --lisasimDir=%(lisasimdir)s --seedNoise=%(seednoise)s %(slnoisefile)s')
    
    pwait()

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
            prun('bin/makeTDIsignals-Galaxy2.py %s %s %s' % (xmlfile,sltdifile,lstdifile))

    pwait()

step6time = time.time()

# -------------------------
# STEP 6: assemble datasets
# -------------------------

# ignore make mode here, we'll always repackage everything

# first empty the Dataset directory (do not remove tar.gz)
run('rm -f Dataset/'+challengename+'*.xml Dataset/'+challengename+'*.bin Dataset/'+challengename+'*.txt')
# run('rm -f Dataset/*.xml Dataset/*.bin Dataset/*.txt')

# improve dataset metadata here

# do synthlisa first

secretseed = 'source seed = %s,  noise seed = %s' % (seed,seednoise)

if istraining:
    globalseed = secretseed
else:
    globalseed = ''

run('cp Template/lisa-xml.xsl Dataset/.')
run('cp Template/lisa-xml.css Dataset/.')

if dosynthlisa:
    # set filenames

    if istraining:
        nonoisefile   = 'Dataset/' + challengename + '-frequency-nonoise-training.xml'
        withnoisefile = 'Dataset/' + challengename + '-frequency-training.xml'

        nonoisetar    = challengename + '-frequency-nonoise-training.tar.gz'
        withnoisetar  = challengename + '-frequency-training.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key-training.xml'
    else:
        nonoisefile   = 'Dataset/' + challengename + '-frequency-nonoise.xml'
        withnoisefile = 'Dataset/' + challengename + '-frequency.xml'

        nonoisetar    = challengename + '-frequency-nonoise.tar.gz'
        withnoisetar  = challengename + '-frequency.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key.xml'

    # create empty files

    lisaxml.lisaXML(nonoisefile,
                    author="MLDC Task Force",
                    comments='No-noise dataset for %s (synthlisa version), %s' % (challengename,globalseed)).close()

    lisaxml.lisaXML(withnoisefile,
                    author="MLDC Task Force",
                    comments='Full dataset for %s (synthlisa version), %s' % (challengename,globalseed)).close()

    lisaxml.lisaXML(keyfile,
                    author="MLDC Task Force",
                    comments='XML key for %s, %s' % (challengename,secretseed)).close()

    # add signals and noise to the no-noise and with-noise files
    # omit keys if we're not doing training

    if istraining:
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-frequency.xml')

        if donoise:
            run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(noisefile)s')
    else:
        if glob.glob('TDI/*-tdi-frequency.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-frequency.xml')

        if donoise:
            run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(noisefile)s')

    # create the key with all source info

    if glob.glob('TDI/*-tdi-frequency.xml'):
        run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-frequency.xml')

    # now do some tarring up, including XSL and CSS files from Template

    os.chdir('Dataset')

    nonoisefile   = os.path.basename(nonoisefile)
    withnoisefile = os.path.basename(withnoisefile)

    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (nonoisetar,  nonoisefile,  re.sub('\.xml','-[0-9].bin',nonoisefile  )))

    if donoise:
        run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-[0-9].bin',withnoisefile)))    

    os.chdir('..')

# next do LISA Simulator

if lisasimdir:
    # set filenames

    if istraining:
        nonoisefile   = 'Dataset/' + challengename + '-strain-nonoise-training.xml'
        withnoisefile = 'Dataset/' + challengename + '-strain-training.xml'

        nonoisetar    = challengename + '-strain-nonoise-training.tar.gz'
        withnoisetar  = challengename + '-strain-training.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key-training.xml'
    else:
        nonoisefile   = 'Dataset/' + challengename + '-strain-nonoise.xml'
        withnoisefile = 'Dataset/' + challengename + '-strain.xml'

        nonoisetar    = challengename + '-strain-nonoise.tar.gz'
        withnoisetar  = challengename + '-strain.tar.gz'

        keyfile       = 'Dataset/' + challengename + '-key.xml'
    
    # create empty files
    
    lisaxml.lisaXML(nonoisefile,
                    author = 'MLDC task force',
                    comments='No-noise dataset for %s (lisasim version), %s' % (challengename,globalseed)).close()

    lisaxml.lisaXML(withnoisefile,
                    author = 'MLDC task force',
                    comments='Full dataset for %s (lisasim version), %s' % (challengename,globalseed)).close()

    # add signals and noise

    if istraining:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py %(nonoisefile)s TDI/*-tdi-strain.xml')

        if donoise:
            run('bin/mergeXML.py %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')
    else:
        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -n %(nonoisefile)s TDI/*-tdi-strain.xml')
        
        if donoise:
            run('bin/mergeXML.py -n %(withnoisefile)s %(nonoisefile)s %(slnoisefile)s')

    # do key file, but only if synthlisa has not run, otherwise it will be erased

    if not dosynthlisa:
        lisaxml.lisaXML(keyfile,comments='XML key for %s, %s' % (challengename,secretseed)).close()

        if glob.glob('TDI/*-tdi-strain.xml'):
            run('bin/mergeXML.py -k %(keyfile)s TDI/*-tdi-strain.xml')

    # now do some tarring up, including XSL and CSS files from Template

    os.chdir('Dataset')

    nonoisefile   = os.path.basename(nonoisefile)
    withnoisefile = os.path.basename(withnoisefile)

    run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (nonoisetar,  nonoisefile,  re.sub('\.xml','-[0-9].bin',nonoisefile  )))

    if donoise:
        run('tar zcf %s %s %s lisa-xml.xsl lisa-xml.css' % (withnoisetar,withnoisefile,re.sub('\.xml','-[0-9].bin',withnoisefile)))    

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
print "    Total time         : %s" % timestring(endtime   - step0time)
print "    Sources time       : %s" % timestring(step3time - step1time)
print "    Synthetic LISA time: %s" % timestring(step4time - step3time)
print "    LISA Simulator time: %s" % timestring(step5time - step4time)
print "    Fast Galaxy time   : %s" % timestring(step6time - step5time)

# exit with success code
sys.exit(0)

# TO DO:
# - must add noise and geometry specification (do it with files in Template)
# - must do some instructions
