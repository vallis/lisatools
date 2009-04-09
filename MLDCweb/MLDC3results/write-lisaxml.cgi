#!/usr/bin/env python

import sys
import cgi
import os
import time
import string
import shutil

sys.stderr = sys.stdout
print "Content-type: text/xml\n"

sys.path.append('/newman/user3/mldc/lib/python2.3/site-packages')
import lisaxml2 as lisaxml

import numpy

# -> setup parameters
datadir = '/home/mldc/CHALLENGE3/'

# -> parse some of the form
data = cgi.FieldStorage()

collaboration = data['collaboration'].value
challenge =     data['challenge'].value

outputfile = datadir + collaboration + '-' + challenge + '-' + time.strftime('%y%m%d-%H%M%S',time.localtime()) + '.xml'
shortoutputfile = datadir + collaboration + '-' + challenge + '.xml'

outxml = lisaxml.lisaXML(outputfile,'w')

outxml.Author = collaboration
# outxml.Comment = '%s' % data.keys()
# outxml.Comment = data['src0:Data'].value

outxml.Comment = "Thank you for your submission! Please check this file for correctness. It can be retrieved at http://www.tapir.caltech.edu/~mldc/submit-3/" + collaboration + '-' + challenge + '-' + time.strftime('%y%m%d-%H%M%S',time.localtime()) + '.xml'

if data['comments'].value:
    outxml.Comment += '\n\nYour comments:\n\n' + data['comments'].value

srccnt = 0

# loop over possible sources
while True:
    srccnt2 = 0

    while True:
        srcstr = "src%s-%s:" % (srccnt,srccnt2)

        filename = "src%s:file" % srccnt

        if filename in data.keys() and data[filename].filename:
            sourcetype = data[srcstr + 'SourceType'].value
            sourcename = data[srcstr + 'Name'].value + (' src%s' % srccnt)
            
            if sourcetype == 'GalacticBinary':
                partab = [  ("Frequency","Hertz"),
                            ("FrequencyDerivative","Hertz/Second"),
                            ("EclipticLatitude","Radian"),
                            ("EclipticLongitude","Radian"),
                            ("Amplitude","1"),
                            ("Inclination","Radian"),
                            ("Polarization","Radian"),
                            ("InitialPhase","Radian") ]                          
            elif sourcetype == 'FastSpinBlackHoleBinary':
                partab = [  ("EclipticLatitude","Radian"),
                            ("EclipticLongitude","Radian"),
                            ("Mass1","SolarMass"),
                            ("Mass2","SolarMass"),
                            ("Spin1","MassSquared"),
                            ("Spin2","MassSquared"),
                            ("PolarAngleOfSpin1","Radian"),
                            ("AzimuthalAngleOfSpin1","Radian"),
                            ("PolarAngleOfSpin2","Radian"),
                            ("AzimuthalAngleOfSpin2","Radian"),
                            ("InitialPolarAngleL","Radian"),
                            ("InitialAzimuthalAngleL","Radian"),
                            ("CoalescenceTime","Second"),
                            ("PhaseAtCoalescence","Radian"),
                            ("Distance","Parsec")  ]
            elif sourcetype == 'ExtremeMassRatioInspiral':
                partab = [  ("EclipticLatitude","Radian"),
                            ("EclipticLongitude","Radian"),
                            ("MassOfSMBH","SolarMass"),
                            ("MassOfCompactObject","SolarMass"),
                            ("Spin","MassSquared"),
                            ("PolarAngleOfSpin","Radian"),
                            ("AzimuthalAngleOfSpin","Radian"),
                            ("InitialAzimuthalOrbitalFrequency","Hertz"),
                            ("InitialAzimuthalOrbitalPhase","Radian"),
                            ("InitialEccentricity","Unit"),
                            ("InitialTildeGamma","Radian"),
                            ("InitialAlphaAngle","Radian"),
                            ("LambdaAngle","Radian"),
                            ("Distance","Parsec") ]            
            elif sourcetype == 'CosmicStringCusp':
                partab = [  ("EclipticLatitude","Radian"),
                            ("EclipticLongitude","Radian"),
                            ("Polarization","Radian"),
                            ("Amplitude","1"),
                            ("CentralTime","Second"),
                            ("MaximumFrequency","Hertz") ]
            elif sourcetype == 'StochasticBackground':
                partab = [  ("GravitationalWavePSD","Hertz^2"),
                            ("pm1", "Hertz"),
                            ("pm1s","Hertz"),
                            ("pm2", "Hertz"),
                            ("pm2s","Hertz"),
                            ("pm3", "Hertz"),
                            ("pm3s","Hertz"),
                            ("pd1", "Hertz"),
                            ("pd1s","1/Hertz"),
                            ("pd2", "1/Hertz"),
                            ("pd2s","1/Hertz"),
                            ("pd3", "1/Hertz"),
                            ("pd3s","1/Hertz") ]
            
            partab.append(('Probability','1'))
            theparams = [par[0] for par in partab]

            theunits = {}
            for par in partab:
                theunits[par[0]] = par[1]
            theunits['Index'] = 1
            
            # if the first line of the file begins with #, use it to set parameter names
            # otherwise use the standard fields
            firstline = data[filename].file.readline()
            
            if firstline[0] == '#':
                theparams = map(string.strip,firstline[1:].split())
            else:
                data[filename].file.seek(0)
            
            mydata = data[filename].file.read()
            
            stream = lisaxml.Stream(data=mydata,filetype='Local',encoding='Text')
            table = lisaxml.Table(stream,stream.Length,stream.Records,sourcename)
            
            for par in theparams:
                setattr(table,par,None)
                
                if par in theunits:
                    setattr(table,par + '_Unit',theunits[par])
            
            sourcetable = lisaxml.SourceTable(sourcetype,sourcename,table)
            
            outxml.SourceData.append(sourcetable)
        
            srccnt2 = 1; break
        elif (srcstr + "source") in data.keys():
            sourcetype = data[srcstr + 'SourceType'].value
            sourcename = data[srcstr + 'Name'].value + ' ' + srcstr[:-1]
        
            makesource = lisaxml.Source.makeSource(sourcetype)
        
            makesource.__dict__['Name'] = makesource.__dict__['name']    = sourcename
            makesource.__dict__['Type'] = makesource.__dict__['xmltype'] = 'PlaneWave'
        
            # TO DO: can do comments here
            makesource.__dict__['Comment'] = ''
        
            for key in data.keys():
                if srcstr in key:
                    if (key not in (srcstr + "source", srcstr + 'Name')) and ('_Unit' not in key):
                        paramname = key.split(':')[1]
                    
                        # need to convert to a number here?
                        setattr(makesource,paramname,data[key].value)
                    
                        if (key + '_Unit') in data.keys():
                            setattr(makesource,paramname + '_Unit',data[key + '_Unit'].value)
        
            if sourcetype == 'ExtremeMassRatioInspiral':
                setattr(makesource,'FixHarmonics','5')
                setattr(makesource,'FixHarmonics_Unit','1')
        
            # this is just a hack for the moment, this bug must be fixed
            makesource.parameters.insert(0,'SourceType')
            
            outxml.SourceData.append(makesource)
        else:
            break
        
        srccnt2 = srccnt2 + 1
    
    if srccnt2 == 0:
        break
    
    srccnt = srccnt + 1

outxml.close()

for line in open(outputfile,'r'):
    print line,

