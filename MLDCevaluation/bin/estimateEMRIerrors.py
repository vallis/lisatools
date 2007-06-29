#!/usr/bin/env python


__version__='$Id:  $'


import lisaxml
from lisaxml.convertunit import convertUnit
import sys, re
import math
import glob

from math import pi, sin, cos, sqrt, fabs

import synthlisa


def FindDeltas(Source, ParamRange, RecSystems, KeySystem):
    spr = "   "
    for emri in RecSystems:
       err = []
       MBH = emri.MassOfSMBH
       doit = 0
       print emri.name
       if (re.search(Source, emri.name) != None):
           err.append((KeySystem.EclipticLatitude - emri.EclipticLatitude)/ParamRange[0])
           delP = (KeySystem.EclipticLongitude - emri.EclipticLongitude)/ParamRange[1]
           delP2 = (KeySystem.EclipticLongitude + 2.*pi - emri.EclipticLongitude)/ParamRange[1]
           delP3 = (KeySystem.EclipticLongitude -2.*pi - emri.EclipticLongitude)/ParamRange[1]
           if (fabs(delP2 ) < fabs(delP)):
                  delP = delP2
           if (fabs(delP3 ) < fabs(delP)):
                  delP = delP3
           err.append(delP)
           err.append((KeySystem.PolarAngleOfSpin - emri.PolarAngleOfSpin)/ParamRange[2])
           delP = (KeySystem.AzimuthalAngleOfSpin - emri.AzimuthalAngleOfSpin)/ParamRange[3]
           delP2 = (KeySystem.AzimuthalAngleOfSpin + 2.*pi - emri.AzimuthalAngleOfSpin)/ParamRange[3]
           delP3 = (KeySystem.AzimuthalAngleOfSpin - 2.*pi - emri.AzimuthalAngleOfSpin)/ParamRange[3]
           if (fabs(delP2 ) < fabs(delP)):
               delP = delP2
           if (fabs(delP3 ) < fabs(delP)):
                      delP = delP3
           err.append(delP)
           err.append((KeySystem.Spin - emri.Spin)/ParamRange[4])
           err.append((KeySystem.MassOfCompactObject - emri.MassOfCompactObject)/ParamRange[5])
           err.append((KeySystem.MassOfSMBH - emri.MassOfSMBH)/ParamRange[6])
           err.append((KeySystem.InitialAzimuthalOrbitalFrequency - emri.InitialAzimuthalOrbitalFrequency)\
                        /KeySystem.InitialAzimuthalOrbitalFrequency)
           delP = (KeySystem.InitialAzimuthalOrbitalPhase - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
           delP2 = (KeySystem.InitialAzimuthalOrbitalPhase + 2.*pi - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
           delP3 = (KeySystem.InitialAzimuthalOrbitalPhase - 2.*pi - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
           if (fabs(delP2 ) < fabs(delP)):
                delP = delP2
           if (fabs(delP3 ) < fabs(delP)):
                delP = delP3
           err.append(delP)
           err.append((KeySystem.InitialEccentricity - emri.InitialEccentricity)/ParamRange[9])
           delP = (KeySystem.InitialTildeGamma - emri.InitialTildeGamma)/ParamRange[10]
           delP2 = (KeySystem.InitialTildeGamma + 2.*pi - emri.InitialTildeGamma)/ParamRange[10]
           delP3 = (KeySystem.InitialTildeGamma - 2.*pi - emri.InitialTildeGamma)/ParamRange[10]
           if (fabs(delP2 ) < fabs(delP)):
                delP = delP2
           if (fabs(delP3 ) < fabs(delP)):
                delP = delP3
           err.append(delP)
           delP = (KeySystem.InitialAlphaAngle - emri.InitialAlphaAngle)/ParamRange[11]
           delP2 = (KeySystem.InitialAlphaAngle + 2.*pi - emri.InitialAlphaAngle)/ParamRange[11]
           delP3 = (KeySystem.InitialAlphaAngle -2*pi - emri.InitialAlphaAngle)/ParamRange[11]
           if (fabs(delP2 ) < fabs(delP)):
                delP = delP2
           if (fabs(delP3 ) < fabs(delP)):
                delP = delP3
           err.append(delP)
           err.append((KeySystem.LambdaAngle - emri.LambdaAngle)/ParamRange[12])
           err.append((KeySystem.Distance - emri.Distance)/KeySystem.Distance)
           rec = " "
           for i in xrange(len(err)):
               rec += str(err[i]) + spr + '&' + spr
           print rec+"\n"
            
    return  err

    

      
           

KeysChal1_3 = ["Key/challenge1.3.1-key.xml", "Key/challenge1.3.2-key.xml", "Key/challenge1.3.3-key.xml",\
"Key/challenge1.3.4-key.xml"]

ParamRange = []
ParamRange.append(pi) # 0-latitude
ParamRange.append(2.*pi)  # 1-longitude
ParamRange.append(pi)  # 2-PolarAngleOfSpin
ParamRange.append(2.*pi)  # 3-AzimuthalAngleOfSpin
ParamRange.append(0.2)  # 4-Spin
ParamRange.append(1.0)  # 5-MassOfCompactObject
ParamRange.append(1.e6)  # 6-MassOfSMBH (for 1.3.1) should be 5.e5 for 132,3 and 1.e5 for 134,5
ParamRange.append("NA")  # 7-InitialAzimuthalOrbitalFrequency
ParamRange.append(2.*pi)  # 8-InitialAzimuthalOrbitalPhase
ParamRange.append(0.15)  # 9- InitialEccentricity (guess)
ParamRange.append(2.*pi)  # 10-InitialTildeGamma
ParamRange.append(2.*pi)  # 11-InitialAlphaAngle
ParamRange.append(pi)  # 12-LambdaAngle
ParamRange.append("NA")  # 13-Distance
ParamRange.append(5.e7) # 14 to be filled up with Plunge time

print ParamRange

sources = "Source/Challenge1.3/*xml"



for xmlKey in KeysChal1_3:
    inputXML = lisaxml.readXML(xmlKey)
    KeySystem = inputXML.getLISASources()[0]
    print "Dealing with ", xmlKey
    for xmlfile in glob.glob(sources):
        SourceXML = lisaxml.readXML(xmlfile)
        RecSystems = SourceXML.getLISASources()
        print "processing file", xmlfile
        if (re.search('1.3.1', xmlKey) != None):
            Errors  = FindDeltas("EMRI-1", ParamRange, RecSystems, KeySystem)
        elif (re.search('1.3.2', xmlKey) != None):
            ParamRange[6] = 5.e5
            Errors  = FindDeltas("EMRI-2", ParamRange, RecSystems, KeySystem)
        elif (re.search('1.3.3', xmlKey) != None):
            ParamRange[6] = 5.e5
            Errors  = FindDeltas("EMRI-3", ParamRange, RecSystems, KeySystem)
        elif (re.search('1.3.4', xmlKey) != None):
            ParamRange[6] = 1.e5
            Errors  = FindDeltas("EMRI-4", ParamRange, RecSystems, KeySystem)
        SourceXML.close()
    inputXML.close()    

