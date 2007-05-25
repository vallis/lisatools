#!/usr/bin/env python

# -> setup modules

import sys
import cgi
import os
import time

sys.stderr = sys.stdout
print "Content-type: text/xml\n"

sys.path.append('/newman/user3/mldc/lib/python2.3/site-packages')
import lisaxml

import BBH
import EMRI

def paramcheck(par):    
    if par in ['NaN','nan','NAN']:
        return par
    else:
        return float(par)

# -> setup parameters
datadir = '/home/mldc/'

# -> parse some of the form
data = cgi.FieldStorage()

# get the name of the challenge and collaboration
challenge = data['challenge'].value
collaboration = data['collaboration'].value

outputfilename = collaboration + '-' + challenge + '.xml'
initialcomment = "Please VERIFY the accuracy of the values given below, and READ the generation log; GO BACK and REPOST if you need to make corrections.\n"

outputfile = lisaxml.lisaXML(datadir + outputfilename,author=collaboration,comments=initialcomment)

outputfile.comments += " \n> Begin XML generation log\n"

# process GB

for suffix in ['all','all-ci','1','1-ci','2','2-ci','3','3-ci','4','4-ci']:
    gbfilename = 'file-gb-' + suffix

    if 'all' in suffix:
        window = 'full GB population'
    else:
        window = 'GB window ' + suffix

    if 'ci' in suffix:
        window = 'confidence intervals, ' + window

    if data.has_key(gbfilename) and data[gbfilename].filename:
        outputfile.comments += "-> Found GB specification for %s in uploaded file %s\n" % (window,data[gbfilename].filename)

        outputtextfilename = datadir + collaboration + '-' + challenge + '-gb-' + suffix + '.txt'
        outputtextfile = open(outputtextfilename,'w')

        linecnt = 1
        errors = 0
        written = 0
        gbsources = []
    
        for line in data[gbfilename].file:
            try:
                pars = map(paramcheck,line.split())
            
                if pars[0] in gbsources:
                    outputfile.comments += "--> Duplicated source index %s on line %s of GB file %s; therefore...\n" % (pars[0],linecnt,data[gbfilename].filename)
                    raise            
                else:
                    gbsources.append(pars[0])

                if 'ci' in suffix:
                    if len(pars) != 15:
                        raise
                else:
                    if len(pars) != 8:
                        raise
                
                outputtextfile.write(line)

                written += 1
                if written == 1:
                    outputfile.comments += "--> First readable line (%s) is:\n" % linecnt
                    outputfile.comments += line
            except:
                if line.split():
                    outputfile.comments += "--> I have problems processing line %s of GB file %s (probably not enough or too many parameters or duplicate source index); I'll drop it\n" % (linecnt,data[gbfilename].filename)

                errors += 1
                
                if errors > 5:
                    outputfile.comments += "--> Too many problems in GB file %s; I'm dropping it after line %s\n" % (data[gbfilename].filename,linecnt)
                    break

                pass

            linecnt += 1

        outputfile.comments += "--> Read data for %s sources from GB file %s\n" % (written,data[gbfilename].filename)

        outputtextfile.close()

        table = lisaxml.Table(outputtextfilename,written)

        table.Index = None;
        table.Index_Unit = '1';

        if 'ci' in suffix:
            table.FrequencyMin         = None; table.FrequencyMin_Unit         = 'Hz';
            table.FrequencyMax         = None; table.FrequencyMax_Unit         = 'Hz';
            table.EclipticLatitudeMin  = None; table.EclipticLatitudeMin_Unit  = 'Radian';
            table.EclipticLatitudeMax  = None; table.EclipticLatitudeMax_Unit  = 'Radian';
            table.EclipticLongitudeMin = None; table.EclipticLongitudeMin_Unit = 'Radian';
            table.EclipticLongitudeMax = None; table.EclipticLongitudeMax_Unit = 'Radian';
            table.AmplitudeMin         = None; table.AmplitudeMin_Unit         = '1';
            table.AmplitudeMax         = None; table.AmplitudeMax_Unit         = '1';
            table.PolarizationMin      = None; table.PolarizationMin_Unit      = 'Radian';
            table.PolarizationMax      = None; table.PolarizationMax_Unit      = 'Radian';
            table.InclinationMin       = None; table.InclinationMin_Unit       = 'Radian';
            table.InclinationMax       = None; table.InclinationMax_Unit       = 'Radian';        
            table.InitialPhaseMin      = None; table.InitialPhaseMin_Unit      = 'Radian';
            table.InitialPhaseMax      = None; table.InitialPhaseMax_Unit      = 'Radian';
        else:
            table.Frequency         = None; table.Frequency_Unit         = 'Hz';
            table.EclipticLatitude  = None; table.EclipticLatitude_Unit  = 'Radian';
            table.EclipticLongitude = None; table.EclipticLongitude_Unit = 'Radian';
            table.Amplitude         = None; table.Amplitude_Unit         = '1';
            table.Polarization      = None; table.Polarization_Unit      = 'Radian';
            table.Inclination       = None; table.Inclination_Unit       = 'Radian';
            table.InitialPhase      = None; table.InitialPhase_Unit      = 'Radian';

        pwtable = lisaxml.PlaneWaveTable('GB-' + suffix,textfilename=outputtextfilename)

        pwtable.SourceType = 'GalacticBinary'
        pwtable.Table = table

        outputfile.SourceData(pwtable)

# process SMBH

bbhdata = None

if data.has_key('file-smbh') and data['file-smbh'].filename:
    outputfile.comments += "-> Found SMBH specification in uploaded file %s\n" % data['file-smbh'].filename
    if data.has_key('text-smbh') and data['text-smbh'].value:
        outputfile.comments += "--> (Neglecting all content in text form)\n"
    bbhdata = data['file-smbh'].file
elif data.has_key('text-smbh') and data['text-smbh'].value:
    outputfile.comments += "-> Found SMBH specification in text form\n"
    bbhdata = data['text-smbh'].value.split('\n')
    
if bbhdata:
    linecnt = 1
    bbhsources = []

    for line in bbhdata:
        try:
            pars = map(paramcheck,line.split())

            sysname = 'SMBH-%s' % int(pars[0])

            # check for duplicates
            if sysname in bbhsources:
                outputfile.comments += "--> Duplicated source index %s on line %s of the SMBH specification; therefore...\n" % (pars[0],linecnt)
                raise
            
            mysystem = BBH.BlackHoleBinary(sysname)

            mysystem.EclipticLatitude  = pars[1]
            mysystem.EclipticLongitude = pars[2]
            mysystem.Polarization      = pars[3]
            mysystem.Inclination       = pars[4]
            mysystem.Distance          = pars[5]
            mysystem.Mass1             = pars[6]
            mysystem.Mass2             = pars[7]
            mysystem.CoalescenceTime   = pars[8]
            mysystem.InitialAngularOrbitalPhase = pars[9]

            if len(pars) > 10:
                outputfile.comments += "--> Ignoring extra params %s in line %s of the SMBH specification\n" % (pars[10:],linecnt)

            outputfile.SourceData(mysystem,name=sysname)
        except:
            if line.split():
                outputfile.comments += "--> I have problems processing line %s of the SMBH specification (wrong syntax or not enough parameters); I'll drop it\n" % linecnt

            pass
        else:
            bbhsources.append(sysname)
            
        linecnt += 1

    outputfile.comments += "--> Read %s SMBH sources\n" % len(bbhsources)

# process EMRI

emridata = None

if data.has_key('file-emri') and data['file-emri'].filename:
    outputfile.comments += "-> Found EMRI specification in uploaded file %s\n" % data['file-emri'].filename
    if data.has_key('text-emri') and data['text-emri'].value:
        outputfile.comments += "--> (Neglecting all content in text form)\n"
    emridata = data['file-emri'].file
elif data.has_key('text-emri') and data['text-emri'].value:
    outputfile.comments += "-> Found EMRI specification in text form\n"
    emridata = data['text-emri'].value.split('\n')
    
if emridata:
    linecnt = 1
    emrisources = []

    for line in emridata:
        try:
            pars = map(paramcheck,line.split())

            sysname = 'EMRI-%s' % int(pars[0])

            if challenge == 'challenge1.3' and (int(pars[0]) < 1 or int(pars[5]) > 5):
                outputfile.comments += "--> Source index %s not between 1 and 5 on line %s of the EMRI specification; therefore...\n" % (pars[0],linecnt)
                raise                

            # check for duplicates
            if sysname in emrisources:
                outputfile.comments += "--> Duplicated source index %s on line %s of the EMRI specification; therefore...\n" % (pars[0],linecnt)
                raise
            else:
                emrisources.append(sysname)
            
            mysystem = EMRI.ExtremeMassRatioInspiral(sysname)

            mysystem.EclipticLatitude     = pars[1]
            mysystem.EclipticLongitude    = pars[2]
            mysystem.PolarAngleOfSpin     = pars[3]
            mysystem.AzimuthalAngleOfSpin = pars[4]
            mysystem.Spin			   	  = pars[5]
            mysystem.MassOfCompactObject  = pars[6]
            mysystem.MassOfSMBH           = pars[7]

            mysystem.InitialAzimuthalOrbitalFrequency = pars[8]
            mysystem.InitialAzimuthalOrbitalPhase     = pars[9]
            mysystem.InitialEccentricity		      = pars[10]
            mysystem.InitialTildeGamma 		          = pars[11]
            mysystem.InitialAlphaAngle		          = pars[12]
            mysystem.LambdaAngle                      = pars[13]
            mysystem.Distance                         = pars[14]

            if len(pars) > 15:
                outputfile.comments += "--> Ignoring extra params %s in line %s of the EMRI specification\n" % (pars[15:],linecnt)

            outputfile.SourceData(mysystem,name=sysname)
        except:
            if line.split():
                outputfile.comments += "--> I have problems processing line %s of the EMRI specification (wrong syntax or not enough parameters or duplicate source index); I'll drop it\n" % linecnt

            pass
            
        linecnt += 1

    outputfile.comments += "--> Read %s EMRI sources\n" % len(emrisources)

outputfile.comments += '> All done\n'
outputfile.comments += ' \n'

datedtarfile = collaboration + '-' + challenge + '-' + time.strftime('%y%m%d-%H%M%S',time.localtime()) + '.tar.gz'

outputfile.comments += 'Your submission was received on ' + ( time.strftime('%y/%m/%d',time.localtime()) + ' at ' +
                                                              time.strftime('%H:%M:%S PDT',time.localtime()) )
outputfile.comments += ' and it can be retrieved at the URL\n http://www.tapir.caltech.edu/~mldc/submit/' + datedtarfile + '\n'
outputfile.comments += ' \n'

outputfile.close()

allgbfiles = collaboration + '-' + challenge + '-gb-*.txt'
idfile = collaboration + '-id.txt'

os.chdir('/home/mldc')
os.system('tar zcvf %(datedtarfile)s %(outputfilename)s %(idfile)s %(allgbfiles)s > /dev/null' % globals())

# TODO errors for SMBH and EMRIs
# TODO (perhaps) check for index correspondence between 

# -> output XML to the browser

for line in open(outputfilename,'r'):
    print line,

os.system('rm ' + outputfilename)
os.system('rm ' + allgbfiles)
