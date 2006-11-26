#!/usr/bin/env python

from distutils.core import setup, Extension

# now run the setup

setup(name = 'MLDC',
      version = '$Id: $',
      description = 'The MLDC waveforms package',
      author = 'M. Vallisneri & S. Babak',
      author_email = 'vallis@vallis.org',
      url = 'http://svn.sourceforge.net/viewvc/lisatools',
      packages = ['MLDC']
      )

# then we should list all plugins, and run their setups, too
# passing the same arguments we got (perhaps the entire argv)
