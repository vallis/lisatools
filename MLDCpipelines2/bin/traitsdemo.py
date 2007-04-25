#!/usr/bin/env python

import sys
import lisaxml

infile  = sys.argv[1]
outfile = sys.argv[2]

inputXML = lisaxml.readXML(infile)
outputXML = lisaxml.lisaXML(outfile)

allsources = inputXML.getLISASources()

for source in allsources:
    newsource = lisaxml.InteractiveMakeSource(source)
    outputXML.SourceData(newsource)
