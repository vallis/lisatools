#!/usr/bin/env python

modulename  = 'Stochastic'
version     = '$Id: $'
description = 'A MLDC plugin to create a Stochastic (pseudorandom) waveform'
author      = 'Michele Vallisneri'
email       = 'vallis@vallis.org'
url         = 'http://lisatools.googlecode.com'

# this is a good template for Python-only MLDC plugins...
# it's quite minimal...

# please don't change anything below without contacting vallis@vallis.org

# now run the setup

from distutils.core import setup

setup(name = modulename,
      version = version,
      description = description,
      author = author,
      author_email = email,
      url = url,

      py_modules = [modulename]
      )
