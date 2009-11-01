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
       Source = "ExtremeMassRatioInspiral"
#       print emri.name
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

from optparse import OptionParser
    
parser = OptionParser(usage="usage: %prog [options] KeyFile.xml source_name ResultFile.xml source_name OutputFile", version="$Id: $")


parser.add_option("-e", "--deltae",
                      type="float", dest="de", default=0.15,
                      help="range of e [default 0.15]")

(options, args) = parser.parse_args()

if len(args) < 5:
    parser.error("not enough parameters, run with --help option ")

keyFile = args[0]
keyS = args[1]
datFile = args[2]
datS = args[3]
outF = args[4]


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
ParamRange.append(options.de)  # 9- InitialEccentricity (guess)
ParamRange.append(2.*pi)  # 10-InitialTildeGamma
ParamRange.append(2.*pi)  # 11-InitialAlphaAngle
ParamRange.append(pi)  # 12-LambdaAngle
ParamRange.append("NA")  # 13-Distance
ParamRange.append(5.e7) # 14 to be filled up with Plunge time

print ParamRange

print "Dealing with ", keyFile
inputXML = lisaxml.readXML(keyFile)
KeySystems = inputXML.getLISASources()

   
SourceXML = lisaxml.readXML(datFile)
RecSystems = SourceXML.getLISASources()

fout = open(outF, 'w')

spr = "    "

for key in KeySystems:
    #print "Keys: ", key.name
    if (key.name == keyS):
        for emri in RecSystems:
           err = []
	   keyPar = []
	   #print datS,"  -   ", emri.name
	   rec = " \n"
	   if (emri.name == datS):
              print "emri : ", emri.name
              rec = rec + "EclipticLatitude:    "
	      rec = rec + str((key.EclipticLatitude - emri.EclipticLatitude)/ParamRange[0]) + \
                        spr + str((key.EclipticLatitude - emri.EclipticLatitude)/key.EclipticLatitude) + "\n"
	      delP = (key.EclipticLongitude - emri.EclipticLongitude)/ParamRange[1]
              delP2 = (key.EclipticLongitude + 2.*pi - emri.EclipticLongitude)/ParamRange[1]
              delP3 = (key.EclipticLongitude -2.*pi - emri.EclipticLongitude)/ParamRange[1]
              if (fabs(delP2 ) < fabs(delP)):
                  delP = delP2
              if (fabs(delP3 ) < fabs(delP)):
                  delP = delP3
	      rec = rec + "EclipticLongitude:   " 
	      rec = rec + str(delP) + spr + str(delP*ParamRange[1]/key.EclipticLongitude) + "\n"
              SkyPolarAngleEMRI = pi/2.-emri.EclipticLatitude
              SkyPolarAngleKey = pi/2.-key.EclipticLatitude
	      print "EcLatKey, EcLongKey:", key.EclipticLatitude, key.EclipticLongitude
              print "EcLatEMRI, EcLongEMRI:", emri.EclipticLatitude, emri.EclipticLongitude
              CosSkyAngle = (cos(SkyPolarAngleEMRI)*cos(SkyPolarAngleKey) \
                                        * cos(emri.EclipticLongitude-key.EclipticLongitude) + \
              			sin(SkyPolarAngleEMRI)*sin(SkyPolarAngleKey))
	      SkyAngleTable=math.acos(CosSkyAngle)*(180.0/pi)
	      print "CosSkyAngle, SkyAngle (deg):", CosSkyAngle, SkyAngleTable
              delP = (key.PolarAngleOfSpin - emri.PolarAngleOfSpin)
	      rec = rec + "PolarAngleOfSpin:    "
	      rec = rec + str(delP/ParamRange[2]) + spr + str(delP/key.PolarAngleOfSpin) + "\n"
              delP = (key.AzimuthalAngleOfSpin - emri.AzimuthalAngleOfSpin)/ParamRange[3]
              delP2 = (key.AzimuthalAngleOfSpin + 2.*pi - emri.AzimuthalAngleOfSpin)/ParamRange[3]
              delP3 = (key.AzimuthalAngleOfSpin - 2.*pi - emri.AzimuthalAngleOfSpin)/ParamRange[3]
              if (fabs(delP2 ) < fabs(delP)):
                 delP = delP2
              if (fabs(delP3 ) < fabs(delP)):
                 delP = delP3
	      rec = rec + "AzimuthalAngleOfSpin:   "
	      rec = rec + str(delP) + spr + str(delP*ParamRange[3]/key.AzimuthalAngleOfSpin) + "\n"
              SpinAngleTable = math.acos (cos(emri.PolarAngleOfSpin)*cos(key.PolarAngleOfSpin) \
						*cos(emri.AzimuthalAngleOfSpin-key.AzimuthalAngleOfSpin) + \
					sin(emri.PolarAngleOfSpin)*sin(key.PolarAngleOfSpin)) * (180.0/pi)
              delP = (key.Spin - emri.Spin)
	      rec = rec + "Spin:     "
	      rec = rec + str(delP/ParamRange[4]) + spr + str(delP/key.Spin) + "\n"
 	      SpinTable = delP * 1000.0	
              delP = key.MassOfCompactObject - emri.MassOfCompactObject
	      rec = rec + "MassOfCompactObject:     "
	      rec = rec + str(delP/ParamRange[5]) + spr + str(delP/key.MassOfCompactObject) + "\n"
              MuTable = delP/key.MassOfCompactObject*1000.0
              delP = key.MassOfSMBH - emri.MassOfSMBH
	      rec = rec + "MassOfSMBH:    "
	      rec = rec + str(delP/ParamRange[6]) + spr + str(delP/key.MassOfSMBH) + "\n"
	      MTable = delP/key.MassOfSMBH*1000.0
	      rec  = rec + "InitialAzimuthalOrbitalFrequency:     "
              rec = rec + str((key.InitialAzimuthalOrbitalFrequency - emri.InitialAzimuthalOrbitalFrequency)\
                        /key.InitialAzimuthalOrbitalFrequency) + "\n"
              NuTable = (key.InitialAzimuthalOrbitalFrequency - emri.InitialAzimuthalOrbitalFrequency)\
                        /key.InitialAzimuthalOrbitalFrequency*100000.0
              delP = (key.InitialAzimuthalOrbitalPhase - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
              delP2 = (key.InitialAzimuthalOrbitalPhase + 2.*pi - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
              delP3 = (key.InitialAzimuthalOrbitalPhase - 2.*pi - emri.InitialAzimuthalOrbitalPhase)/ParamRange[8]
              if (fabs(delP2 ) < fabs(delP)):
                  delP = delP2
              if (fabs(delP3 ) < fabs(delP)):
                  delP = delP3
	      rec = rec + "InitialAzimuthalOrbitalPhase:    "
	      rec = rec + str(delP) + spr + str(delP*ParamRange[8]/key.InitialAzimuthalOrbitalPhase) + "\n"
              delP = key.InitialEccentricity - emri.InitialEccentricity
	      rec = rec + "InitialEccentricity:     " 
	      rec = rec + str(delP/ParamRange[9]) + spr +  str(delP/key.InitialEccentricity) + "\n"
	      EccTable=delP*1000.0
              delP = (key.InitialTildeGamma - emri.InitialTildeGamma)/ParamRange[10]
              delP2 = (key.InitialTildeGamma + 2.*pi - emri.InitialTildeGamma)/ParamRange[10]
              delP3 = (key.InitialTildeGamma - 2.*pi - emri.InitialTildeGamma)/ParamRange[10]
              if (fabs(delP2 ) < fabs(delP)):
                 delP = delP2
              if (fabs(delP3 ) < fabs(delP)):
                 delP = delP3
	      rec = rec + "InitialTildeGamma:      "
	      rec =  rec + str(delP) +  spr + str(delP*ParamRange[10]/key.InitialTildeGamma) + "\n"
              delP = (key.InitialAlphaAngle - emri.InitialAlphaAngle)/ParamRange[11]
              delP2 = (key.InitialAlphaAngle + 2.*pi - emri.InitialAlphaAngle)/ParamRange[11]
              delP3 = (key.InitialAlphaAngle -2*pi - emri.InitialAlphaAngle)/ParamRange[11]
              if (fabs(delP2 ) < fabs(delP)):
                  delP = delP2
              if (fabs(delP3 ) < fabs(delP)):
                  delP = delP3
	      rec =  rec + "InitialAlphaAngle:    "
	      rec = rec + str(delP) + spr + str(delP*ParamRange[11]/key.InitialAlphaAngle) + "\n"
              delP = key.LambdaAngle - emri.LambdaAngle
	      rec = rec + "LambdaAngle:      " 
	      rec = rec + str(delP/ParamRange[12]) + spr + str(delP/key.LambdaAngle) + "\n"
              LambdaTable = delP/key.LambdaAngle
	      rec = rec + "Distance:    "
	      rec = rec + str((key.Distance - emri.Distance)/key.Distance) + "\n"
              DistTable = (key.Distance - emri.Distance)/key.Distance 
	      fout.write(rec + "\n")	

print "Group & SNR & $\\frac{\\Delta M}{M}$ & $\\frac{\\Delta \\mu}{\\mu}$ & $ \\frac{\\Delta \\nu_0}{\\nu_0} $" + \
	" &  $\\Delta  e_0$ &  $\\Delta |S|$ & " + \
	"$\\frac{\\Delta \\lambda_{\\rm SL}}{\\lambda_{\\rm SL}}$ & $\\Delta{\\rm Spin}$ & " + \
	"$\\Delta{\\rm Sky}$ & $\\frac{\\Delta D}{D}$ \\\\" + "\n" 
print "& & $\\times 10^{-3}$ & $\\times 10^{-3}$ & $\\times 10^{-5}$ & $\\times 10^{-3}$ &" + \
	" $\\times 10^{-3}$ & & (deg) & (deg) &   \\\\" + "\n"
print "?", " & ", "?", " & ", MTable, " & ",  MuTable, " & ", NuTable, " & ", \
	EccTable, " & ", SpinTable, " & ", LambdaTable, " & ", SpinAngleTable, " & ", \
	SkyAngleTable, " & ", DistTable, " \\\\" + "\n"
fout.close()	      



    #MBH = emri.MassOfSMBH
    #Source = "ExtremeMassRatioInspiral"
	    
#	    Errors  = FindDeltas("EMRI-1", ParamRange, RecSystems, KeySystem)
#            print Errors
#            print ParamRange
#            print xmlfile
#            print xmlKey
#            sys.exit(0)
"""        elif (re.search('3.2', xmlKey) != None and re.search('3.2', xmlfile) != None):
            print "processing file", xmlfile
            ParamRange[6] = 5.e5
            Errors  = FindDeltas("EMRI-2", ParamRange, RecSystems, KeySystem)
        elif (re.search('3.3', xmlKey) != None and re.search('3.3', xmlfile) != None):
            print "processing file", xmlfile
            ParamRange[6] = 5.e5
            Errors  = FindDeltas("EMRI-3", ParamRange, RecSystems, KeySystem)
        elif (re.search('3.4', xmlKey) != None and re.search('3.4', xmlfile) != None):
            print "processing file", xmlfile
            ParamRange[6] = 1.e5
            Errors  = FindDeltas("EMRI-4", ParamRange, RecSystems, KeySystem)
        elif (re.search('3.5', xmlKey) != None and re.search('3.5', xmlfile) != None):
            print "processing file", xmlfile
            ParamRange[6] = 1.e5
            Errors  = FindDeltas("EMRI-5", ParamRange, RecSystems, KeySystem)
        SourceXML.close()
#        print "Errors"
#        print Errors
    inputXML.close()    
"""
