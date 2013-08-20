#!/usr/bin/env python

__version__='$Id: $'

import sys
import os
import glob
import re
import time
from datetime import date
import math

#import lisaxml
#import xmlLCTools
#import xml.dom.minidom as xmld
import xml.etree.ElementTree as ET
import numpy as np

#import lisacode


################ Globales variables ################

CfgDir = "/home/stas/lisatools/LISACode/Cfg25/"
BinDir = "/home/stas/lisatools/LISACode/bin25/"
# For testing
#CfgDir="/Users/petiteau/Applications/src/LISACode/LISACode_2_0/LISACode2/ConfigFiles/"
#BinDir="/Users/petiteau/Applications/src/LISACode/LISACode_2_0/LISACode2/Main/Exe/"



############### Functions ###############

def makefromtemplate(output,template,**kwargs):
    fi = open(template,'r')
    fo = open(output,'w')
	
			
    for line in fi:
        repline = line
        
        for kw in kwargs:
            repline = re.sub('{' + kw + '}',str(kwargs[kw]),repline)
        
        print >> fo, repline,


def run(command, disp=False, NoExit=False):
    commandline = command % globals()
    if disp : 
        print "----> %s" % commandline
    
    try:
        assert(os.system(commandline) == 0)
    except:
        print 'Script %s failed at command "%s".' % (sys.argv[0],commandline)
        if not NoExit :
            sys.exit(1)



######### XML tools

#### Add tree file 
def AddRootTree(newR, refR) :
    # 2 arguments :
    #  - newR : the new root that we have to add
    #  - refR : the root in reference tree where we want to add the new root

   
    foundR = False
    for child in refR :
        if child.tag == newR.tag and child.get('Type') == newR.get('Type') and child.get('Name') == newR.get('Name') :
            foundR = True
            Elmt = child
    if not foundR :
        Elmt = ET.SubElement(refR,newR.tag)
    for attName in newR.keys() :
        Elmt.set(attName,newR.get(attName))
    if foundR and Elmt.tag=="Comment" :
        Elmt.text = Elmt.text + newR.text
    else :
        Elmt.text = newR.text
    #print newR, refR, foundR,  Elmt.attrib, newR.getchildren()
    for child in newR :
        AddRootTree(child,Elmt)


def dispTree(root,indent):
    indent2=indent+"\t"
    print indent,root, root.attrib
    for elmt in root :
        dispTree(elmt,indent2)
        


############### Main program ###############

from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] [INPUTGW1-bary.xml INPUTGW2-bary.xml ...] [INPUTSPECIFCNOISE1.xml ...] OutputBaseName",
                      version="02.07.13, Antoine Petiteau partially based M. Vallisneri development for lisatools ")

# note that the default value is handled below

##### Options for base configuration #####
parser.add_option("-s", "--seed",
                  type="int", dest="seed", default=12345,
                  help="seed for random number generator (int) [required]")

parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="display parameter values [off by default] : Not use by LISACode at the moment")

parser.add_option("-O", "--OutDataType",
                  type="string", dest="OutDataType", default="ASCII",
                  help="Type of output : ASCII,Binary,XML [default ASCII]")

#### Options fixing xml files to used ####

parser.add_option("-I", "--Instrument",
                  type="string", dest="Instrument", default="ELISA",
                  help="Type of instrument : ELISA, LISA [default ELISA]")

parser.add_option("-N", "--NoNoise",
                  action="store_true", dest="NoNoise", default=False,
                  help="No noise [off by default : noise inlcuded]")
				  
parser.add_option("-P", "--phasemeterFilter",
                  action="store_true", dest="PhaMetFilter", default=False,
                  help="Use standard filter on phasemeter (this option is always true if FullSim) [off by default]")

parser.add_option("-L", "--LaserNoise",
                  action="store_true", dest="LaserNoise", default=False,
                  help="Include laser noise (this option is always true if FullSim or not NoNoise ) [off by default]")

parser.add_option("-F", "--FullSim",
                  action="store_true", dest="FullSim", default=False,
                  help="Full simulation [off by default]")

parser.add_option("-r", "--rawMeasurements",
                  action="store_true", dest="rawMeasurements", default=False,
                  help="output raw phase measurements (y's and z's) in addition to TDI X, Y, Z")

parser.add_option("-D", "--TDI",
                  type="string", dest="TDI", default="XYZ",
                  help="Type of instrument : X, XYZ or XAE (note : for eLISA it will always be forced at X) [default XYZ]")


parser.add_option("-T", "--duration",
                  type="float", dest="duration", default=62914560.0,
                  help="total time for TDI observables (s) [default 62914560 = 2^22 * 15]")

parser.add_option("-d", "--timeStep",
                  type="float", dest="timeStep", default=15.0,
                  help="time step for measurement sampling (s) [default 15]")

parser.add_option("-p", "--timeStepPhysical",
                  type="float", dest="timeStepPhys", default=15.0,
                  help="time step for physical simulation sampling (s) [default equal to timestep]")

parser.add_option("-U", "--FixShotNoise",
                  action="store_true", dest="FixShotNoise", default=False,
                  help="Fix the shot noise ; if false the shot noise vary with the orbit. [default false]")

"""
parser.add_option("-t", "--initialTime",
                  type="float", dest="inittime", default=0.0,
                  help="initial time for TDI observables (s) [default 0.0]")

parser.add_option("-L", "--OrbitsModel",
                  type="string", dest="OrbitsModel", default='Eccentric',
                  help="LISA model: Static, Rigid, Eccentric [default]")

parser.add_option("-O", "--observables",
                  type="string", dest="observables", default='1.5',
                  help="TDI observables: 1.5 [default], 2.0, Sagnac (experimental; not compatible with -r) : Not use by LISACode at the moment")

parser.add_option("-A", "--keyOmitsLISA",
                  action="store_true", dest="keyOmitsLISA", default=False,
                  help="do not include LISA specification in key [included by default]")

parser.add_option("-n", "--noiseOnly",
                  action="store_true", dest="noiseOnly", default=False,
                  help="compute SNR using instrumental noise only [off by default] : Not use by LISACode at the moment")

parser.add_option("-D", "--debugSNR",
                  action="store_true", dest="debugSNR", default=False,
                  help="show more SNR data [off by default] : Not use by LISACode at the moment")

parser.add_option("-c", "--combinedSNR",
                  action="store_true", dest="combinedSNR", default=False,
                  help="use combined snr = sqrt(2)*max{SNR_x, SNR_y, SNR_z} as SNR constrain [off by default] : Not use by LISACode at the moment")

"""

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("You must specify the output file (and input file before if you don't want the default noise only simulation) !")

IdTmpRun=str(options.seed)+""+str(int(100000*np.random.random()))


##### Some checking
if options.OutDataType=="ASCII" :
    extentionOut=".txt"
elif options.OutDataType=="Binary" or options.OutDataType=="XML" : 
    extentionOut=".bin"
else :
    print options.OutDataType, " : unknown type of data (only ASCII, Binary and XML)"
    sys.exit(1)



PhaMetFilter = options.PhaMetFilter
LaserNoise = options.LaserNoise
if not options.NoNoise:
    PhaMetFilter = True
if options.FullSim :
    PhaMetFilter = True
    LaserNoise   = True


##### Input and output files
InputFileName = args[0:-1]
OutputBaseName = args[-1]

##### Make the list of default xml file based on options
InXML=[]
Nlinks=""
if options.Instrument=="ELISA" :
    Nlinks="2links"
    PhaMetmesSC="sci1,sci1s,sci2s,sci3"
    ### Orbits, telescope, laser
    InXML.append(CfgDir+'/ConfigNewLISA/ELISA/Config-Orbits_1Gm.xml')
    InXML.append(CfgDir+'/ConfigNewLISA/ELISA/Config-TelescLaser_P07_D25.xml')
    ### Noises
    if not options.NoNoise :
        InXML.append(CfgDir+'/ConfigNewLISA/ELISA/Noises/Config-NoiseAcc_ELISA.xml')
        InXML.append(CfgDir+'/ConfigNewLISA/ELISA/Noises/Config-NoiseOOPN_ELISA.xml')
        InXML.append(CfgDir+'/ConfigNewLISA/ELISA/Noises/Config-NoiseShot_ELISA.xml')


elif options.Instrument=="LISA" :
    Nlinks="3links"
    PhaMetmesSC="sci1,sci1s,sci2,sci2s,sci3,sci3s"
    InXML.append(CfgDir+'/ConfigNewLISA/LISA/Config-Orbits_std.xml')
    InXML.append(CfgDir+'/ConfigNewLISA/LISA/Config-TelescLaser_std.xml')
    if not options.NoNoise :
        InXML.append(CfgDir+'/ConfigNewLISA/LISA/Noises/Config-NoiseAcc_std2.xml')
        InXML.append(CfgDir+'/ConfigNewLISA/LISA/Noises/Config-NoiseOOPN_std.xml')
        InXML.append(CfgDir+'/ConfigNewLISA/LISA/Noises/Config-NoiseShot_std.xml')
else:
    print "ERROR : The instrument ",options.Instrument," is unknown (only ELISA and LISA are already implemented)."

#### Optical benches
if PhaMetFilter :
    if LaserNoise :
        InXML.append(CfgDir+'/pyTemplates/Std2002/LC2-'+Nlinks+'_PhaMet-sci-Filter.xml')
        InXML.append(CfgDir+'/pyTemplates/Std2002/LC2-'+Nlinks+'_PhaMet-tau-Filter.xml')
        InXML.append(CfgDir+'/pyTemplates/Std2002/LC2-'+Nlinks+'_PhaMet-TDIInt.xml')
        PhaMetmesSC=PhaMetmesSC+",tau1,tau1s,tau2,tau2s,tau3,tau3s"
    else:
        InXML.append(CfgDir+'/pyTemplates/Std2002/LC2-'+Nlinks+'_PhaMet-sci-WeakFilter.xml')
else :
     InXML.append(CfgDir+'/pyTemplates/Std2002/LC2-'+Nlinks+'_PhaMet-sci-NoFilter.xml')

#### TDI (and output)
fNTmpTDI="tmpTDI"+IdTmpRun+".xml"
if Nlinks=="2links":
    makefromtemplate(fNTmpTDI,CfgDir+'/pyTemplates/LC2_TDI-X.xml',\
                     outname=OutputBaseName+"-TDI"+extentionOut,\
                     outdatatype=options.OutDataType)
else:
    makefromtemplate(fNTmpTDI,CfgDir+'/pyTemplates/LC2_TDI-'+options.TDI+'.xml',\
                     outname=OutputBaseName+"-TDI"+extentionOut,\
                     outdatatype=options.OutDataType)
InXML.append(fNTmpTDI)

#### Ouputs
if options.rawMeasurements :
    fNTmpOut="tmpOut"+IdTmpRun+".xml"
    makefromtemplate(fNTmpOut,CfgDir+'/pyTemplates/LC2-Output-PhaMet.xml',\
                     outname=OutputBaseName+"-SC"+extentionOut,\
                     outdatatype=options.OutDataType,\
                     PhaMetmesSC=PhaMetmesSC)
    InXML.append(fNTmpOut)

#### Times
fNTmpTime="tmpTime"+IdTmpRun+".xml"
FixShotNoise="On"
if options.FixShotNoise :
    FixShotNoise="Off"
makefromtemplate(fNTmpTime,CfgDir+'/pyTemplates/LC2-Times-Detector.xml',\
                 cadence=str(options.timeStep),\
                 duration=str(options.duration),\
                 cadencePhys=str(options.timeStepPhys),\
                 UpdateShotNoise=FixShotNoise,\
                 InterpolationNoises="Lagrange")
InXML.append(fNTmpTime)

#### Add input file given in argument
for file in InputFileName :
     InXML.append(file)


print "XML inputs :",InXML
for file in InXML :
    if not os.path.isfile(file) :
        print "XML file "+file+"is not found !"
        sys.exit(1)


##### Start the output tree 
OutTree = ET.Element('XSIL')
tmp = ET.SubElement(OutTree,'Param')
tmp.set("Name","Author")
tmp.text = "Antoine Petiteau"
tmp = ET.SubElement(OutTree,'Param')
tmp.set("Name","GenerationDate")
tmp.set("Type","ISO-8601")
tmp.text = str(date.today().isoformat())
tmp = ET.SubElement(OutTree,'Comment')
tmp.text = "lisaXML 1.2 [A. Petiteau (based on M. Vallisneri), July 2013]"


##### Loop on input xml file
for fNIn in InXML :
    root = (ET.parse(fNIn)).getroot()
    childs = root.getchildren()
    icStart=0
    while icStart<len(childs) and (childs[icStart].tag == 'Comment' or childs[icStart].get('Name')=="Author" or childs[icStart].get('Name')=="GenerationDate" ) :
        icStart = icStart + 1
    for ic in xrange(icStart,len(childs)) :
        AddRootTree(childs[ic],OutTree)

#dispTree(OutTree,"")

##### Write the output tree
rec=ET.tostringlist(OutTree)
fOut=open(OutputBaseName+".xml",'w')
fOut.write('<?xml version="1.0"?>\n')
fOut.write('<!DOCTYPE XSIL SYSTEM "http://www.vallis.org/lisa-xml.dtd">\n')
fOut.write('<?xml-stylesheet type="text/xsl" href="lisa-xml.xsl"?>\n')
for ir in xrange(len(rec)) :
    xr=rec[ir]
    if re.search('</',xr) or ir==1 :
        fOut.write(xr+"\n")
    else:
        fOut.write(xr)
fOut.close()


##### Clean
for fNIn in InXML :
    if re.search(IdTmpRun,fNIn) :
        run("rm "+fNIn) 

##### Create command for LISACode
xmlOutGlob=OutputBaseName + "-output.xml"
cmdLC = BinDir + "LISACode2 "
if options.seed != 0 :
	cmdLC = cmdLC + " -s " + str(options.seed)
if options.verbose :
	cmdLC = cmdLC + " -v"
if options.OutDataType=="XML" :
    cmdLC = cmdLC + " -x " + xmlOutGlob
cmdLC = cmdLC + " " +  OutputBaseName + ".xml"
cmdLC = cmdLC +	" > log_" + OutputBaseName + "\n" 

"""
print cmdLC

for fls in glob.glob(OutputBaseName+"*"):
   print "stas:", fls

for fls in glob.glob("log_*"):
   print "stas, log files:", fls

sys.exit(0)
"""
### Run LISACode 
run(cmdLC,True,True)

if options.OutDataType=="XML" and os.path.isfile(xmlOutGlob) :
    print "Stas check:", xmlOutGlob	
    root = (ET.parse(xmlOutGlob)).getroot()
    childs = root.getchildren()
    icStart=0
    while icStart<len(childs) and (childs[icStart].tag == 'Comment' or childs[icStart].get('Name')=="Author" or childs[icStart].get('Name')=="GenerationDate" ) :
        icStart = icStart + 1
    for ic in xrange(icStart,len(childs)) :
        AddRootTree(childs[ic],OutTree)
    rec=ET.tostringlist(OutTree)
    fOut=open(OutputBaseName+".xml",'w')
    fOut.write('<?xml version="1.0"?>\n')
    fOut.write('<!DOCTYPE XSIL SYSTEM "http://www.vallis.org/lisa-xml.dtd">\n')
    fOut.write('<?xml-stylesheet type="text/xsl" href="lisa-xml.xsl"?>\n')
    for ir in xrange(len(rec)) :
        xr=rec[ir]
        if re.search('</',xr) or ir==1 :
            fOut.write(xr+"\n")
        else:
            fOut.write(xr)
    fOut.close()

for fls in glob.glob(OutputBaseName+"*"):
   print "stas:", fls
   run("mv " + fls + " TDI/", True) 

for fls in glob.glob("log_*"):
   print "stas, log files:", fls
   run("mv " + fls + " Log/", True)


