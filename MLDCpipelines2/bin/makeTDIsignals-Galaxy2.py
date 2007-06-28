#!/usr/bin/env python

__version__='$Id: $'

import os
import sys
import tempfile

import lisaxml

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

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] INPUT.xml OUTPUT-synthlisa.xml OUTPUT-lisasim.xml",
                      version="$Id: $")

(options, args) = parser.parse_args()

if len(args) != 3:
    parser.error("I need the Galaxy input file and the output TDI files!")

galaxyfile = args[0]

sltdifile = args[1]
lstdifile = args[2]

# try to find the Galaxy code..

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

# begin work

os.chdir(here)

readgalaxy = lisaxml.readXML(galaxyfile)
sources = readgalaxy.getLISASources()

stream = None

for src in sources:
    if type(src) == lisaxml.PlaneWaveTable:
        if '-bright' not in src.name:
            streamlen = src.Table.StreamLength
            
            if (src.Table.parameters == ['Frequency', 'EclipticLatitude', 'EclipticLongitude','Amplitude',
                                         'Inclination', 'Polarization', 'InitialPhase'] and streamlen > 0):
                stream = src.Table.StreamName
                streamseed = 42
                break

if not stream:
    print "Cannot find Galaxy table1"
    sys.exit(1)

readgalaxy.close()

galaxyfile = os.path.abspath(galaxyfile)
sltdifile = os.path.abspath(sltdifile)
lstdifile = os.path.abspath(lstdifile)

os.chdir(workdir)

run('ln -s %s Data/Galaxy_%s.dat' % (stream,streamseed),quiet=True)
print >> open('Data/count_%s.dat' % streamseed,'w'), ("%s 0" % streamlen)

run('./Fast_Response %s' % streamseed)
run('./Fast_XML_LS TheGalaxy Data/Sources_LS.txt') 
run('./Fast_XML_SL TheGalaxy Data/Sources_SL.txt')

sltdiobj = lisaxml.lisaXML(sltdifile)
sltdiobj.close()

run('%s/bin/mergeXML.py %s XML/TheGalaxy_synthLISA_noisefree.xml %s' % (here,sltdifile,galaxyfile))

lstdiobj = lisaxml.lisaXML(lstdifile)
lstdiobj.close()

run('%s/bin/mergeXML.py %s XML/TheGalaxy_LISAsim_noisefree.xml   %s' % (here,lstdifile,galaxyfile))

os.chdir(here)

# ...remove the tmp dir

run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
