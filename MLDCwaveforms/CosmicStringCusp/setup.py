#!/usr/bin/env python

modulename  = 'CosmicStringCusp'
version     = '$Id: $'
description = 'A MLDC plugin to create cosmic string cusp bursts'
author      = 'Neil Cornish'
email       = 'cornish@physics.montana.edu'
url         = 'http://lisatools.googlecode.com'

csourcefiles = ['Cusp.c']
sourcefiles  = csourcefiles + ['CosmicStringCusp.i']
headers      = ['Constants.h', 'Declarations.h']

# please don't change anything below without contacting vallis@vallis.org

# note: setup.cfg specifies that we want SWIG to create a C extension

import os
import sys

from distutils.core import setup, Extension
from distutils.command.build import build
from distutils.command.install_lib import install_lib
from distutils.spawn import spawn
from distutils.file_util import copy_file
from distutils.util import get_platform

# process arguments...

argv_replace = []
make_clib = False
fftwdir = None
prefix = None

for arg in sys.argv:
    if arg == '--make-clib':
        make_clib = True
    elif arg.startswith('--fftw='):
        fftwdir = arg.split('=',1)[1]
    elif arg.startswith('--prefix='):
        prefix = arg.split('=',1)[1]
        argv_replace.append(arg)
    else:
        argv_replace.append(arg)

sys.argv = argv_replace

# build the Python modules last so that the include the SWIG interface

class swig_build(build):
    # we assume build_py is the first in the sequence
    sub_commands = build.sub_commands[1:] + [build.sub_commands[0]]

# build also a C library

class qm_install_lib(install_lib):
    def run(self):
        install_lib.run(self)
        
        if self.distribution.has_c_libraries():
            build_clib = self.get_finalized_command('build_clib')
            libs = build_clib.get_library_names()
            
            clib_dir = build_clib.build_clib
            
            for lib in libs:
                clib = 'lib' + lib + '.a'
                
                src_file = os.path.join(clib_dir, clib)
                dest_file = os.path.join(self.install_dir, clib)
                
                copy_file(src_file, dest_file)
                
                if sys.platform[:6] == "darwin":
                    spawn(['ranlib'] + [dest_file])
    

# get the numpy installation dir so we know where to find the header
# (an interesting approach)

includedirs, librarydirs, runtimelibrarydirs = [], [], []

from numpy import __path__ as numpypath
includedirs.append(numpypath[0] + '/core/include')

if fftwdir == None and prefix != None and os.path.isfile(prefix + '/include/fftw3.h'):
    fftwdir = prefix

if fftwdir:
    includedirs.append(fftwdir + '/include')
    librarydirs.append(fftwdir + '/lib')
    runtimelibrarydirs.append(fftwdir + '/lib')

# now run the setup

if make_clib == True:
    clibrary = [(modulename,{'sources': csourcefiles,
                             'depends': headers,
                             'include_dirs': includedirs,
                             'library_dirs': librarydirs,
                             'libraries': ['lfftw3'],
                             'runtime_library_dirs': runtimelibrarydirs})]

    # this hack needed on OS X for universal binaries since ar fails if the .a is already present...

    try:
        os.remove('build/temp.' + get_platform() + '-%s.%s' % sys.version_info[0:2] + '/lib' + modulename + '.a')
    except:
        pass
else:
    clibrary = []

setup(name = modulename,
      version = version,
      description = description,
      author = author,
      author_email = email,
      url = url,

      py_modules = [modulename],
      
      ext_modules = [Extension('_' + modulename,
                               sourcefiles,
                               include_dirs = includedirs,
                               library_dirs = librarydirs,
                               runtime_library_dirs = runtimelibrarydirs,
                               libraries = ['fftw3'],
                               depends = headers)],

      libraries = clibrary,      
                               
      cmdclass = {'build': swig_build, 'install_lib' : qm_install_lib}
      )
