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

for arg in sys.argv:
    if arg.startswith('--prefix='):
        libdir = arg.split('=', 1)[1]
    elif arg.startswith('--lisasim='):
        lisasim = arg.split('=',1)[1]

if libdir == None:
    print "!!! You need to specify a master lib dir with --prefix=<libdir>"
    print "    (i.e., where is all my stuff?)"
    sys.exit(1)

if lisasim == None:
    lisasim = libdir + '/Simulator2.1.1'    

pythonlib = get_python_lib(prefix=libdir)

if pythonlib not in sys.path:
    print "!!! You need to add %s to PYTHONPATH (use export or setenv)" % pythonlib
    print "    (sorry, I can't do it from here, it wouldn't stick...)"
    sys.exit(1)

here = os.getcwd()

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
    os.chdir('..')
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

# install/check install for Galaxy TDI

os.chdir('MLDCwaveforms/Galaxy')
print "--> Installing/refreshing Galaxy"

sources = glob.glob('*.c') + glob.glob('*.h') + glob.glob('../../lisaXML/io-C/*.c') + glob.glob('../../lisaXML/io-C/*.h')

if ( newer_group(sources,'Galaxy_Maker') or newer_group(sources,'Galaxy_key') or
     newer_group(sources,'Fast_XML_LS') or newer_group(sources,'DataImport') ):
     print "    (recompiling Galaxy)"
     assert(0 == os.system('./Compile'))

os.chdir(here)

# install/check install for synthlisa

print "--> Checking synthLISA"
try:
    import synthlisa
except:
    print "!!! You need to install synthlisa > 1.3.2!"
    print "    (I can't do it for you, since synthlisa is not part of lisatools)"
    print "!!! CD into synthlisa's distribution directory, and run"
    print "    python setup.py install --prefix=%s" % libdir

# install/check install for LISA Simulator

print "--> Checking LISA Simulator"

if not os.path.isdir(lisasim):
    print "!!! I can't find the LISA Simulator in %s" % libdir
    print "!!! Please unpack it there, or in a location of your choice"
    print "    (but then tell me with --lisasim=YOURLOCATION)"
    syu.exit(1)

os.chdir(lisasim)

# it would be nice to have the LISA Simulator compile with the latest io-C,
# but perhaps we can leave this for later

datfiles = ( glob.glob('Binary/Setup.dat') +  glob.glob('Binary/UnVc*.dat') + glob.glob('Binary/Vert*.dat') )
sources = glob.glob('*.c') + glob.glob('*.h')

print "    (I will assume you have not randomly deleted any file in Binary/)"

import operator
if datfiles == [] or reduce(operator.__or__,[newer_group(sources,datfile) for datfile in datfiles]):
    print "    (Running Compile and Setup... this will take a while.)"
    print "    (Disregard all the warnings... this is Neil's code after all...)"
    assert(0 == os.system('./Compile'))
    assert(0 == os.system('./Setup'))

os.chdir(here)

# now let's tell the pipeline scripts where to find the LISA Simulator...

print >> open('MLDCpipelines2/bin/lisasimulator.py','w'), "lisasim = '%s'" % lisasim
