#!/usr/bin/env python

import os
import sys

import lisaxml

def run(command):
    commandline = command % globals()
    print "---> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        sys.exit(1)

# only one argument, and it's the seed...
seed = int(sys.argv[1])

here = os.getcwd()
os.chdir('../MLDCwaveforms/Galaxy')

# compute the instrument response (takes a couple of hours)
run('./Fast_Response %s' % seed)
run('./Fast_XML_LS TheGalaxy Data/Sources_LS.txt') 
run('./Fast_XML_SL TheGalaxy Data/Sources_SL.txt')

sltdiobj = lisaxml.lisaXML(here + '/TDI/Galaxy-tdi-frequency.xml')
sltdiobj.close()

run('%s/bin/mergeXML.py %s/TDI/Galaxy-tdi-frequency.xml XML/TheGalaxy_synthLISA_noisefree.xml %s/Galaxy/Galaxy.xml' % (here,here,here))

lstdiobj = lisaxml.lisaXML(here + '/TDI/Galaxy-tdi-strain.xml')
lstdiobj.close()

run('%s/bin/mergeXML.py %s/TDI/Galaxy-tdi-strain.xml XML/TheGalaxy_LISAsim_noisefree.xml %s/Galaxy/Galaxy.xml' % (here,here,here))

run('rm XML/TheGalaxy_*.xml')
run('rm Binary/*.bin')

run('rm Data/Sources_LS.txt')
run('rm Data/Sources_SL.txt')

run('rm Data/count_%s.dat' % seed)
run('rm Data/Galaxy_%s.dat' % seed)
run('rm Data/Galaxy_Bright_%s.dat' % seed)

# there may be more cleaning to do...

os.chdir(here)
