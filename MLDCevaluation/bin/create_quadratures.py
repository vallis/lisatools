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

inputXML = lisaxml.readXML(xmlInput)
mysystem = inputXML.getLISASources()[0]
inputXML.close()

Zerosystem = mysystem

if (  re.search('Challenge1.2', xmlInput) != None):
    print "creating quadratures for BBH waveform " 
    Zerosystem.InitialAngularOrbitalPhase = 0.0
elif(  re.search('Challenge1.1.1', xmlInput) != None):
    Zerosystem.InitialPhase = 0.0
else:
   print "unknown challenge name", challengename
   sys.exit(1)


outputname1 = xmlInput[:-4]+"_0.xml" 
 
# write out the XML file


outputXML1 = lisaxml.lisaXML(outputname1,author='Stas Babak')
outputXML1.SourceData(Zerosystem)
outputXML1.close()

# the same for pi/2

Piby2system = mysystem
if (  re.search('Challenge1.2', xmlInput) != None):
    Piby2system.InitialAngularOrbitalPhase = math.pi/2.0
elif(  re.search('Challenge1.1.1', xmlInput) != None):
    Piby2system.InitialPhase = math.pi/2.0
else:
   print "unknown challenge name", challengename
   sys.exit(1)

outputname2 = xmlInput[:-4]+"_piby2.xml"
outputXML2 = lisaxml.lisaXML(outputname2,author='Stas Babak')
outputXML2.SourceData(Piby2system)
outputXML2.close()
