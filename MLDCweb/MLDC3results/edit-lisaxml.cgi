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
    inputfilename = data['filename'].value
    inputfile = lisaxml.lisaXML(templatedir + inputfilename,'r')
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

print """<head>
    <title>
        MLDC2 result submission
    </title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8">
    <style type="text/css">
        body { font-family: Helvetica, sans-serif; font-size: 11pt; color: #000;}

        .commentclass { width: 700px; border: solid #000 1px; margin: 20px 0px 20px 0px; padding: 10px 10px 10px 10px; }    
        .sourceclass { width: 700px; border: solid #000 1px; margin: 20px 0px 20px 0px; padding: 10px 10px 10px 10px; }
        .entry { width: 690px; margin-bottom: 5px; border: dotted #88a 1px; padding: 5px; }

        p { margin: 0px 0px 5px 0px; }

        table { width: 690px; color: #800; font-size: 10pt; border-spacing: 0px; border-collapse: collapse; table-layout: fixed; }
        tr { border: solid #008 1px; }
        td { width: auto; border: solid #008 1px; margin: 0px; padding: 2px; }
        td i { color: #000; }
    </style>
    <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.2.6/jquery.min.js" type="text/javascript"></script>
    <script type="text/javascript">
        function duplicate_source() {
            $('#challengesubmit').before($('.sourceclass:last').clone());
            $('.sourceclass:last .entry:gt(0)').remove();
    
            var srcid = $('.sourceclass:last input:first').attr('name').match(/src([0-9]+)-([0-9]+):source/);

            var srcnum = parseInt(srcid[1]);
            var oldsrc = 'src' + srcnum;
            var newsrc = 'src' + (srcnum+1);

            $('.sourceclass:last p:first').each(function () {this.innerHTML = this.innerHTML.replace(oldsrc,newsrc)});
            $('.sourceclass:last').each(function () {this.id = newsrc});
            /* using jQuery */
            $('.sourceclass:last input').each(function () {$(this).attr('name',$(this).attr('name').replace(oldsrc,newsrc))});
            /* subs using DOM... this.onClick does not seem to work */
            $('.sourceclass:last a[onClick]').each(function () {this.setAttribute('onClick',this.getAttribute('onClick').replace(oldsrc,newsrc))});            
        }

        function remove_source() {
            $('.sourceclass:gt(0):last').remove();
        }

        function duplicate_entry(src) {
            var lastentry = '#' + src + ' .entry:last';

            $(lastentry).after($(lastentry).clone());

            var srcid = $(lastentry + ' input').attr('name').match(/src([0-9]+)-([0-9]+):source/);
    
            var srcnum = parseInt(srcid[1]);
            var entrynum = parseInt(srcid[2]);
    
            var oldsrc = 'src' + srcnum + '-' + entrynum;
            var newsrc = 'src' + srcnum + '-' + (entrynum+1);
    
            $(lastentry + ' p:first').each(function () {this.innerHTML = this.innerHTML.replace(oldsrc,newsrc)});
            $(lastentry + ' input').each(function () {this.setAttribute('name',this.getAttribute('name').replace(oldsrc,newsrc))});
        }

        function remove_entry(src) {
            $('#' + src + ' .entry:gt(0):last').remove();
        }
    </script>
</head>"""

print '<body>'
print '<form enctype = "multipart/form-data" method = "post" action = "http://www.tapir.caltech.edu/~mldc/cgi-bin/write-lisaxml.cgi">'

print '<div class="commentclass" id="challengetitle" style="background: #eff;">'

print '<p><b>Challenge</b>: %s</p>' % challengename
print '<input type="hidden" name="challenge" value="%s" />' % challengename

print '<p><b>Collaboration</b>: %s</p>' % collaborationname
print '<input type="hidden" name="collaboration" value="%s" />' % collaborationname

print '<hr/>'

print """<small><p>If you want to enter multiple likelihood/posterior maxima:</p>
<ul><li>If you have determined the maxima separately for each source,
use multiple entries (dotted boxes) under each source (yellow box),
and indicate the relative likehood/posterior in the Probability field.</li>
<li>If you have determined the maxima jointly for all sources (i.e., if you did global fits),
use multiple entries (dotted boxes) for different sources
in the same maximum (yellow box), and indicate the relative likelihood/posterior
in the Probability field of the first source.</li></ul>
<p>Please use the Comments field to specify which you have done.</p></small>"""

print '<hr/>'

print """<small><p>If you want to upload one or more ASCII text file instead of using the forms
(especially <b>recommended for challenge 3.1</b>):</p>
<ul><li>Use one line per entry and separating parameter values by spaces or tabs; and list the same
parameters requested in the form, in the same order.</li>
<li>If you wish to use a different parameter ordering,
or add one or more parameters (such as an Index), list the parameters in the first line of the ASCII file;
separate them by spaces and start the line with # (hash).</li>
<li>If you upload multiple tables for multiple sources, specify in Comments field whether the lines
in each file represent multiple maxima for the same source, or multiple sources for different maxima.
(In the second case, the use of an Index field is recommended.)</li></ul></small>"""

print '</div>'

print '<div class="sourceclass" style="background: #ffe;" id="src0">'

# start putting down sources

cntsrc = 0

for src in inputfile.SourceData:
    srcname = src.Name
    srctype = src.SourceType
    
    srcid = ('src0-%s' % cntsrc)
    
    istable = isinstance(src,lisaxml.SourceTable)
    
    print '<div class="entry">'
    
    if istable:
        print '<input type="hidden" name="%s:table" value="True" />' % srcid
        print '<p><b>Table of %s objects</b>:' % srctype,
    else:
        print '<input type="hidden" name="%s:source" value="True" />' % srcid
        print '<p><b>%s object %s</b>:' % (srctype, srcid)
    
    print '<input type="hidden" name="%s:Name" value="%s" /></p>' % (srcid,srcname)
    print '<input type="hidden" name="%s:SourceType" value="%s" />' % (srcid,srctype)

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
            print '<input type="hidden" name="%s:%s" value="None" />' % (srcid,src.Table.parameters[ii])
            print '<input type="hidden" name="%s:%s_Unit" value="%s" />' % (srcid,src.Table.parameters[ii],paramunits[ii])
        
        print '<p><textarea name="%s:Data" rows="10" cols="80">' % srcid
            
        for line in src.Table.Data:
            for val in line:
                print val,
            print
        
        print '</textarea></p>'
    else:    
        print '<table style="table-layout: fixed; width: auto;">'
    
        for param in src.parameters:
            # check that this is not a Min or Max parameter
            
            if ( (srctype == 'ExtremeMassRatioInspiral' and param == 'Polarization') or
                 (srctype == 'BlackHoleBinary' and (param == 'TruncationTime' or param == 'TaperApplied')) or
                 (srctype == 'FastSpinBlackHoleBinary' and (param == 'AmplPNorder' or param == 'TaperApplied')) ):
                break
            
            if hasattr(src,param + '_Unit'):
                paramunit = getattr(src,param + '_Unit')
            else:
                paramunit = ''
        
            paramvalue = getattr(src,param)
                        
            print '<tr>'
            print '<td>%s (%s):</td>' % (param,paramunit)
            print '<td><input type="text" name="%s:%s" size="32" maxlength="255" value="%s" /></td>' % (srcid,param,paramvalue)
            print '<input type="hidden" name="%s:%s_Unit" value="%s" />' % (srcid,param,paramunit)

            # do the minimum value here (with its hidden value)
            
            # do the maximum value here (with its hidden value)
            
            print '</tr>'
        
        print '<tr><td>Probability (relative):</td>'
        print     '<td><input type="text" name="%s:Probability" size="32" maxlength="255" value="1" /></td>' % srcid
        print     '<input type="hidden" name="%s:Probability_Unit" value="1" />' % srcid
        print '</tr>'
        
        print '</table>'
    
    print '</div>'
    
    cntsrc = cntsrc + 1

inputfile.close()

print """<p><a href="#" onClick="duplicate_entry('src0')">Add</a>/<a href="#" onClick="remove_entry('src0')">remove</a> entry</p>"""
print '<p>Alternatively, upload a parameter file (will override all fields): <input type="file" name="src0:file" size="32"/></p>'

print '</div>'

print '<div class="commentclass" id="challengesubmit" style="background: #fef;">'


print '<p><a href="#" onClick="duplicate_source()">Add</a>/<a href="#" onClick="remove_source()">remove</a> source</a></p>'
print '<p><b>Comments</b>:<textarea name="comments" rows="5" cols="80">'
print '</textarea></p>'
print '<input type="submit" name="action" value="Submit">'
print '</div>'

print '</form>'

print '</body></html>'
