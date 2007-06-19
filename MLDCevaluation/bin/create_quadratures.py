#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit
import sys, re
import math

import synthlisa

if (len(sys.argv) < 2):
    print "missing name of xml file: usage create_quadratures.py XMLFILE"
    sys.exit(1)

xmlInput = sys.argv[1]
outputname1 = xmlInput[:-4]+"_0.xml" 

inputXML = lisaxml.readXML(xmlInput)
mysystems = inputXML.getLISASources()
inputXML.close()

outputXML1 = lisaxml.lisaXML(outputname1,author='Stas Babak')


Zerosystems = mysystems

for system in Zerosystems:
  system.InitialAngularOrbitalPhase = 0.0
  outputXML1.SourceData(system)

outputXML1.close()


