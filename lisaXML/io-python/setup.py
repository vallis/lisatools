#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.sysconfig import get_python_lib

import sys

# get the install path so we know where to put test.xml

install_prefix = ''

for arg in sys.argv:
    if arg.startswith('--prefix='):
        install_prefix = arg.split('=', 1)[1]

install_path = get_python_lib(prefix=install_prefix)

# now run the setup

setup(name = 'lisaxml',
      version = '$Id$',
      description = 'The lisaXML Python package',
      author = 'M. Vallisneri',
      author_email = 'vallis@vallis.org',
      url = 'http://svn.sourceforge.net/viewvc/lisatools',
      packages = ['lisaxml'],
      data_files = [(install_path + '/lisaxml', ['test.xml'])]
      )
