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
import subprocess
import urllib

from distutils.sysconfig import get_python_lib
from distutils.dep_util import newer, newer_group
from distutils.version import LooseVersion

def runcommand(command):
    """Runs a command and returns a 2-tuple of stdout and stderr."""
    
    sp = subprocess.Popen([command], stdout=subprocess.PIPE, shell=True)    
    stdout,stderr = sp.communicate()
    
    return sp.returncode, stdout, stderr

def checkpackage(package):
    """Checks if a Python package is installed, returned the version number (0 if not found)"""
    
    try:
        module = __import__(package)
        
        if hasattr(module,'version'):
            if isinstance(module.version,str):
                return LooseVersion(module.version)
            else:
                if hasattr(module.version,'version_short') and isinstance(module.version.version_short,str):
                    return LooseVersion(module.version.version_short)
                elif hasattr(module.version,'version') and isinstance(module.version.version,str):
                    return LooseVersion(module.version.version)
        elif hasattr(module,'__version__') and isinstance(module.__version__,str):
            return LooseVersion(module.__version__)
        
        print '!!! Cannot find version number for package', package
        sys.exit(1)
    except ImportError:
        return LooseVersion('0')
    

def findpackage(package):
    """Looks for a package, returns the newest version with its version number."""
    
    files = [ re.sub('Packages/','',match) for match in glob.glob('Packages/' + package + '*.tar.gz') ]
    filesandversions = [ (onefile,re.match('.*-([0-9a-z\.]*).tar.gz',onefile).group(1)) for onefile in files ]
    filesandversions.sort(lambda x,y: cmp(x[1],y[1]),reverse=True) # sort by decreasing version number
    
    return filesandversions[0]

def installpackage(package,packagedir=None,prefix=None,keepdownload=False,configureflags=''):
    if 'http:' in package:
        print "---> Downloading " + package + '...'
        packagetar = os.path.basename(package)
        urllib.urlretrieve(package,packagetar)
    elif 'tar.gz' in package:
        packagetar = package
    else:
        packagedir = package
        packagetar = ''
    
    if packagedir == None:
        packagedir = re.sub('.tar.gz','',packagetar)
    
    if prefix == None:
        prefix = gsldir
    
    configureflags += " --prefix=" + prefix
    
    if platform.system() == 'Darwin':
        # attempt to build universal binary version of library
        
        if '10.4' in platform.mac_ver()[0]:
            # previously had also "-O -g"; adding "-arch ppc64 -arch x86_64" would build also 64-bit versions,
            #   which are not backward compatible with Tiger
            os.environ['CFLAGS'] = "-arch i386 -arch ppc"
            os.environ['CXXFLAGS'] = "-arch i386 -arch ppc"
            os.environ['LDFLAGS'] = "-arch i386 -arch ppc"
            
            # the -isysroot is needed because the system is not universal on PPC OSX Tiger
            os.environ['CFLAGS']   += " -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
            os.environ['CXXFLAGS'] += " -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
            os.environ['LDFLAGS']  += " -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
        elif platform.architecture()[0] == '64bit':
            if platform.processor() == 'i386':
                os.environ['CFLAGS']   = "-arch i386 -arch x86_64"
                os.environ['CXXFLAGS'] = "-arch i386 -arch x86_64"
                os.environ['LDFLAGS']  = "-arch i386 -arch x86_64"
            else:
                os.environ['CFLAGS']   = "-arch ppc -arch ppc64"
                os.environ['CXXFLAGS'] = "-arch ppc -arch ppc64"
                os.environ['LDFLAGS']  = "-arch ppc -arch ppc64"
        
        # this is probably not needed anymore...
        # configureflags += " --disable-dependency-tracking"
    
    if packagetar:
        assert(0 == os.system('tar zxf ' + packagetar))
    
    # use the local m4 if there is one
    if prefix != None and os.path.isfile(prefix + '/bin/m4'):
        os.environ['M4'] = prefix + '/bin/m4'
    
    os.chdir(packagedir)
    if os.path.isfile('setup.py'):
        assert(0 == os.system('python setup.py install' + configureflags))
        # special case for LISA...
        if 'synthLISA' in packagetar and makeclib:
            assert(0 == os.system('python setup.py install' + configureflags + ' --make-clib'))
    elif os.path.isfile('configure'):
        assert(0 == os.system('./configure ' + configureflags))
        assert(0 == os.system('make -j %s' % makeproc))
        assert(0 == os.system('make install'))
    else:
        print "!!! Don't know how to install", packagetar
        sys.exit(1)
    os.chdir('..')
    
    if 'm4' in package and prefix != None:
        os.system('ln -fs %s/bin/m4 %s/bin/gm4' % (prefix,prefix))
    
    os.environ['CFLAGS'] = ""
    os.environ['CXXFLAGS'] = ""
    os.environ['LDFLAGS'] = ""
    
    if 'http:' in package or 'tar.gz' in package:
        assert(0 == os.system('rm -rf ' + packagedir))
    
    if 'http:' in package and not keepdownload:
        assert(0 == os.system('rm -rf ' + packagetar))
    

libdir = None
gsldir = None
fftwdir = None

newsynthlisa = False
newlisasim   = False
nolisasim    = False
newlisacode  = False
nolisacode   = False
dotraits     = False
installgsl   = False
installswig  = False
installfftw  = False
downloadgalaxy = False
makeclib = False
makeproc = '1'

for arg in sys.argv:
    if arg.startswith('--prefix='):             # main library dir
        libdir = arg.split('=', 1)[1]
    elif arg.startswith('--gsl='):              # use pre-installed GSL
        gsldir = arg.split('=',1)[1]
    elif arg.startswith('--fftw='):             # use pre-installed FFTW
        fftwdir = arg.split('=',1)[1]        
    elif arg.startswith('--newsynthlisa'):      # force synthlisa reinstallation
        newsynthlisa = True
    elif arg.startswith('--make-clib'):         # make clib (synthlisa alone, for the moment)
        makeclib = True
    elif arg.startswith('--newlisasim'):        # force lisasim reinstallation
        newlisasim = True
    elif arg.startswith('--nolisasim'):         # avoid lisasim installation
        nolisasim = True
    elif arg.startswith('--newlisacode'):       # force lisacode reinstallation
        newlisacode = True
    elif arg.startswith('--nolisacode'):        # avoid lisacode installation
        nolisacode = True
    elif arg.startswith('--traits'):            # include experimental traits functionality
        dotraits = True
    elif arg.startswith('--installgsl'):        # force GSL install
        installgsl = True
    elif arg.startswith('--installswig'):       # force SWIG install
        installswig = True
    elif arg.startswith('--installfftw'):       # force FFTW install
        installfftw = True        
    elif arg.startswith('--nproc'):             # run multiproc make
        makeproc = arg.split('=', 1)[1]
    elif arg.startswith('--downloadgalaxy'):    # force Galaxy download
        if '=' in arg:
            downloadgalaxy = int(arg.split('=',1)[1])
        else:
            downloadgalaxy = True

if libdir == None:
    print "!!! You need to specify a master lib dir with --prefix=<libdir>"
    print "    (i.e., where is all my stuff?)"
    sys.exit(1)

if gsldir == None:
    gsldir = libdir

if fftwdir == None:
    fftwdir = libdir

pythonlib = get_python_lib(prefix=libdir)

if pythonlib not in sys.path:
    print "!!! You need to add %s to PYTHONPATH; use the lines" % pythonlib
    print "    export PYTHONPATH=%s:$PYTHONPATH (in bash)" % pythonlib
    print "    setenv PYTHONPATH %s:$PYTHONPATH (in tcsh)" % pythonlib    
    print "    (Sorry, I can't do it from here, it wouldn't stick...)"
    print "    If you're on a 64-bit platform, you should add also %s." % get_python_lib(prefix=libdir,plat_specific=True)
    sys.exit(1)

# grab SVN revision

assert(0 == os.system("svn info | grep Revision | sed 's/Revision:/lisatoolsrevision =/g' > MLDCpipelines2/bin/lisatoolsrevision.py"))

# note: the plat_specific=True location is used for all C extensions... may need to check if that
# should be used in one of the setup.py files

here = os.getcwd()

# check that our Python is high enough. Currently requiring 2.4

pythonversion = sys.version.split()[0]
if LooseVersion(pythonversion) < LooseVersion('2.4'):
    print "!!! I need a more recent Python (>= 2.4). You should get it from python.org."
    print "    (It could also be already in your system, but the PATH set up so that"
    print "     the shell gets to an older Python first.)"
    sys.exit(1)

# check if we have GSL, install it otherwise
# TO DO: would be nice to check with pkg-config

# later, install after getting from http://mirrors.kernel.org/gnu/gsl/gsl-1.10.tar.gz
#
# to make universal binaries on OS X 10.4:
# export CFLAGS="-O -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc"
# export LDFLAGS="-arch i386 -arch ppc"
# ./configure --prefix=/Users/vallis --disable-dependency-tracking
# make
# make install

print "--> Checking GSL"
if not os.path.isfile(gsldir + '/bin/gsl-config') or installgsl:
    if installgsl == True:
        print "--> Installing GSL"    
        installpackage('http://mirrors.kernel.org/gnu/gsl/gsl-1.10.tar.gz',prefix=gsldir)
        
        # print "--> Installing GSL"
        # os.chdir('Packages')
        # assert(0 == os.system('tar zxf gsl-1.8.tar.gz'))
        # os.chdir('gsl-1.8')
        # assert(0 == os.system('./configure --prefix=%s' % gsldir))
        # assert(0 == os.system('make'))
        # assert(0 == os.system('make install'))
        # os.chdir('..')
        # assert(0 == os.system('rm -rf gsl-1.8'))
        # os.chdir(here)
    else:
        print "!!! I cannot find GSL on your system. If you have it, please specify"
        print "    its location with --gsl=GSLDIR (for instance, --gsl=/usr/local if"
        print "    the GSL libraries are in /usr/local/lib). Otherwise, I can install"
        print "    v1.10 for you if you give me the --installgsl option."
        sys.exit(1)

fftw3package = 'fftw-3.2.1'
fftw3tar = fftw3package + '.tar.gz'
fftwdownload = 'http://www.fftw.org/' + fftw3tar
# also ftp://ftp.fftw.org/pub/fftw/old/

# universal binary technique should work here, too
# may need to remove build dir between two compilations

print "--> Checking FFTW"
if not os.path.isfile(fftwdir + '/include/fftw3.h') or installfftw:
    if installfftw == True:
        print "--> Installing FFTW"    
        installpackage(fftwdownload,prefix=fftwdir,keepdownload=True, configureflags='--enable-shared --enable-threads')
        # redo in single precision, get rid of tar.gz
        installpackage(fftwdownload,prefix=fftwdir,keepdownload=False,configureflags='--enable-shared --enable-float --enable-threads')
    else:
        print "!!! I cannot find FFTW3 on your system. If you have it, please specify"
        print "    its location with --fftw=FFTWDIR (for instance, --fftw=/usr/local if"
        print "    the FFTW3 libraries are in /usr/local/lib). Otherwise, I can install"
        print "    v3.2.1 for you if you give me the --installfftw option."
        sys.exit(1)

# on Leopard, there's a problem with m4 (version 1.4.6) that breaks the LISACode compilation
# therefore we install a newer local version

if (platform.system() == 'Darwin') and ('10.5' in platform.mac_ver()[0]):
    ret, stdo, stde = runcommand('m4 --version')
    m4version = re.search('.* ([0-9\.]*)',stdo).group(1)
    
    if m4version == '1.4.6':
        print "--> I've found m4 version 1.4.6; I'll install 1.4.9 locally."
        installpackage('http://ftp.gnu.org/gnu/m4/m4-1.4.9.tar.gz',prefix=libdir,keepdownload=False)

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

# check if we have SWIG, install it if not, or if it needs upgrading
# use this template for more version checking!

print "--> Checking SWIG"
package, packageversion = findpackage('swig')
packagedir = re.sub('.tar.gz','',package)

# see if we can get the SWIG version currently installed

ret, stdo, stde = runcommand('swig -version')
if ret == 0:
    try:
        # if we can, compare it with what we have and update it if necessary
        swigversion = re.search('SWIG Version ([0-9\.]*)',stdo).group(1)
        if LooseVersion(swigversion) < LooseVersion(packageversion):
            print "---> I am finding version %s, but I'll install the newer %s" % (swigversion,packageversion)
            ret = 1
    except:
        print "!!! I cannot determine the SWIG version; currently lisatools is shipping"
        print "    with %s. If you are having problems with SWIG, please force its" % package
        print "    install by giving the --installswig option to master-install.py"
else:
    print "---> I can't find it, I'd better install it! (from %s)" % package

if ret or installswig:
    # first check if libdir/bin is on the path
    bindir = libdir + '/bin'
    if bindir not in os.getenv('PATH'):
        print "!!! You need to put %s in front of the PATH (use export or setenv)" % bindir
        print "    (sorry, I can't do it from here, it wouldn't stick...)"
        sys.exit(1)
    
    print "--> Installing SWIG"
    os.chdir('Packages')
    assert(0 == os.system('tar zxf %s' % package))
    os.chdir(packagedir)
    assert(0 == os.system('./configure --prefix=%s' % libdir))
    assert(0 == os.system('make'))
    assert(0 == os.system('make install'))
    os.chdir('..')
    assert(0 == os.system('rm -rf %s' % packagedir))
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

os.chdir('MLDCwaveforms/CosmicStringCusp')
print "--> Installing/refreshing CosmicStringCusp"
assert(0 == os.system('python setup.py install --prefix=%s --fftw=%s' % (libdir,fftwdir)))
os.chdir(here)

os.chdir('MLDCwaveforms/FastBBH')
print "--> Installing/refreshing FastBBH"
assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
os.chdir(here)

# install/check install for EMRI waveform

os.chdir('MLDCwaveforms/EMRI')
print "--> Installing/refreshing EMRI"
assert(0 == os.system('python setup.py install --prefix=%s --with-gsl=%s' % (libdir,gsldir)))
os.chdir(here)

os.chdir('MLDCwaveforms/Stochastic')
print "--> Installing/refreshing Stochastic"
assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
os.chdir(here)

os.chdir('MLDCwaveforms/PseudoRandomNoise')
print "--> Installing/refreshing PseudoRandomNoise"
assert(0 == os.system('python setup.py install --prefix=%s' % libdir))
os.chdir(here)


# install/check install for Galaxy_General TDI

os.chdir('MLDCwaveforms/Galaxy_General')
print "--> Installing/refreshing Galaxy_General"

sources = glob.glob('*.c') + glob.glob('*.h') + glob.glob('../../lisaXML/io-C/*.c') + glob.glob('../../lisaXML/io-C/*.h')

if ( newer_group(sources,'Setup') or newer_group(sources,'Fast_Response') or newer_group(sources,'Galaxy_Maker')
     or newer_group(sources,'Galaxy_key') or newer_group(sources,'Fast_XML_LS') 
     or newer_group(sources,'Fast_XML_SL')      ): # or newer_group(sources,'Confusion_Maker')
    print "    (recompiling Galaxy_General)"
    assert(0 == os.system('python Compile --gsl=%s --fftw=%s' % (gsldir,fftwdir)))

assert(0 == os.system('ln -fs ../../Galaxy3/Data/AMCVn_GWR_MLDC.dat Data/.'))
assert(0 == os.system('ln -fs ../../Galaxy3/Data/dwd_GWR_MLDC.dat Data/.'))

os.chdir(here)


# install/check install for Galaxy3 TDI

os.chdir('MLDCwaveforms/Galaxy3')
print "--> Installing/refreshing Galaxy3"

sources = glob.glob('*.c') + glob.glob('*.h') + glob.glob('../../lisaXML/io-C/*.c') + glob.glob('../../lisaXML/io-C/*.h')

if ( newer_group(sources,'Fast_Response3') or newer_group(sources,'Galaxy_Maker3')
     or newer_group(sources,'Galaxy_key3') or newer_group(sources,'Fast_XML_LS3') 
     or newer_group(sources,'Fast_XML_SL3') or newer_group(sources,'Confusion_Maker3')     ):
    print "    (recompiling Galaxy3)"
    assert(0 == os.system('./Compile --gsl=%s --fftw=%s' % (gsldir,fftwdir)))

os.chdir('Data')

if not os.path.isfile('AMCVn_GWR_MLDC.dat') or not os.path.isfile('dwd_GWR_MLDC.dat'):
    if downloadgalaxy == True or downloadgalaxy == 3:
        assert(0 == os.system('curl -O http://www.tapir.caltech.edu/mldc/data/AMCVn_GWR_MLDC.dat.bz2'))
        assert(0 == os.system('bunzip2 AMCVn_GWR_MLDC.dat.bz2'))
        assert(0 == os.system('curl -O http://www.tapir.caltech.edu/mldc/data/dwd_GWR_MLDC.dat.bz2'))
        assert(0 == os.system('bunzip2 dwd_GWR_MLDC.dat.bz2'))
    else:
        print "!!! If you want to generate galactic backgrounds for challenge 3, you'll need to download the catalogs."
        print "    Do so with the option --downloadgalaxy=3 or --downloadgalaxy to get all catalogs."

if not os.path.isfile('AMCVn_GWR_MLDC_bulgefix_opt.dat') or not os.path.isfile('dwd_GWR_MLDC_bulgefix.dat'):
    if downloadgalaxy == True or downloadgalaxy == 4:
        assert(0 == os.system('curl -O http://www.tapir.caltech.edu/mldc/data/AMCVn_GWR_MLDC_bulgefix_opt.dat.bz2'))
        assert(0 == os.system('bunzip2 AMCVn_GWR_MLDC_bulgefix_opt.dat.bz2'))
        assert(0 == os.system('curl -O http://www.tapir.caltech.edu/mldc/data/dwd_GWR_MLDC_bulgefix.dat.bz2'))
        assert(0 == os.system('bunzip2 dwd_GWR_MLDC_bulgefix.dat.bz2'))
    else:
        print "!!! If you want to generate galactic backgrounds for challenge 4, you'll need to download the catalogs."
        print "    Do so with the option --downloadgalaxy=4 or --downloadgalaxy to get all catalogs."

os.chdir(here)

# install/check install for Galaxy TDI

os.chdir('MLDCwaveforms/Galaxy')
print "--> Installing/refreshing Galaxy"

sources = glob.glob('*.c') + glob.glob('*.h') + glob.glob('../../lisaXML/io-C/*.c') + glob.glob('../../lisaXML/io-C/*.h')

if ( newer_group(sources,'Fast_Response') or newer_group(sources,'Galaxy_Maker')
     or newer_group(sources,'Galaxy_key')  or newer_group(sources,'Fast_XML_LS')
     or newer_group(sources,'Fast_XML_SL') or newer_group(sources,'DataImport') ):
    print "    (recompiling Galaxy)"
    assert(0 == os.system('./Compile --gsl=%s --fftw=%s' % (gsldir,fftwdir)))

os.chdir('Data')

if not os.path.isfile('dwd_GWR_all_pars.dat'):
    if downloadgalaxy == True or downloadgalaxy == 2:
        assert(0 == os.system('curl -O http://www.tapir.caltech.edu/mldc/data/dwd_GWR_all_pars.dat.gz'))
        assert(0 == os.system('gunzip dwd_GWR_all_pars.dat.gz'))
    else:
        print "!!! If you want to generate galactic backgrounds for challenge 2, you'll need to download the catalogs."
        print "    Do so with the option --downloadgalaxy=2 or --downloadgalaxy to get all catalogs."
        
os.chdir(here)

# install/check install for synthlisa

print "--> Checking synthLISA"

if checkpackage('synthlisa') < LooseVersion('1.4.2') or newsynthlisa:
    print "--> Installing Synthetic LISA"
    installpackage('http://www.vallis.org/software/synthLISA-1.4.2.tar.gz',prefix=libdir,keepdownload=False)

# install/check install for LISACode

print "--> Checking LISACode"

if ((not os.path.isfile(libdir + '/bin/LISACode')) or newlisacode) and (not nolisacode):
    print "--> Installing LISACode"
    if os.path.isfile('LISACode/Makefile'):
        assert(0 == os.system('make -C LISACode clean'))
    installpackage('LISACode',prefix=libdir,configureflags='CPPFLAGS="-I'+fftwdir+'/include" LDFLAGS="-L'+fftwdir+'/lib" CXXFLAGS="-O3" CFLAGS=""')

print >> open('MLDCpipelines2/bin/lisacode.py','w'), "lisacode = '%s'" % (libdir + '/bin/LISACode')

# install/check install for LISA Simulator

print "--> Checking LISA Simulator"

package, packageversion = findpackage('Simulator')
packagedir = re.sub('.tar.gz','',package)

lisasimtar = 'Packages/' + package
lisasimdir = packagedir

# still will not upgrade automatically, need to save the version number somewhere...
# could compact into a single installation section, iterating over '1year', '2year'

def makelisasim(path,include):
    sharepath = libdir + '/share/' + path
    
    if (not os.path.isdir(sharepath) or newlisasim) and (not nolisasim):
        print "--> Installing LISA Simulator from %s into %s" % (package,path)
        
        if not os.path.isdir(libdir + '/share'):
            os.mkdir(libdir + '/share')
        
        if newlisasim and os.path.isdir(sharepath):
            assert(0 == os.system('rm -rf %s' % sharepath))
        
        # untar
        os.chdir(libdir + '/share')
        assert(0 == os.system('tar zxf %s -C .' % (here + '/' + lisasimtar)))
        assert(0 == os.system('mv %s %s' % (lisasimdir,path)))
        
        # copy modified LISAconstants.h and Compile files
        assert(0 == os.system('cp %s %s/LISAconstants.h' % (here + '/Packages/LISASimulator/' + include,       path)))
        assert(0 == os.system('cp %s %s/.'               % (here + '/Packages/LISASimulator/Compile',          path)))
        
        # patch IO on cygwin
        if 'CYGWIN' in platform.system():
            assert(0 == os.system('patch %s/IO/ezxml.c %s' % (path,here + '/Packages/Patch-cygwin/io-C-ezxml.c')))
        
        # compile and setup
        os.chdir(path)
        assert(0 == os.system('./Compile --gsl=' + gsldir))
        assert(0 == os.system('./Setup'))
        os.chdir('../..')

makelisasim('lisasimulator-1year','LISAconstants-1year.h')
makelisasim('lisasimulator-2year','LISAconstants-2year.h')
# makelisasim('lisasimulator-2year-high','LISAconstants-2year-high.h')
# the high-frequency installation takes 90 mins and 12 GB
# a high frequency signal run takes 55 mins and 5 GB storage

os.chdir(here)

lisasimpy = open('MLDCpipelines2/bin/lisasimulator.py','w')
print >> lisasimpy, "lisasim1yr = '%s'" % (libdir + '/share/lisasimulator-1year')
print >> lisasimpy, "lisasim2yr = '%s'" % (libdir + '/share/lisasimulator-2year')
print >> lisasimpy, "lisasim2yrhigh = '%s'" % (libdir + '/share/lisasimulator-2year-high')
lisasimpy.close()

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
