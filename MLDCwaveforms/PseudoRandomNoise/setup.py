#!/usr/bin/env python

modulename  = 'PseudoRandomNoise'
version     = '$Id: $'
description = 'A MLDC plugin to access SynthLISA pseudorandom instrument noise'
author      = 'Michele Vallisneri'
email       = 'vallis@vallis.org'
url         = 'http://lisatools.googlecode.com'

# please don't change anything below without contacting vallis@vallis.org

from distutils.core import setup

setup(name = modulename,
      version = version,
      description = description,
      author = author,
      author_email = email,
      url = url,

      py_modules = [modulename]
      )
