#!/usr/bin/env python

import sys
import os

here   = os.path.dirname(os.path.abspath(sys.argv[0]))
script = os.path.abspath(here + '/../../lisaXML/XSL/xml2txt-source.xsl')

for arg in sys.argv[1:]:
    os.system('xsltproc %s %s' % (script,arg))
