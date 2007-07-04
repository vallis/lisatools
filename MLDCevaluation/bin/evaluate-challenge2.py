#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit

import sys
import math
import os
import glob
import re

import synthlisa


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

parser = OptionParser(usage="usage: %prog [options] CHALLENGENAME [Challenge1.3 or Challenge2.2]", version="$Id: $")


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

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("I need the challenge name: Challenge1.3/Challenege2.2")

challengename = args[0]
timestep = options.timestep
duration = options.duration

"""
##### I : creating barycentric data

sources = "Source/"+ challengename + '/' + "/*xml"
for xmlfile in glob.glob(sources):
    baryfile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlfile))
    run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(baryfile)s')


#### IIa : creating TDI files for all barycentric files using synthetic LISA

barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
for xmlfile in glob.glob(barycentric):
    if (re.search('key', xmlfile) == None):
         tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(xmlfile))
         run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlfile)s %(tdifile)s')
"""

#### IIb : creating TDI files for all barycentric files using LISA simulator
"""
barycentric = 'Barycentric/'+challengename+'/*-barycentric.xml'
for xmlfile in glob.glob(barycentric):
    if (re.search('key', xmlfile) == None):
         tdifile = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(xmlfile))
         run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=/local_data_300GB/stas/lisasimulator-2year/  --duration=%(duration)s  %(xmlfile)s %(tdifile)s')
"""

"""
#### call evaluation script for synthetic LISA data

keyTdis = glob.glob('TDI/'+challengename+'/*key-*frequency.xml')

if (challengename == "Challenge1.3"):
   tdis =  glob.glob('TDI/'+challengename+'/*challenge1.3-*frequency.xml')
   for keyTDI in (keyTdis):
       if(re.search('1.3.1', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.1-frequency/challenge1.3.1-frequency.xml'
       elif (re.search('1.3.2', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.2-frequency/challenge1.3.2-frequency.xml'
       elif (re.search('1.3.4', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.4-frequency/challenge1.3.4-frequency.xml'
       run('bin/evaluate-syntheticLISA2.py  %(dataTdi)s %(keyTDI)s %(tdis)s')

if (challengename == "Challenge2.2"):
   tdis = glob.glob('TDI/'+challengename+'/*challenge2.2-*frequency.xml')
   dataTdi = 'Data/challenge2.2-frequency/challenge2.2-frequency.xml'
   for KeyTDI in (keyTdis):
      run('bin/evaluate-syntheticLISA2.py --maxPhase --Galaxy  %(dataTdi)s %(KeyTDI)s %(tdis)s')
"""

#### call evaluation script for LISA Simulator data

keyTdis = glob.glob('TDI/'+challengename+'/*key-*strain.xml')

if (challengename == "Challenge1.3"):
   tdis =  glob.glob('TDI/'+challengename+'/*challenge1.3-*strain.xml')
   for keyTDI in (keyTdis):
       if(re.search('1.3.1', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.1-frequency/challenge1.3.1-strain.xml'
       elif (re.search('1.3.2', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.2-frequency/challenge1.3.2-strain.xml'
       elif (re.search('1.3.4', keyTDI) != None):
           dataTdi = 'Data/challenge1.3.4-frequency/challenge1.3.4-strain.xml'
       run('bin/evaluate-syntheticLISA2.py  %(dataTdi)s %(keyTDI)s %(tdis)s')

if (challengename == "Challenge2.2"):
   tdis = glob.glob('TDI/'+challengename+'/*challenge2.2-*strain.xml')
   dataTdi = 'Data/challenge2.2-frequency/challenge2.2-strain.xml'
   for KeyTDI in (keyTdis):
      run('bin/evaluate-syntheticLISA2.py --maxPhase --Galaxy  %(dataTdi)s %(KeyTDI)s %(tdis)s')
      
