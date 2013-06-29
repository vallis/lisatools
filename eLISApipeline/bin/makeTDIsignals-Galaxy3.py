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

parser.add_option("-g", "--general",
                  action="store_true", dest="general", default=False,
                  help="use Galaxy_General instead of Galaxy [off by default]")

mydir = os.path.dirname(os.path.abspath(sys.argv[0]))

(options, args) = parser.parse_args()

if len(args) != 3:
    parser.error("I need the Galaxy input file and the output TDI files!")

galaxyfile = args[0]

sltdifile = args[1]
lstdifile = args[2]

galaxywhich = None

# find the Galaxy table

readgalaxy = lisaxml.readXML(galaxyfile)
sources = readgalaxy.getLISASources()

stream = None

# will use the first PlaneWaveTable it finds, but skip the -bright Galaxies...

for src in sources:
    if type(src) == lisaxml.PlaneWaveTable:
        if '-bright' not in src.name:
            streamlen = src.Table.StreamLength
            
            # should be easy to generalize this to recognize which Galaxy code should be used
            
            if (src.Table.parameters == ['Frequency', 'EclipticLatitude', 'EclipticLongitude','Amplitude',
                                         'Inclination', 'Polarization', 'InitialPhase'] and streamlen > 0):
                galaxywhich = 1
                stream = src.Table.StreamName
                break
            elif (src.Table.parameters == ['Frequency', 'FrequencyDerivative', 'EclipticLatitude', 'EclipticLongitude','Amplitude',
                                         'Inclination', 'Polarization', 'InitialPhase'] and streamlen > 0):
                galaxywhich = 3
                stream = src.Table.StreamName
                break

if not stream:
    print "Cannot find Galaxy table in ", galaxyfile
    sys.exit(1)

# doesn't really matter, may as well make it a number of cosmic significance
streamseed = 42

readgalaxy.close()

# try to find the Galaxy code..

if galaxywhich == 1:
    galaxydir = os.path.abspath(mydir + '/../../MLDCwaveforms/Galaxy')
elif galaxywhich == 3:
    if not options.general:
        galaxydir = os.path.abspath(mydir + '/../../MLDCwaveforms/Galaxy3')
    else:
        galaxydir = os.path.abspath(mydir + '/../../MLDCwaveforms/Galaxy_General')
else:
    print "!!! I'm not sure what kind of Galaxy you're giving me here!"
    sys.exit(1)

# see if the fast galaxy code is there

if not os.path.isfile(galaxydir + '/Fast_Response') and not os.path.isfile(galaxydir + '/Fast_Response3'):
    print "Cannot find the fast Galaxy code! Try re-running master-install.py."
    sys.exit(1)

# --- make a virtual Galaxy environment

here = os.getcwd()

workdir = os.path.abspath(tempfile.mkdtemp(dir='.'))
os.chdir(workdir)

if galaxywhich == 1 or (galaxywhich == 3 and options.general == True):
    execfiles = ['Fast_Response','Fast_XML_LS','Fast_XML_SL','Galaxy_key','Galaxy_Maker']
else:
    execfiles = ['Fast_Response3','Fast_XML_LS3','Fast_XML_SL3','Galaxy_key3','Galaxy_Maker3']

if galaxywhich == 3 and options.general == True:
    for f in execfiles:
        run('ln -s %s/%s ./%s3' % (galaxydir,f,f),quiet=True)
else:
    for f in execfiles:
        run('ln -s %s/%s ./%s' % (galaxydir,f,f),quiet=True)

run('mkdir Binary',quiet=True)
run('mkdir Data',quiet=True)
run('mkdir XML',quiet=True)

if galaxywhich == 1:
    run('ln -s %s/Data/dwd_GWR_all_pars.dat ./Data/dwd_GWR_all_pars.dat' % galaxydir,quiet=True)
    print 'ln -s %s/Data/dwd_GWR_all_pars.dat ./Data/dwd_GWR_all_pars.dat' % galaxydir
    run('ln -s %s/Data/Verification.dat ./Data/Verification.dat' % galaxydir,quiet=True)
    print 'ln -s %s/Data/Verification.dat ./Data/Verification.dat' % galaxydir
elif galaxywhich == 3:
    run('ln -s %s/Data/AMCVn_GWR_MLDC.dat ./Data/AMCVn_GWR_MLDC.dat' % galaxydir,quiet=True)
    print 'ln -s %s/Data/AMCVn_GWR_MLDC.dat ./Data/AMCVn_GWR_MLDC.dat' % galaxydir
    run('ln -s %s/Data/dwd_GWR_MLDC.dat ./Data/dwd_GWR_MLDC.dat' % galaxydir,quiet=True)
    print 'ln -s %s/Data/dwd_GWR_MLDC.dat ./Data/dwd_GWR_MLDC.dat' % galaxydir

# begin work

os.chdir(here)

galaxyfile = os.path.abspath(galaxyfile)
sltdifile = os.path.abspath(sltdifile)
lstdifile = os.path.abspath(lstdifile)

os.chdir(workdir)

run('ln -s %s Data/Galaxy_%s.dat' % (stream,streamseed),quiet=True)
print >> open('Data/count_%s.dat' % streamseed,'w'), ("%s 0" % streamlen)

if galaxywhich == 1:
    run('./Fast_Response %s' % streamseed)
    run('./Fast_XML_LS TheGalaxy Data/Sources_LS.txt')
    run('./Fast_XML_SL TheGalaxy Data/Sources_SL.txt')
elif galaxywhich == 3:
    run('./Fast_Response3 %s' % streamseed)
    run('./Fast_XML_LS3 TheGalaxy Data/Sources_LS.txt')
    run('./Fast_XML_SL3 TheGalaxy Data/Sources_SL.txt')
    print './Fast_Response3 %s' % streamseed
    print './Fast_XML_LS3 TheGalaxy Data/Sources_LS.txt'
    print './Fast_XML_SL3 TheGalaxy Data/Sources_SL.txt'

sltdiobj = lisaxml.lisaXML(sltdifile)
sltdiobj.close()

run('%s/mergeXML.py %s XML/TheGalaxy_synthLISA_noisefree.xml %s' % (mydir,sltdifile,galaxyfile))

lstdiobj = lisaxml.lisaXML(lstdifile)
lstdiobj.close()

run('%s/mergeXML.py %s XML/TheGalaxy_LISAsim_noisefree.xml   %s' % (mydir,lstdifile,galaxyfile))

os.chdir(here)

# ...remove the tmp dir

run('rm -rf %s' % workdir,quiet=True)

sys.exit(0)
