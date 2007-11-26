#!/usr/bin/env python

# -> setup modules

import sys
import cgi
import os
import re
import time

sys.stderr = sys.stdout
print "Content-type: text/html\n"

# -> setup parameters
datadir = '/home/mldc/CHALLENGE1B/'
htmldir = 'http://www.tapir.caltech.edu/~mldc/cgi-bin/edit-lisaxml.cgi'

# -> parse some of the form
data = cgi.FieldStorage()

shortcollaboration = data['shortcollaboration'].value
submit = data['action'].value

collaboration = data['collaboration'].value
authors = data['authors'].value

if not shortcollaboration or not re.match('[a-zA-Z0-9]*$',shortcollaboration):
    print "I need the short collaboration name as an alphanumeric-only string!"
    sys.exit(0)

if 'MLDC' in submit:
    submitchallenge = submit.split()[1]
    
    # NEED TO DO SEPARATE CASE FOR SINGLE GALACTIC BINARY
    if '1B.1.1' in submit:
        viewpage = htmldir + '?filename=GalacticBinary-key.xml&challenge=%s&collaboration=%s' % (submitchallenge,shortcollaboration)
    elif '1B.1' in submit:
            viewpage = htmldir + '?filename=Galaxy-key.xml&challenge=%s&collaboration=%s' % (submitchallenge,shortcollaboration)
    elif '1B.2' in submit:
        viewpage = htmldir + '?filename=SMBH-key.xml&challenge=%s&collaboration=%s'   % (submitchallenge,shortcollaboration)
    elif '1B.3' in submit:
        viewpage = htmldir + '?filename=EMRI-key.xml&challenge=%s&collaboration=%s'   % (submitchallenge,shortcollaboration)
elif 'write-up' in submit:
    viewpage = 'writeup'

# now save the collaboration name and authors to a text file

notefile = open(datadir + shortcollaboration + '-id.txt','w')

notefile.write('Collaboration id:\n')
notefile.write(shortcollaboration)
notefile.write('\n\n')

notefile.write('Collaboration name:\n')
notefile.write(collaboration)
notefile.write('\n\n')

notefile.write('Collaboration authors:\n')
notefile.write(authors)
notefile.write('\n')

notefile.close()

# -> output HTML to the browser

if viewpage == 'writeup':
    if data.has_key('writeup') and data['writeup'].filename:
        if '.tar.gz' in data['writeup'].filename:
            suffix = '.tar.gz'
        elif '.tar' in data['writeup'].filename:
            suffix = '.tar'
        elif '.tgz' in data['writeup'].filename:
            suffix = '.tar.gz'
        elif '.zip' in data['writeup'].filename:
            suffix = '.zip'
        else:
            suffix = '.file'
            
        datedwriteup = datadir + shortcollaboration + '-writeup-' + time.strftime('%y%m%d-%H%M%S',time.localtime()) + suffix
        
        outputfile = open(datedwriteup,'w')
        
        for line in data['writeup'].file:
            outputfile.write(line)
            
        outputfile.close()
        
        print "Thanks! I've received file %s. Now you can go back and upload your recovered parameters (if you haven't already)." % data['writeup'].filename
else:
    print '<html><head><meta http-equiv="refresh" content="0; URL=%s"/></head></html>' % viewpage   
