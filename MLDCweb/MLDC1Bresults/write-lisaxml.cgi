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

# -> setup parameters
datadir = '/home/mldc/CHALLENGE1B/'

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

outxml.Comment = "Please check this file for correctness. It can be retrieved at http://www.tapir.caltech.edu/~mldc/submit-1B/" + collaboration + '-' + challenge + '-' + time.strftime('%y%m%d-%H%M%S',time.localtime()) + '.xml'

if data['comments'].value:
    outxml.Comment += '\n\nYour comments:\n\n' + data['comments'].value

srccnt = 0

# loop over possible sources
while True:
    srcstr = "src%s:" % srccnt
    
    if (srcstr + "source") in data.keys():
        sourcetype = data[srcstr + 'SourceType'].value
        sourcename = data[srcstr + 'Name'].value
        
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
        
        outxml.SourceData.append(makesource)
        
    elif (srcstr + "table") in data.keys():
        sourcetype = data[srcstr + 'SourceType'].value
        sourcename = data[srcstr + 'Name'].value
        
        mydata = data[srcstr + 'Data'].value
        
        stream = lisaxml.Stream(data=mydata,filetype='Local',encoding='Text')
        table = lisaxml.Table(stream,stream.Length,stream.Records,sourcename)
        
        for key in data.keys():
                if srcstr in key:
                    if (key not in (srcstr + "table", srcstr + 'Name', srcstr + 'SourceType', srcstr + 'Data')) and ('_Unit' not in key):
                        paramname = key.split(':')[1]
                        
                        # need to convert to a number here?
                        setattr(table,paramname,None)
                        
                        if (key + '_Unit') in data.keys():
                            setattr(table,paramname + '_Unit',data[key + '_Unit'].value)
        
        sourcetable = lisaxml.SourceTable(sourcetype,sourcename,table)
        
        outxml.SourceData.append(sourcetable)
    else:
        break
    
    srccnt = srccnt + 1

outxml.close()

for line in open(outputfile,'r'):
    print line,

