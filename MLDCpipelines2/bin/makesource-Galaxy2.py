#!/usr/bin/env python

__version__='$Id: $'

import sys
import os
import tempfile

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

# set ourselves up to parse command-line options

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] OUTPUT.xml",
                      version="$Id: $")

parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=None,
                  help="Galaxy seed (int) [required]")

parser.add_option("-v", "--verification",
                  action="store_true", dest="verification", default=False,
                  help="include verification binaries [off by default]")

(options, args) = parser.parse_args()

if options.seed == None:
    parser.error("You must give me a seed!")

seed = options.seed
verification = options.verification
    
if len(args) != 1:
    parser.error("I need a location for the output Galaxy file!")

outputfile = args[0]

# try to find the Galaxy code...

galaxydir = os.path.abspath(os.path.dirname(os.path.abspath(sys.argv[0])) + '/../../MLDCwaveforms/Galaxy')

if not os.path.isfile(galaxydir + '/Fast_Response'):
    print "Cannot find the fast Galaxy code!"
    sys.exit(1)

# --- make a virtual Galaxy environment

here = os.getcwd()

workdir = os.path.abspath(tempfile.mkdtemp(dir='.'))
os.chdir(workdir)

execfiles = ['Fast_Response','Fast_XML_LS','Fast_XML_SL','Galaxy_key','Galaxy_Maker']

for f in execfiles:
    run('ln -s %s/%s ./%s' % (galaxydir,f,f),quiet=True)

run('mkdir Binary',quiet=True)
run('mkdir Data',quiet=True)
run('mkdir XML',quiet=True)

run('ln -s %s/Data/dwd_GWR_all_pars.dat ./Data/dwd_GWR_all_pars.dat' % galaxydir,quiet=True)
run('ln -s %s/Data/Verification.dat ./Data/Verification.dat' % galaxydir,quiet=True)

os.chdir(here)
galaxyfile = os.path.abspath(outputfile)

# make Galaxy!

os.chdir(workdir)

if verification:
    run('./Galaxy_Maker %s 1' % seed)
else:
    run('./Galaxy_Maker %s 0' % seed)

run('./Galaxy_key TheGalaxy %s' % seed)

# move the Galaxy XML file

run("sed 's/Data\///g' < XML/TheGalaxy_key.xml > %s" % galaxyfile)

destdir = os.path.dirname(galaxyfile) + '/.'
run("mv Data/Galaxy_%s.dat %s" % (seed,destdir))
run("mv Data/Galaxy_Bright_%s.dat %s" % (seed,destdir))

run('rm -f XML/noise-only.xml',quiet=True)
run('rm -f XML/noise-only.bin',quiet=True)
run('rm -f Data/*.txt',quiet=True)

os.chdir(here)

# ...remove the tmp dir

run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
