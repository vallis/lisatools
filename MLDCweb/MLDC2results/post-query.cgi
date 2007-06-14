#!/usr/bin/env python

# -> setup modules

import sys
import cgi
import os
import time
import string

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

def paramcheck2(line):
    pars = line.split()
    
    retpars = []    
    for i in range(len(pars)):
        if '#' in pars[i]:
            return retpars, string.join(pars[i:])
        else:
            retpars.append(paramcheck(pars[i]))

    return retpars, ''

# -> setup parameters
datadir = '/home/mldc/'

# -> parse some of the form
data = cgi.FieldStorage()

# get the name of the challenge and collaboration
challenge = data['challenge'].value
collaboration = data['collaboration'].value

outputfilename = collaboration + '-' + challenge + '.xml'
initialcomment = "This is the XML solution file for %s.\n" % challenge

outputfile = lisaxml.lisaXML(datadir + outputfilename,author=collaboration,comments=initialcomment)

outputfile.comments += " \n"
outputfile.comments += "Please VERIFY the accuracy of the values given below, and READ the generation log; GO BACK and REPOST if you need to make corrections.\n"
outputfile.comments += " \n"
outputfile.comments += "> Begin XML generation log\n"

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
                pars, parscomment = paramcheck2(line)
                # pars = map(paramcheck,line.split())
            
                if int(pars[0]) in gbsources:
                    outputfile.comments += "--> Duplicated source index %s on line %s of GB file %s; therefore...\n" % (int(pars[0]),linecnt,data[gbfilename].filename)
                    raise            
                else:
                    gbsources.append(int(pars[0]))

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

bbhsourcelist = {}
bbhcomments = {}
    
if bbhdata:
    linecnt = 1
    bbhsources = []

    for line in bbhdata:
        try:
            pars, parscomment = paramcheck2(line)
            # pars = map(paramcheck,line.split())

            sysname = 'SMBH-%s' % int(pars[0])

            # check for duplicates
            if sysname in bbhsources:
                outputfile.comments += "--> Duplicated source index %s on line %s of the SMBH specification; therefore...\n" % (int(pars[0]),linecnt)
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
        except:
            if line.split():
                outputfile.comments += "--> I have problems processing line %s of the SMBH specification (wrong syntax or not enough parameters or duplicate source index); I'll drop it\n" % linecnt

            pass
        else:
            bbhsources.append(sysname)
            bbhsourcelist[sysname] = mysystem
            bbhcomments[sysname] = parscomment
            
        linecnt += 1

    outputfile.comments += "--> Read %s SMBH sources\n" % len(bbhsources)

    bbherrordata = None

    if data.has_key('file-smbh-ci') and data['file-smbh-ci'].filename:
        outputfile.comments += "-> Found SMBH confidence-interval specification in uploaded file %s\n" % data['file-smbh-ci'].filename
        if data.has_key('text-smbh-ci') and data['text-smbh-ci'].value:
            outputfile.comments += "--> (Neglecting all content in text form)\n"
        bbherrordata = data['file-smbh-ci'].file
    elif data.has_key('text-smbh-ci') and data['text-smbh-ci'].value:
        outputfile.comments += "-> Found SMBH confidence-interval specification in text form\n"
        bbherrordata = data['text-smbh-ci'].value.split('\n')

    if bbherrordata:
        linecnt = 1
        bbherrorsources = []

        for line in bbherrordata:
            try:
                pars, parscomment = paramcheck2(line)
                # pars = map(paramcheck,line.split())

                sysname = 'SMBH-%s' % int(pars[0])

                # check if it exists...
                if sysname not in bbhsourcelist:
                    outputfile.comments += "--> Confidence interval specification for unspecified source index %s on line %s; therefore...\n" % (int(pars[0]),linecnt)
                    raise
                elif sysname in bbherrorsources:
                    outputfile.comments += "--> Duplicated source index %s on line %s; therefore...\n" % (int(pars[0]),linecnt)
                    raise                    

                mysystem = bbhsourcelist[sysname]

                mysystem.EclipticLatitudeMin           = pars[1];  mysystem.EclipticLatitudeMax           = pars[2]
                mysystem.EclipticLongitudeMin          = pars[3];  mysystem.EclipticLongitudeMax          = pars[4]
                mysystem.PolarizationMin               = pars[5];  mysystem.PolarizationMax               = pars[6]
                mysystem.InclinationMin                = pars[7];  mysystem.InclinationMax                = pars[8]
                mysystem.DistanceMin                   = pars[9];  mysystem.DistanceMax                   = pars[10]
                mysystem.Mass1Min                      = pars[11]; mysystem.Mass1Max                      = pars[12]
                mysystem.Mass2Min                      = pars[13]; mysystem.Mass2Max                      = pars[14]
                mysystem.CoalescenceTimeMin            = pars[15]; mysystem.CoalescenceTimeMax            = pars[16]
                mysystem.InitialAngularOrbitalPhaseMin = pars[17]; mysystem.InitialAngularOrbitalPhaseMax = pars[18]

                mysystem.EclipticLatitudeMin_Unit  = 'Radian'    ; mysystem.EclipticLatitudeMax_Unit  = 'Radian' 
                mysystem.EclipticLongitudeMin_Unit = 'Radian'    ; mysystem.EclipticLongitudeMax_Unit = 'Radian'    
                mysystem.PolarizationMin_Unit      = 'Radian'    ; mysystem.PolarizationMax_Unit      = 'Radian'    
                mysystem.InclinationMin_Unit       = 'Radian'    ; mysystem.InclinationMax_Unit       = 'Radian'    
                mysystem.DistanceMin_Unit          = 'Parsec'    ; mysystem.DistanceMax_Unit          = 'Parsec'
                mysystem.Mass1Min_Unit             = 'SolarMass' ; mysystem.Mass1Max_Unit             = 'SolarMass'
                mysystem.Mass2Min_Unit             = 'SolarMass' ; mysystem.Mass2Max_Unit             = 'SolarMass'
                mysystem.CoalescenceTimeMin_Unit   = 'Second'    ; mysystem.CoalescenceTimeMax_Unit   = 'Second'
                mysystem.InitialAngularOrbitalPhaseMin_Unit = 'Radian'; mysystem.InitialAngularOrbitalPhaseMax_Unit = 'Radian'   

                if len(pars) > 19:
                    outputfile.comments += "--> Ignoring extra params %s in line %s\n" % (pars[19:],linecnt)
            except:
                if line.split():
                    outputfile.comments += "--> I have problems processing line %s of the SMBH confidence-interval specification; I'll drop it\n" % linecnt

                pass
            else:
                if parscomment:
                    bbhcomments[sysname] += '\n' + parscomment
                bbherrorsources.append(sysname)
                outputfile.comments += "--> Added confidence interval specification for source index %s\n" % int(pars[0])

        linecnt += 1

    bbhsourcelistsort = bbhsourcelist.keys()
    bbhsourcelistsort.sort()

    for sysname in bbhsourcelistsort:
        outputfile.SourceData(bbhsourcelist[sysname],name=sysname,comments=bbhcomments[sysname])

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
    
emrisourcelist = {}
emricomments = {}
    
if emridata:
    linecnt = 1
    emrisources = []

    for line in emridata:
        try:
            # pars = map(paramcheck,line.split())
            pars, parscomment = paramcheck2(line)
                
            sysname = 'EMRI-%s' % int(pars[0])

            # if challenge == 'challenge1.3' and (int(pars[0]) < 1 or int(pars[0]) > 5):
            #     outputfile.comments += "--> Source index %s not between 1 and 5 on line %s of the EMRI specification; therefore...\n" % (int(pars[0]),linecnt)
            #     raise                

            # check for duplicates
            if sysname in emrisources:
                outputfile.comments += "--> Duplicated source index %s on line %s of the EMRI specification; therefore...\n" % (int(pars[0]),linecnt)
                raise
            
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
        except:
            if line.split():
                outputfile.comments += "--> I have problems processing line %s of the EMRI specification (wrong syntax or not enough parameters or duplicate source index); I'll drop it\n" % linecnt

            pass
        else:
            emrisources.append(sysname)
            emrisourcelist[sysname] = mysystem
            emricomments[sysname] = parscomment
            
        linecnt += 1

    outputfile.comments += "--> Read %s EMRI sources\n" % len(emrisources)

    emrierrordata = None

    if data.has_key('file-emri-ci') and data['file-emri-ci'].filename:
        outputfile.comments += "-> Found EMRI confidence-interval specification in uploaded file %s\n" % data['file-emri-ci'].filename
        if data.has_key('text-emri-ci') and data['text-emri-ci'].value:
            outputfile.comments += "--> (Neglecting all content in text form)\n"
        emrierrordata = data['file-emri-ci'].file
    elif data.has_key('text-emri-ci') and data['text-emri-ci'].value:
        outputfile.comments += "-> Found EMRI confidence-interval specification in text form\n"
        emrierrordata = data['text-emri-ci'].value.split('\n')

    if emrierrordata:
        linecnt = 1
        emrierrorsources = []

        for line in emrierrordata:
            try:
                # pars = map(paramcheck,line.split())
                pars, parscomment = paramcheck2(line)

                sysname = 'EMRI-%s' % int(pars[0])

                # check if it exists...
                if sysname not in emrisourcelist:
                    outputfile.comments += "--> Confidence interval specification for unspecified source index %s on line %s; therefore...\n" % (int(pars[0]),linecnt)
                    raise
                elif sysname in emrierrorsources:
                    outputfile.comments += "--> Duplicated source index %s on line %s; therefore...\n" % (int(pars[0]),linecnt)
                    raise

                mysystem = emrisourcelist[sysname]

                paramlist = (('EclipticLatitude',                 'Radian'     ),
                             ('EclipticLongitude',                'Radian'     ),
                             ('PolarAngleOfSpin',                 'Radian'     ),
                             ('AzimuthalAngleOfSpin',             'Radian'     ),
                             ('Spin',                             'MassSquared'),
                             ('MassOfCompactObject',              'SolarMass'  ),
                             ('MassOfSMBH',                       'SolarMass'  ),
                             ('InitialAzimuthalOrbitalFrequency', 'Hertz'      ),
                             ('InitialAzimuthalOrbitalPhase',     'Radian'     ),
                             ('InitialEccentricity',              'Unit'       ),
                             ('InitialTildeGamma',                'Radian'     ),
                             ('InitialAlphaAngle',                'Radian'     ),
                             ('LambdaAngle',                      'Radian'     ),
                             ('Distance',                         'Parsec'     ))

                paramcount = 1
                for paramtuple in paramlist:
                    setattr(mysystem,paramtuple[0]+'Min',pars[paramcount])
                    setattr(mysystem,paramtuple[0]+'Max',pars[paramcount+1])

                    setattr(mysystem,paramtuple[0]+'Min_Unit',paramtuple[1])
                    setattr(mysystem,paramtuple[0]+'Max_Unit',paramtuple[1])

                    paramcount += 2
                
                if len(pars) > 29:
                    outputfile.comments += "--> Ignoring extra params %s in line %s\n" % (pars[29:],linecnt)
            except:
                if line.split():
                    outputfile.comments += "--> I have problems processing line %s of the EMRI confidence-interval specification; I'll drop it\n" % linecnt

                pass
            else:
                emrierrorsources.append(sysname)
                if parscomment:
                    emricomments[sysname] += '\n' + parscomment
                outputfile.comments += "--> Added confidence interval specification for source index %s\n" % int(pars[0])

    emrisourcelistsort = emrisourcelist.keys()
    emrisourcelistsort.sort()

    for sysname in emrisourcelistsort:
        outputfile.SourceData(emrisourcelist[sysname],name=sysname,comments=emricomments[sysname])

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

# -> output XML to the browser

for line in open(outputfilename,'r'):
    print line,

os.system('rm ' + outputfilename)
os.system('rm ' + allgbfiles)
