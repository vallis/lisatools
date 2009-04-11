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
import lisasimulator


import numpy
import numpy.oldnumeric as Numeric

from math import pi, sin, cos, sqrt


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

parser = OptionParser(usage=\
"usage: %prog [options] CHALLENGENAME [Challenge1.3 / Challenge2.2 / Challenge1B.2 / Challenge1B.3 / Challenge3.2 /Challenge3.3 / Challenge3.4]",\
 version="$Id: $")


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
    parser.error("I need the challenge name: Challenge1.3 / Challenege2.2 / Challenge1B.2 / Challenge1B.3 / Challenge3.2 /Challenge3.3 /Challenge 3.4 ")

challengename = args[0]
timestep = options.timestep
duration = options.duration

if duration == 62914560:
   lisasimdir = lisasimulator.lisasim2yr
elif options.duration == 31457280:
   lisasimdir = lisasimulator.lisasim1yr


KeysChal1_3 = ["Key/challenge1.3.1-key.xml", "Key/challenge1.3.2-key.xml","Key/challenge1.3.4-key.xml"]

if (challengename == "Challenge1.3"):
   for xmlKey in KeysChal1_3:
       baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
  #     run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
       tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryKeyFile))
       tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryKeyFile))
       run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryKeyFile)s %(tdiKeyFileF)s')
       run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=/home/stas/lisasimulator-2year/  --duration=%(duration)s  %(baryKeyFile)s %(tdiKeyFileS)s')

if (challengename == "Challenge2.2"):
       xmlKey1 = "Key/challenge2.2BBH-key.xml"
       run("bin/create_quadratures.py %s" % xmlKey1)
       for xmlKey in [xmlKey1, "Key/challenge2.2BBH-key_0.xml"]:
            baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
       #     run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
       barycentric = 'Barycentric/'+challengename+'/*key_*barycentric.xml'
       for baryxmlfile in glob.glob(barycentric):
            tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryxmlfile))
            tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryxmlfile))
	    if (re.search('key', baryxmlfile) != None):
        #       run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryxmlfile)s %(tdiKeyFileF)s')
               run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=/home/stas/lisasimulator-2year/  --duration=%(duration)s  %(baryxmlfile)s %(tdiKeyFileS)s')

if (challengename == "Challenge1B.2"):
    xmlKeys = ["Key/Challenge1B/challenge1B.2.1-key.xml", "Key/Challenge1B/challenge1B.2.2-key.xml"]
    for key in xmlKeys:
       run("bin/create_quadratures.py %s" % key)
    xmlKeys.append("Key/Challenge1B/challenge1B.2.1-key_0.xml")
    xmlKeys.append("Key/Challenge1B/challenge1B.2.2-key_0.xml")
    for xmlKey in xmlKeys:
        if (re.search('1B.2.1', xmlKey) != None):
           baryKeyFile = 'Barycentric/Challenge1B.2.1/' + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
           run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
           tdiKeyFileF = 'TDI/Challenge1B.2.1/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryKeyFile))
           run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryKeyFile)s %(tdiKeyFileF)s')
        elif (re.search('1B.2.2', xmlKey) != None):
            baryKeyFile = 'Barycentric/Challenge1B.2.2/' + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
            run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
            tdiKeyFileF = 'TDI/Challenge1B.2.2/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryKeyFile))
            run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryKeyFile)s %(tdiKeyFileF)s')
    
KeysChal1B_3 = ['Key/Challenge1B/challenge1B.3.1-key.xml', 'Key/Challenge1B/challenge1B.3.2-key.xml', \
'Key/Challenge1B/challenge1B.3.3-key.xml', 'Key/Challenge1B/challenge1B.3.4-key.xml', 'Key/Challenge1B/challenge1B.3.5-key.xml']        

if (challengename == "Challenge1B.3"):
     for xmlKey in KeysChal1B_3:
        baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
        run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
        tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryKeyFile))
        tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryKeyFile))
        run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryKeyFile)s %(tdiKeyFileF)s')
        #run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=/home/stas/lisasimulator-2year/  --duration=%(duration)s  %(baryKeyFile)s %(tdiKeyFileS)s')    
            
if (challengename == "Challenge3.2"):
   xmlKeysChal3 = ["Key/challenge3.2-training-key.xml"]
#   xmlKeys = ["Key/challenge3.2-key.xml", "Key/challenge3.3-key.xml", "Key/challenge3.4-key.xml"]
   for xmlKey in xmlKeysChal3:
      baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
      run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
   barycentric = 'Barycentric/'+challengename+'/*key-*barycentric.xml'
   for baryxmlfile in glob.glob(barycentric):
      tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryxmlfile))
      tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryxmlfile))
      if (re.search('key', baryxmlfile) != None):
         run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryxmlfile)s %(tdiKeyFileF)s')
         run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=%(lisasimdir)s  --duration=%(duration)s  %(baryxmlfile)s %(tdiKeyFileS)s')


if (challengename == "Challenge3.3"):
   xmlKeysChal3 = ["Key/challenge3.3-training-key.xml"]
   #   xmlKeys = ["Key/challenge3.2-key.xml", "Key/challenge3.3-key.xml", "Key/challenge3.4-key.xml"]
   for xmlKey in xmlKeysChal3:
      baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
      run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
   barycentric = 'Barycentric/'+challengename+'/*key-*barycentric.xml'
   for baryxmlfile in glob.glob(barycentric):
      tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryxmlfile))
      tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryxmlfile))
      if (re.search('key', baryxmlfile) != None):
         run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryxmlfile)s %(tdiKeyFileF)s')
         run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=%(lisasimdir)s  --duration=%(duration)s  %(baryxmlfile)s %(tdiKeyFileS)s')


if (challengename == "Challenge3.4"):
   xmlKeysChal3 = ["Key/challenge3.4-training-key.xml"]
   #   xmlKeys = ["Key/challenge3.2-key.xml", "Key/challenge3.3-key.xml", "Key/challenge3.4-key.xml"]
   for xmlKey in xmlKeysChal3:
      baryKeyFile = 'Barycentric/'+challengename+"/" + re.sub('\.xml$','-barycentric.xml',os.path.basename(xmlKey))
      run('../MLDCpipelines2/bin/makebarycentric.py --duration=%(duration)s --timeStep=%(timestep)s %(xmlKey)s %(baryKeyFile)s')
   barycentric = 'Barycentric/'+challengename+'/*key-*barycentric.xml'
   for baryxmlfile in glob.glob(barycentric):
      tdiKeyFileF = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-frequency.xml',os.path.basename(baryxmlfile))
      tdiKeyFileS = 'TDI/'+challengename + '/' + re.sub('barycentric\.xml$','tdi-strain.xml',os.path.basename(baryxmlfile))
      if (re.search('key', baryxmlfile) != None):
         run('../MLDCpipelines2/bin/makeTDIsignal-synthlisa.py --noiseOnly --duration=%(duration)s --timeStep=%(timestep)s %(baryxmlfile)s %(tdiKeyFileF)s')
         run('../MLDCpipelines2/bin/makeTDIsignal-lisasim.py --lisasimDir=%(lisasimdir)s  --duration=%(duration)s  %(baryxmlfile)s %(tdiKeyFileS)s')
            
            
#, "Key/challenge3.3-training-key.xml", "Key/challenge3.4-training-key.xml            