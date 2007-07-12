#!/usr/bin/env python

from synthlisa import *
from Numeric import *

import math
import sys

if len(sys.argv) < 2:
    print "Usage: %s <challenge-name> <xml source file> [<xml sourcefile> ...]" % sys.argv[0]

allsources = []

for file in sys.argv[2:]:
    print "Parsing source XML file %s" % file
    
    inputXML = readXML(file)
    sources = inputXML.getLISASources()
    inputXML.close()

    for source in sources:
        print "...parsing source %s" % source.name

        allsources.append(source)

print "Writing key to file %s" % sys.argv[1] + '-sourcekey.xml'

publicXML = lisaXML(sys.argv[1] + '-sourcekey.xml',author='Michele Vallisneri',
                                                   comments='Public lisaXML key for challenge '+sys.argv[1])

for source in allsources:
    publicXML.SourceData(source,name=source.name)

publicXML.close()
