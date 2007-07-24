#!/usr/bin/env python

__version__ = '$Id$'

# master install script for challenge 2 
# re-run after every SVN update
# see comments below for command-line parameters

import sys
import os
import glob
import platform
import re

from distutils.sysconfig import get_python_lib
from distutils.dep_util import newer, newer_group

libdir = None
gsldir = None

newsynthlisa = False
newlisasim   = False
dotraits     = False

for arg in sys.argv:
    if arg.startswith('--prefix='):             # main library dir
        libdir = arg.split('=', 1)[1]
    elif arg.startswith('--gsl='):              # use pre-installed GSL
        gsldir = arg.split('=',1)[1]
    elif arg.startswith('--newsynthlisa'):      # force synthlisa reinstallation
        newsynthlisa = True
    elif arg.startswith('--newlisasim'):        # force lisasim reinstallation
        newlisasim = True
    elif arg.startswith('--traits'):            # include experimental traits functionality
        dotraits = True

if libdir == None:
    print "!!! You need to specify a master lib dir with --prefix=<libdir>"
    print "    (i.e., where is all my stuff?)"
    sys.exit(1)

if gsldir == None:
    gsldir = libdir

pythonlib = get_python_lib(prefix=libdir)

if pythonlib not in sys.path:
    print "!!! You need to add %s to PYTHONPATH (use export or setenv)" % pythonlib
    print "    (sorry, I can't do it from here, it wouldn't stick...)"
    print "    (if you're on a 64-bit platform, you should add also %s)" % get_python_lib(prefix=libdir,plat_specific=True)
    sys.exit(1)

# note: the plat_specific=True location is used for all C extensions... may need to check if that
# should be used in one of the setup.py files

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
    # run patch to exclude numarray compilation on cygwin
    if 'CYGWIN' in platform.system():
        assert(0 == os.system('patch numpy-1.0/numpy/setup.py Patch-cygwin/numpy-setup.py'))
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

# install/check install for GalacticBinary waveform

os.chdir('MLDCwaveforms/GalacticBinary')
print "--> Installing/refreshing GalacticBinary"
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

if ( newer_group(sources,'Galaxy_Maker') or newer_group(sources,'Galaxy_key')  or
     newer_group(sources,'Fast_XML_LS')  or newer_group(sources,'Fast_XML_SL') or
                                            newer_group(sources,'DataImport') ):
    print "    (recompiling Galaxy)"
    assert(0 == os.system('./Compile --gsl=' + gsldir))

# copy every time...

if platform.system() == 'Darwin':
    if platform.processor() == 'i386':
        assert(0 == os.system('cp Fast_Response_OSX_Intel Fast_Response'))
    else:
        # assume platform.processor() == 'powerpc'
        # I'm not sure how to differentiate between G4 and G5, so will use G5...
        assert(0 == os.system('cp Fast_Response_OSX_G5 Fast_Response'))
elif platform.system() == 'Linux':
    assert(0 == os.system('cp Fast_Response_LINUX_X86_32 Fast_Response'))
elif 'CYGWIN' in platform.system():
    print "!!! I'm on cygwin... I'm skipping Galaxy for the moment."
else:
    print "!!! Can't determine platform/processor, or don't have Fast_Response executable!" 
    sys.exit(1)

if not os.path.isfile('Data/dwd_GWR_all_pars.dat') and (not 'CYGWIN' in platform.system()):
    print "    (downloading Nelemans galaxy (388M), this will take a while...)"
    assert(0 == os.system('curl http://www.physics.montana.edu/faculty/cornish/LISA/dwd_GWR_all_pars.dat.gz > dwd_GWR_all_pars.dat.gz'))
    assert(0 == os.system('gunzip dwd_GWR_all_pars.dat.gz'))
    assert(0 == os.system('mv dwd_GWR_all_pars.dat Data/.'))

os.chdir(here)

# install/check install for synthlisa

print "--> Checking synthLISA"
try:
    assert(not newsynthlisa)
    import synthlisa
except:
    print "--> Installing Synthetic LISA"
    os.chdir('Packages')
    # find the latest .tar.gz
    synthlisatargz = glob.glob('synthLISA-*.tar.gz')[-1]
    assert(0 == os.system('tar zxf %s' % synthlisatargz))
    synthlisatar = re.sub('\.tar\.gz','',synthlisatargz)
    os.chdir(synthlisatar)
    assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
    os.chdir('..')
    assert(0 == os.system('rm -rf %s' % synthlisatar))
    os.chdir(here)

# install/check install for LISA Simulator

print "--> Checking LISA Simulator"

lisasimtar = os.path.abspath('Packages/Simulator-2.1.1b.tar.gz')
lisasimdir = 'Simulator2.1.1'

if not os.path.isdir(libdir + '/lisasimulator-1year') or newlisasim:
    print "--> Installing LISA Simulator (1-year version)"

    if newlisasim and os.path.isdir(libdir + '/lisasimulator-1year'):
        assert(0 == os.system('rm -rf %s' % libdir + '/lisasimulator-1year'))

    # untar
    os.chdir(libdir)
    assert(0 == os.system('tar zxf %s -C .' % lisasimtar))
    assert(0 == os.system('mv %s lisasimulator-1year' % lisasimdir))

    # copy modified LISAconstants.h file
    assert(0 == os.system('cp %s lisasimulator-1year/LISAconstants.h' % (here + '/Packages/LISASimulator/LISAconstants-1year.h')))

    # patch IO on cygwin
    if 'CYGWIN' in platform.system():
        assert(0 == os.system('patch lisasimulator-1year/IO/ezxml.c %s' % (here + '/Packages/Patch-cygwin/io-C-ezxml.c')))

    # compile and setup
    os.chdir('lisasimulator-1year')
    assert(0 == os.system('./Compile'))
    assert(0 == os.system('./Setup'))
    os.chdir('../..')

if not os.path.isdir(libdir + '/lisasimulator-2year') or newlisasim:
    print "--> Installing LISA Simulator (2-year version)"

    if newlisasim and os.path.isdir(libdir + '/lisasimulator-2year'):
        assert(0 == os.system('rm -rf %s' % libdir + '/lisasimulator-2year'))

    # untar
    os.chdir(libdir)
    assert(0 == os.system('tar zxf %s -C .' % lisasimtar))
    assert(0 == os.system('mv %s lisasimulator-2year' % lisasimdir))

    # copy modified LISAconstants.h file
    assert(0 == os.system('cp %s lisasimulator-2year/LISAconstants.h' % (here + '/Packages/LISASimulator/LISAconstants-2year.h')))

    # patch IO on cygwin
    if 'CYGWIN' in platform.system():
        assert(0 == os.system('patch lisasimulator-2year/IO/ezxml.c %s' % (here + '/Packages/Patch-cygwin/io-C-ezxml.c')))

    # compile and setup
    os.chdir('lisasimulator-2year')
    assert(0 == os.system('./Compile'))
    assert(0 == os.system('./Setup'))
    os.chdir('../..')

os.chdir(here)
print >> open('MLDCpipelines2/bin/lisasimulator.py','w'), "lisasim1yr = '%s'; lisasim2yr = '%s'" % (libdir + '/lisasimulator-1year',
                                                                                                    libdir + '/lisasimulator-2year')
# install/check install for traits

if dotraits:
    try:
        import enthought.traits
    except:
        try:
            import wx
            if not '2.6.3' in wx.__version__:
                raise
        except:
            print "!!! Sorry, but Enthought Traits requires wxPython 2.6.3.3"
            print "    (you can get it from http://www.wxpython.org)"
        else:        
            print "--> Installing traits"
            os.chdir('Packages')
            assert(0 == os.system('tar zxf enthought.traits-1.1.0-src.tar.gz'))
            # compile in place, then move to python libdir
            os.chdir('enthought.traits-1.1.0/enthought/traits')
            assert(0 == os.system('python setup_minimal.py build_ext --inplace'))
            os.chdir('../../..')
            assert(0 == os.system('mv enthought.traits-1.1.0/enthought %s' % pythonlib))
            assert(0 == os.system('rm -rf enthought.traits-1.1.0'))
            os.chdir(here)
