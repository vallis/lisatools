#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import math
import os
import glob
import re
from distutils.dep_util import newer, newer_group

import synthlisa
#import lisasimulator

import numpy
import numpy.oldnumeric as Numeric

from math import pi, sin, cos, sqrt

def run(command):
    commandline = command % globals()
    print "--> %s" % commandline

    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME [Challenge3.2, Challenge3.3 or Challenge3.4]", version="$Id: $")


parser.add_option("-b", "--preBuffer",
                  type="float", dest="prebuffer", default=900.0,
                  help="length of data computed before initial time (s) [default 900.0]")

parser.add_option("-p", "--postBuffer",
                  type="float", dest="postbuffer", default=900.0,
                  help="length of data computed after final time (s) [default 900.0]")

parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for waveform, not counting prebuffer (s) [default 0.0]")


parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
                  help="total time for waveform, not counting pre- and postbuffer (s) [default 62914560 = 2^22 * 15]")


parser.add_option("-d", "--timeStep",
                  type="float", dest="timestep", default=15.0,
                  help="timestep for waveform sampling (s) [default 15]")
 
parser.add_option("-k","--usekey",
                  type="string", dest="usekey", default=None,
                  help="generate data using given key file [default: None]")  
                  
parser.add_option("-x", "--dataFile",
                 type="string", dest="dataFile", default=None,
                 help="path and name of the xml with data file [default: None]")   
                 
parser.add_option("-s", "--sourceFile",
                 type="string", dest="sourceFile", default=None,
                 help="path and name of the xml submission file [default: None]")                                
                  
parser.add_option("-S", "--synthlisa",
                  action="store_true", dest="synthlisaonly", default=False,
                  help="run only Synthetic LISA")
                  
parser.add_option("-L", "--lisasim",
                  action="store_true", dest="lisasimonly", default=False,
                  help="run only the LISA Simulator")       

parser.add_option("-m", "--make",
                  action="store_true", dest="makemode", default=False,
                  help="run in make mode (use already-generated source key files and intermediate data products)")                                                               

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name: Challenge3.2/Challenege3.3/Challenge3.4")

challengename = args[0]
timestep = options.timestep
duration = options.duration
makemode = options.makemode

dosynthlisa = not (options.lisasimonly) or options.synthlisaonly
dolisasim   = not (options.synthlisaonly) or options.lisasimonly
if (options.verbose):
   if (dosynthlisa):
      print "we will generate the data using synthetic lisa"
   if (dolisasim):
      print "we will generate the data using lisa simulator"   


if (challengename == "Challenge3.4"):
   dosynthlisa = True
   dolisasim = False

run('cp ../MLDCpipelines2/bin/lisasimulator.py bin/')
run('cp ../MLDCpipelines2/bin/lisasimulator.pyc bin/')

import lisasimulator

if duration == 62914560:
   lisasimdir = lisasimulator.lisasim2yr
elif options.duration == 31457280:
   lisasimdir = lisasimulator.lisasim1yr


##### I : creating barycentric data

if (options.sourceFile  == None):
    print "Use default location of source xml files"   
    sources = "Source/"+ challengename + '/' + "/*xml"
    for xmlfile in glob.glob(sources):
        baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
        if (not makemode) or newer(xmlfile,baryfile):
            run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')
else:
    xmlfile = options.sourceFile 
    if (os.path.isfile(xmlfile) ):
       pass
    else:   
       print "source file ", xmlfile, "cannot be found"
       sys.exit(1)
    baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    if (not makemode) or newer(xmlfile,baryfile):
        run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')       



#### IIa : creating TDI files for all barycentric files using synthetic LISA

if (dosynthlisa):
   barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
   for xmlfile in glob.glob(barycentric):
       if (re.search('key', xmlfile) == None):
            tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
            if (not makemode) or newer(xmlfile, tdifile):
                run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')




#### IIb : creating TDI files for all barycentric files using LISA simulator
if (dolisasim):
   barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
   for xmlfile in glob.glob(barycentric):
       if (re.search('key', xmlfile) == None):
            tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(xmlfile))
            if (not makemode) or newer(xmlfile, tdifile):
               run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=%(lisasimdir)s  --duration=%(duration)s  %(xmlfile)s %(tdifile)s')



### if key is know -- generate the data corresponding to the keys 

if (options.usekey != None):
   key = options.usekey
   if (dolisasim):
      run('./bin/compute-KeyData.py -k %(key)s -L --duration=%(duration)s --timeStep=%(timestep)s %(challengename)s')
   if (dosynthlisa):
      run('./bin/compute-KeyData.py -k %(key)s -S  --duration=%(duration)s --timeStep=%(timestep)s %(challengename)s')


#### IIIa. call evaluation script for synthetic LISA data

if (dosynthlisa):
   keyTdisF = glob.glob('TDI/'+challengename+'/*key-*frequency.xml')
if (dolisasim):
   keyTdisS = glob.glob('TDI/'+challengename+'/*key-*frequency.xml')


if (challengename == "Challenge3.2"):
   logFile = "Results/log_" + challengename
   if (dosynthlisa):
      tdisF = glob.glob('TDI/'+challengename+'/*frequency.xml')
      if (options.dataFile  == None):
         print "use default location of data xmlfile"
         dataTdiF = 'Data/challenge3.2-frequency.xml'
      else:
         dataTdi = options.dataFile 
         dataTdiF = dataTdi   
         print "WARNING: you have supplied the data file: make sure you have specified -L or -S options"  
      if (os.path.isfile(dataTdiF) ):
         pass
      else:   
         print "data file ", dataTdiF, "cannot be found"
         sys.exit(1)
      if (options.usekey != None):   
         ind = 0
         for KeyTDI in (keyTdisF):
            logFilek = logFile + "_" + str(ind)
            run('bin/evaluate-syntheticLISA3.py --maxPhase --Galaxy --usekey  %(logFilek)s  %(dataTdiF)s %(KeyTDI)s %(tdisF)s')
            ind = ind+1
            #  sys.exit(0)
      else:
         run('bin/evaluate-syntheticLISA3.py --maxPhase --Galaxy  %(logFile)s  %(dataTdiF)s %(dataTdiF)s %(tdisF)s')   
   if (dolisasim):
      tdisS = glob.glob('TDI/'+challengename+'/*strain.xml')
      if (options.dataFile  == None):
         print "use default location of data xmlfile"
         dataTdiS = 'Data/challenge3.2-strain.xml'
      else:
         dataTdi = options.dataFile 
         dataTdiS = dataTdi   
         print "WARNING: you have supplied the data file: make sure you have specified -L or -S options"  
      if (os.path.isfile(dataTdiS) ):
         pass
      else:   
         print "data file ", dataTdiS, "cannot be found"
         sys.exit(1)
      if (options.usekey != None):   
         ind = 0
         for KeyTDI in (keyTdisS):
            logFilek = logFile + "_" + str(ind)
            run('bin/evaluate-LISAsimulator3.py --maxPhase --Galaxy --usekey  %(logFilek)s  %(dataTdiS)s %(KeyTDI)s %(tdisS)s')
            ind = ind+1
                  #  sys.exit(0)
      else:
         run('bin/evaluate-LISAsimulator3.py --maxPhase --Galaxy  %(logFile)s  %(dataTdiS)s %(dataTdiS)s %(tdisS)s')  
          
       
if (challengename == "Challenge3.3"):
   logFile = "Results/log_" + challengename
   if (dosynthlisa):
      tdisF = glob.glob('TDI/'+challengename+'/*frequency.xml')
      if (options.dataFile  == None):
         print "use default location of data xmlfile"
         dataTdiF = 'Data/challenge3.3-frequency.xml'
      else:
         dataTdi = options.dataFile 
         dataTdiF = dataTdi   
         print "WARNING: you have supplied the data file: make sure you have specified -L or -S options"  
      if (os.path.isfile(dataTdiF) ):
         pass
      else:   
         print "data file ", dataTdiF, "cannot be found"
         sys.exit(1)
      if (options.usekey != None):   
         ind = 0
         for KeyTDI in (keyTdisF):
            logFilek = logFile + "_" + str(ind)
            run('bin/evaluate-syntheticLISA3.py --usekey  %(logFilek)s  %(dataTdiF)s %(KeyTDI)s %(tdisF)s')
            ind = ind+1
            #  sys.exit(0)
      else:
         run('bin/evaluate-syntheticLISA3.py %(logFile)s  %(dataTdiF)s %(dataTdiF)s %(tdisF)s')   
   if (dolisasim):
      tdisS = glob.glob('TDI/'+challengename+'/*strain.xml')
      if (options.dataFile  == None):
         print "use default location of data xmlfile"
         dataTdiS = 'Data/challenge3.3-strain.xml'
      else:
         dataTdi = options.dataFile 
         dataTdiS = dataTdi   
         print "WARNING: you have supplied the data file: make sure you have specified -L or -S options"  
      if (os.path.isfile(dataTdiS) ):
         pass
      else:   
         print "data file ", dataTdiS, "cannot be found"
         sys.exit(1)
      if (options.usekey != None):   
         ind = 0
         for KeyTDI in (keyTdisS):
            logFilek = logFile + "_" + str(ind)
            run('bin/evaluate-LISAsimulator3.py --usekey  %(logFilek)s  %(dataTdiS)s %(KeyTDI)s %(tdisS)s')
            ind = ind+1
               #  sys.exit(0)
      else:
         run('bin/evaluate-LISAsimulator3.py  %(logFile)s  %(dataTdiS)s %(dataTdiS)s %(tdisS)s')  
       
       
if (challengename == "Challenge3.4"):
   logFile = "Results/log_" + challengename
   if (dosynthlisa):
      tdisF = glob.glob('TDI/'+challengename+'/*frequency.xml')
      if (options.dataFile  == None):
         print "use default location of data xmlfile"
         dataTdiF = 'Data/challenge3.4-frequency.xml'
      else:
         dataTdi = options.dataFile 
         dataTdiF = dataTdi   
         print "WARNING: you have supplied the data file: make sure you have specified -L or -S options"  
      if (os.path.isfile(dataTdiF) ):
         pass
      else:   
         print "data file ", dataTdiF, "cannot be found"
         sys.exit(1)
      if (options.usekey != None):   
         ind = 0
         for KeyTDI in (keyTdisF):
            logFilek = logFile + "_" + str(ind)
            run('bin/evaluate-syntheticLISA3.py --usekey  %(logFilek)s  %(dataTdiF)s %(KeyTDI)s %(tdisF)s')
            ind = ind+1
            #  sys.exit(0)
      else:
         run('bin/evaluate-syntheticLISA3.py   %(logFile)s  %(dataTdiF)s %(dataTdiF)s %(tdisF)s')   
   if (dolisasim):       
      print "There is no LISA simulator data for this challenge"
      sys.exit(1) 
      

