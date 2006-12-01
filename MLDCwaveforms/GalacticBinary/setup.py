#!/usr/bin/env python

modulename  = 'GalacticBinary'
version     = '$Id: $'
description = 'A MLDC plugin to create GalacticBinary waveforms'
author      = 'Michele Vallisneri'
email       = 'vallis@vallis.org'
url         = 'http://svn.sourceforge.net/viewvc/lisatools'

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
