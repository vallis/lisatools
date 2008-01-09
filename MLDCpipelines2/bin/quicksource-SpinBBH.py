#!/usr/bin/env python

__version__='$Id:  $'

# keep it clean!

import lisaxml

import sys
import os
import re
import getopt

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

# get our location

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: $")

# options for makesource-SpinBBH.py

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="seed for random number generator (int) [required]")

parser.add_option("-d", "--distance",
                  type="float", dest="D", default=1.0e9,
                  help="distance to source (Pc)")

parser.add_option("-R", "--requestSN",
                  type="float", dest="RequestSN", default=None,
                  help="requested source amplitude SN (satisfied at TDI-generation time)")

parser.add_option("-t", "--coalescTime",
                  type="float", dest="Tc", default=None,
                  help="time of coalescence from t = 0 (days) [required]")

parser.add_option("-r", "--coalescRange",
                  type="float", dest="deltaTc", default=None,
                  help="half width of uniform probability distribution for coalescence time around Tc (days) [required]")

parser.add_option("-n", "--sourceName",
                  type="string", dest="sourceName", default="2PN SpinBBH binary",
                  help='name of source [defaults to "2PN SpinBBH binary"]')

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default]")

parser.add_option("-F", "--FastBBH",
                  action="store_true", dest="fastbbh", default=False,
                  help="use Fast SMBH code [off by default]")

# options for makeTDIsignal-synthlisa.py

parser.add_option("-o", "--noiseOnly",
                  action="store_true", dest="noiseOnly", default=False,
                  help="compute SNR using instrumental noise only [off by default]")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined snr = sqrt(2)*max{SNR_x, SNR_y, SNR_z} as SNR constrain [off by default]")

# options from challenge.py

parser.add_option("-N","--noNoise",
                  action="store_true", dest="nonoise", default=False,
                  help="do not include noise [default False]")

parser.add_option("-S", "--synthlisa",
                  action="store_true", dest="synthlisaonly", default=False,
                  help="run only Synthetic LISA")

parser.add_option("-L", "--lisasim",
                  action="store_true", dest="lisasimonly", default=False,
                  help="run only the LISA Simulator")

(options, args) = parser.parse_args()

# see if we have all we need

if len(args) != 1:
    parser.error("You must specify the output file!")
    
if options.seed == None:
    parser.error("You must specify the seed!")
    
if options.Tc == None:
    parser.error("You must specify the coalescence time!")

if options.deltaTc == None:
    parser.error("You must specify the coalescence width!")

# get the name of the output file

outXMLfile = args[0]

# build the parameter file

basename = re.sub('\.xml','',outXMLfile)
sourcefile = basename + '-source.xml'

requestSN = options.RequestSN and ('-S %s' % options.RequestSN) or ''
verbose = options.verbose and '-v' or ''
FastBBH = options.fastbbh and '-F' or ''

run('%s/makesource-SpinBBH.py -s %s -d %s %s -t %s -r %s -n "%s" %s %s %s' % (mydir,options.seed,options.D,requestSN,
                                                                              options.Tc,options.deltaTc,options.sourceName,
                                                                              verbose,FastBBH,sourcefile))

# make the barycentric file

barycentricfile = basename + '-barycentric.xml'

run('%s/makebarycentric.py %s %s' % (mydir,sourcefile,barycentricfile))

# make synthlisa signal (also adjust barycentric-file SNR)

if (not options.lisasimonly) or options.RequestSN:
    if not options.nonoise:
        synthlisafile = basename + '-nonoise-frequency.xml'
    else:
        synthlisafile = basename + '-frequency.xml'

    noiseOnly = options.noiseOnly and '-n' or ''
    combinedSNR = options.combinedSNR and '-c' or ''
    run('%s/makeTDIsignal-synthlisa.py %s %s %s %s' % (mydir,noiseOnly,combinedSNR,barycentricfile,synthlisafile))

    # make synthlisa noise

    if not options.nonoise:
        synthlisanoisefile = basename + '-noiseonly-frequency.xml'
    
        run('%s/makeTDInoise-synthlisa.py -T 62914560.0 -s %s %s' % (mydir,options.seed,synthlisanoisefile))

        mergedfile = basename + '-frequency.xml'
        lisaxml.lisaXML(mergedfile,author = 'Michele Vallisneri',comments='quicksource SBBH file, seed = %s' % options.seed).close()

        run('%s/mergeXML.py %s %s %s' % (mydir,mergedfile,synthlisafile,synthlisanoisefile))

if not options.synthlisaonly:
    if not options.nonoise:
        lisasimfile = basename + '-nonoise-strain.xml'
    else:
        lisasimfile = basename + '-strain.xml'
    
    run('%s/makeTDIsignal-lisasim.py %s %s' % (mydir,barycentricfile,lisasimfile))
    
    if not options.nonoise:
        lisasimnoisefile = basename + '-noiseonly-strain.xml'
        
        run('%s/makeTDInoise-lisasim.py -T 62914560.0 -n %s %s' % (mydir,options.seed,lisasimnoisefile))

        mergedfile = basename + '-strain.xml'
        lisaxml.lisaXML(mergedfile,author = 'Michele Vallisneri',comments='quicksource SBBH file, seed = %s' % options.seed).close()
        
        run('%s/mergeXML.py %s %s %s' % (mydir,mergedfile,lisasimfile,lisasimnoisefile))
