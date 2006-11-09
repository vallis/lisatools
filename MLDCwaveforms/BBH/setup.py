#!/usr/bin/env python

modulename  = 'BBH'
version     = '$Id: setup.py 91 2006-10-10 21:01:15Z vallisneri $'
description = 'A MLDC plugin to create BBH PN waveforms'
author      = 'Stas Babak'
email       = 'stba@aei.mpg.de'
url         = 'http://svn.sourceforge.net/viewvc/lisatools'

sourcefiles = ['BBHChallenge1.cc', 'Constants.cc', 'BBH.i']
headers     = ['BBHChallenge1.hh', 'Constants.hh', 'Macros.hh']

# please don't change anything below without contacting vallis@vallis.org

# note: setup.cfg specifies that we want SWIG to create a C++ extension

from distutils.core import setup, Extension
from distutils.command.build import build

# build the Python modules last so that the include the SWIG interface

class swig_build(build):
    # we assume build_py is the first in the sequence
    sub_commands = build.sub_commands[1:] + [build.sub_commands[0]]

# get the numpy installation dir so we know where to find the header
# (an interesting approach)

from numpy import __path__ as numpypath
numpyinclude = numpypath[0] + '/core/include'

# now run the setup

setup(name = modulename,
      version = version,
      description = description,
      author = author,
      author_email = email,
      url = url,

      py_modules = [modulename],
      
      ext_modules = [Extension('_' + modulename,
                               sourcefiles,
                               include_dirs = [numpyinclude],
                               depends = headers)],
                               
      cmdclass = {'build': swig_build}
      )
