#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name = 'BBH',
      version = '$Id$',
#     description = '',
      author = 'Stas Babak',
      author_email = 'stba@aei.mpg.de>',
#     url = '',
      
      py_modules = ['BBH'],
      
      ext_modules = [Extension('_BBH',
                               ['BBHChallenge1.cc', 'Constants.cc', 'BBH.i'],
                               depends = ['BBHChallenge1.hh', 'Constants.hh', 'Macros.hh'])]
      )

# I'm specifying that we want SWIG to create a CPP extension in setup.cfg...
# how do I set which 