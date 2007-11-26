#!/usr/bin/env python

# SHOULD DO ALSO CONFIDENCE INTERVALS!
# could do min and max fields, if present, on the side of the corresponding central value
# ignore input value for min and max units, use central value
# just say that we're going to ignore min and max if not given in table...

# COMMENTS...

import sys
import cgi
import os
import time
import string

sys.stderr = sys.stdout
print "Content-type: text/html\n"

sys.path.append('/newman/user3/mldc/lib/python2.3/site-packages')
import lisaxml2 as lisaxml

# -> setup parameters
templatedir = '/newman/user3/mldc/templates/'

# -> parse the form
data = cgi.FieldStorage()

# see if we're passed a filename...
if 'filename' in data:
    inputfile = lisaxml.lisaXML(templatedir + data['filename'].value,'r')
else:
    # otherwise use the uploaded file...
    raise NotImplementedError

if 'challenge' in data:
    challengename = data['challenge'].value
else:
    challengename = 'challenge'
    
if 'collaboration' in data:
    collaborationname = data['collaboration'].value
else:
    collaborationname = 'collaboration'

# begin the form
print '<html>'

print """
<head>
    <title>MLDC2 result submission</title>

    <meta http-equiv="content-type" content="text/html; charset=iso-8859-1" />

    <style>
        body { font-family: Helvetica, sans-serif; font-size: 11pt; color: #000;}
    
        .sourceclass { width: 700px; border: solid #000 1px; margin: 20px 0px 20px 0px; padding: 10px 10px 10px 10px; }
        .entry { width: 696px; border: dotted #88a 1px; padding: 5px; }
    
        p { margin: 0px 0px 5px 0px; }
    
        table { width: 690px; color: #800; font-size: 10pt; border-spacing: 0px; border-collapse: collapse; table-layout: fixed; }
        tr { border: solid #008 1px; }
        td { width: auto; border: solid #008 1px; margin: 0px; padding: 2px; }
        td i { color: #000; }
    </style>
</head>
"""

print '<body>'
print '<form enctype = "multipart/form-data" method = "post" action = "http://www.tapir.caltech.edu/~mldc/cgi-bin/write-lisaxml.cgi">'

print '<div class="sourceclass" style="background: #eff;">'
print '<p><b>Challenge</b>: %s</p>' % challengename
print '<p><b>Collaboration</b>: %s</p>' % collaborationname
print '<input type="hidden" name="challenge" value="%s" />' % challengename
print '<input type="hidden" name="collaboration" value="%s" />' % collaborationname
print '</div>'

print '<div class="sourceclass" style="background: #ffe;">'

cntsrc = 0

for src in inputfile.SourceData:
    srcname = src.Name
    srctype = src.SourceType
    
    istable = isinstance(src,lisaxml.SourceTable)
    
    print '<div class="entry">'
    
    if istable:
        print '<input type="hidden" name="src%s:table" value="True" />' % cntsrc
        print '<p><b>Table of %s objects</b>:' % srctype,
    else:
        print '<input type="hidden" name="src%s:source" value="True" />' % cntsrc
        print '<p><b>%s object</b>:' % srctype,
    
    print '<input type="text" name="src%s:Name" size="48" maxlength="255" value="%s" /></p>' % (cntsrc,srcname)
    print '<input type="hidden" name="src%s:SourceType" value="%s" />' % (cntsrc,srctype)        

    if istable:
        print '<p><table>'

        paramunits = []
        for param in src.Table.parameters:
            if hasattr(src.Table,param + '_Unit'):
                paramunits.append(getattr(src.Table,param + '_Unit'))
            else:
                paramunits.append('')

        print '<tr>'
        for param in src.Table.parameters:
            print '<td style="font-size: 9pt">%s</td>' % param
        print '</tr>'
        
        print '<tr>'
        for paramunit in paramunits:
            print '<td style="font-size: 9pt">(%s)</td>' % paramunit
        print '</tr>'
        
        print '</table></p>'
        
        for ii in range(0,len(src.Table.parameters)):
            print '<input type="hidden" name="src%s:%s" value="None" />' % (cntsrc,src.Table.parameters[ii])
            print '<input type="hidden" name="src%s:%s_Unit" value="%s" />' % (cntsrc,src.Table.parameters[ii],paramunits[ii])
        
        print '<p><textarea name="src%s:Data" rows="10" cols="80">' % cntsrc
        
        for line in src.Table.Data:
            for val in line:
                print val,
            print
        print '</textarea></p>'
    else:    
        print '<table style="table-layout: fixed; width: auto;">'
    
        for param in src.parameters:
            # check that this is not a Min or Max parameter
            
            if hasattr(src,param + '_Unit'):
                paramunit = getattr(src,param + '_Unit')
            else:
                paramunit = ''
        
            paramvalue = getattr(src,param)
        
            print '<tr>'
            print '<td>%s (%s):</td>' % (param,paramunit)
            print '<td><input type="text" name="src%s:%s" size="32" maxlength="255" value="%s" /></td>' % (cntsrc,param,paramvalue)
            print '<input type="hidden" name="src%s:%s_Unit" value="%s" />' % (cntsrc,param,paramunit)

            # do the minimum value here (with its hidden value)
            
            # do the maximum value here (with its hidden value)
            
            print '</tr>'
    
        print '</table>'
    
    print '</div>'
    
    cntsrc = cntsrc + 1

inputfile.close()

print '</div>'

print '<div class="sourceclass" style="background: #fef;">'
print '<p><b>Comments</b>:<textarea name="comments" rows="5" cols="80"></textarea>'
print '<input type="submit" name = "action" value = "write" />'
print '</div>'

print '</form>'
print '</body></html>'
