#!/usr/bin/env python

# master install script for challenge 2 
# re-run after every SVN update

import sys
import os
import glob

from distutils.sysconfig import get_python_lib
from distutils.dep_util import newer_group

libdir = None
lisasim = None
synthlisatar = None
gsldir = None

for arg in sys.argv:
    if arg.startswith('--prefix='):
        libdir = arg.split('=', 1)[1]
    elif arg.startswith('--lisasim='):
        lisasim = arg.split('=',1)[1]
    elif arg.startswith('--synthlisa='):
        synthlisatar = arg.split('=',1)[1]
    elif arg.startswith('--gsl='):
        gsldir = arg.split('=',1)[1]

if libdir == None:
    print "!!! You need to specify a master lib dir with --prefix=<libdir>"
    print "    (i.e., where is all my stuff?)"
    sys.exit(1)

if gsldir == None:
    gsldir = libdir

# change version of LISA Simulator here...

if lisasim == None:
    lisasim = libdir + '/Simulator2.1.1.tgz'    

if synthlisatar == None:
    synthlisatar = libdir + '/synthLISA-1.3.2.tar.gz'

pythonlib = get_python_lib(prefix=libdir)

if pythonlib not in sys.path:
    print "!!! You need to add %s to PYTHONPATH (use export or setenv)" % pythonlib
    print "    (sorry, I can't do it from here, it wouldn't stick...)"
    sys.exit(1)

here = os.getcwd()

# check if we have GSL, install it otherwise

print "--> Checking GSL"
if not os.path.isfile(gsldir + '/bin/gsl-config'):
    print "--> Installing GSL"
    os.chdir('Packages')
    assert(0 == os.system('tar zxf gsl-1.8.tar.gz'))
    os.chdir('gsl-1.8')
    assert(0 == os.system('./configure --prefix=%s' % gsldir))
    assert(0 == os.system('make'))
    assert(0 == os.system('make install'))
    os.chdir('..')
    assert(0 == os.system('rm -rf gsl-1.8'))
    os.chdir(here)

# check if we have numpy, install it otherwise

print "--> Checking numpy"
try:
    import numpy
except:
    print "--> Installing numpy"
    os.chdir('Packages')
    assert(0 == os.system('tar zxf numpy-1.0.tar.gz'))
    os.chdir('numpy-1.0')
    assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
    os.chdir('..')
    assert(0 == os.system('rm -rf numpy-1.0'))
    os.chdir(here)

# check if we have pyRXP, install it otherwise

print "--> Checking pyRXP"
try:
    import pyRXP
except:
    print "--> Installing pyRXP"
    os.chdir('Packages')
    assert(0 == os.system('tar zxf pyRXP-1.11.tar.gz'))
    os.chdir('pyRXP-1.11-20061017/pyRXP')
    assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
    os.chdir('../..')
    assert(0 == os.system('rm -rf pyRXP-1.11-20061017'))
    os.chdir(here)

# check if we have SWIG, install it otherwise 

print "--> Checking SWIG"
if os.system('swig -version > /dev/null') != 0:
    # first check if libdir/bin is on the path
    bindir = libdir + '/bin'
    if bindir not in os.getenv('PATH'):
        print "!!! You need to add %s to the PATH (use export or setenv)" % bindir
        print "    (sorry, I can't do it from here, it wouldn't stick...)"
        sys.exit(1)
    
    print "--> Installing SWIG"
    os.chdir('Packages')
    assert(0 == os.system('tar zxf swig-1.3.29.tar.gz'))
    os.chdir('swig-1.3.29')
    assert(0 == os.system('./configure --prefix=%s' % libdir))
    assert(0 == os.system('make'))
    assert(0 == os.system('make install'))
    os.chdir('..')
    assert(0 == os.system('rm -rf swig-1.3.29'))
    os.chdir(here)
    
# install/check install for lisaxml

os.chdir('lisaXML/io-python')
print "--> Installing/refreshing lisaxml"
assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
os.chdir(here)

# install/check install for BBH waveform

os.chdir('MLDCwaveforms/BBH')
print "--> Installing/refreshing BBH"
assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
os.chdir(here)

# install/check install for EMRI waveform

os.chdir('MLDCwaveforms/EMRI')
print "--> Installing/refreshing EMRI"
assert(0 == os.system('python setup.py install --prefix=%s --with-gsl=%s' % (libdir,gsldir)))
os.chdir(here)

# install/check install for Galaxy TDI

os.chdir('MLDCwaveforms/Galaxy')
print "--> Installing/refreshing Galaxy"

sources = glob.glob('*.c') + glob.glob('*.h') + glob.glob('../../lisaXML/io-C/*.c') + glob.glob('../../lisaXML/io-C/*.h')

if ( newer_group(sources,'Galaxy_Maker') or newer_group(sources,'Galaxy_key') or
     newer_group(sources,'Fast_XML_LS') or newer_group(sources,'DataImport') ):
     print "    (recompiling Galaxy)"
     assert(0 == os.system('./Compile'))

import platform
if platform.system() == 'Darwin' and platform.processor() == 'powerpc':
    # unfortunately I'm not sure how to differentiate between G4 and G5...
    # let's use the G5. I'm sure it will run on G4s...
    assert(0 == os.system('cp Fast_Response_OSX_G5 Fast_Response'))
elif platform.system() == 'Linux':
    assert(0 == os.system('cp Fast_Response_LINUX_X86_32 Fast_Response'))
else:
    print "!!! Can't determine platform/processor, or don't have Fast_Response executable!" 
    sys.exit(1)

os.chdir(here)

# install/check install for synthlisa

print "--> Checking synthLISA"
try:
    import synthlisa
except:
    print "--> Installing Synthetic LISA"
    if not os.path.isfile(synthlisatar):
        print "!!! Can't find the Synthetic LISA archive!"
        print "    (I'm looking for %s)" % synthlisatar
        print "!!! Put it there, or please tell me the filename with --synthlisa=SYNTHLISA.TAR.GZ"        
        sys.exit(1)
    
    os.chdir(libdir)
    
    assert(0 == os.system('tar zxf %s -C .' % synthlisatar))

    synthlisadir = synthlisatar[:-7]
    if not os.path.isdir(synthlisadir):
        print "!!! Can't find Synthetic LISA unpacking directory (I'm trying %s...)" % synthlisadir
        sys.exit(1)
    os.chdir(synthlisadir)

    assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
    
    os.chdir(here)

# install/check install for LISA Simulator

print "--> Checking LISA Simulator"

if not os.path.isdir(libdir + '/lisasimulator-1year') or not os.path.isdir(libdir + '/lisasimulator-2year'):
    print "--> Installing LISA Simulator"
    if not os.path.isfile(lisasim):
        print "!!! Can't find the LISA Simulator archive!"
        print "    (I'm looking for %s)" % lisasim
        print "!!! Put it there, or please tell me the filename with --lisasim=LISASIMULATOR.TAR.GZ"
        sys.exit(1)

    os.chdir(libdir)

    lisasimfile = os.path.basename(lisasim)
    if '.tar.gz' in lisasimfile:
        lisasimdir = lisasimfile[:-7]
    elif '.tgz' in lisasimfile:
        lisasimdir = lisasimfile[:-4]
    else:
        lisasimdir = lisasimfile

    print "    (compiling 1-year version... disregard all warnings, this is Neil's code :]...)"
    assert(0 == os.system('tar zxf %s -C .' % lisasim))
    if not os.path.isdir(lisasimdir):
        print "!!! Can't find LISA Simulator unpacking directory (I'm trying %s...)" % lisasimdir
        sys.exit(1)
    assert(0 == os.system('mv %s lisasimulator-1year' % lisasimdir))
    assert(0 == os.system('cp %s lisasimulator-1year/LISAconstants.h' % (here + '/Packages/LISASimulator/LISAconstants-1year.h')))
    # patching Package.c
    assert(0 == os.system('cp %s lisasimulator-1year/.' % (here + '/Packages/LISASimulator/Package.c')))
    # patching IO
    assert(0 == os.system('cp %s lisasimulator-1year/IO/.' % (here + '/lisaXML/io-C/*.c')))
    assert(0 == os.system('cp %s lisasimulator-1year/IO/.' % (here + '/lisaXML/io-C/*.h')))
    os.chdir('lisasimulator-1year')
    assert(0 == os.system('./Compile'))
    print "    (setting up 1-year version... this will take a while...)"
    assert(0 == os.system('./Setup'))
    os.chdir('..')

    print "    (compiling 2-year version... disregard all warnings, this is Neil's code :]...)"
    assert(0 == os.system('tar zxf %s -C .' % lisasim))
    assert(0 == os.system('mv %s lisasimulator-2year' % lisasimdir))
    assert(0 == os.system('cp %s lisasimulator-2year/LISAconstants.h' % (here + '/Packages/LISASimulator/LISAconstants-2year.h')))
    # patching Package.c
    assert(0 == os.system('cp %s lisasimulator-1year/.' % (here + '/Packages/LISASimulator/Package.c')))
    # patching IO
    assert(0 == os.system('cp %s lisasimulator-2year/IO/.' % (here + '/lisaXML/io-C/*.c')))
    assert(0 == os.system('cp %s lisasimulator-2year/IO/.' % (here + '/lisaXML/io-C/*.h')))
    os.chdir('lisasimulator-2year')
    assert(0 == os.system('./Compile'))
    print "    (setting up 2-year version... this will take a while...)"
    assert(0 == os.system('./Setup'))
    os.chdir('..')

    # now let's tell the pipeline scripts where to find the LISA Simulator...

os.chdir(here)
print >> open('MLDCpipelines2/bin/lisasimulator.py','w'), "lisasim1yr = '%s'; lisasim2yr = '%s'" % (libdir + '/lisasimulator-1year',
                                                                                                        libdir + '/lisasimulator-2year')
    


