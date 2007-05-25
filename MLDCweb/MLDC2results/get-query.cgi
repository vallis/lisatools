#!/usr/bin/env python

# -> setup modules

import sys
import cgi
import os
import re

sys.stderr = sys.stdout
print "Content-type: text/html\n"

# -> setup parameters
datadir = '/home/mldc/'
htmldir = '/newman/user3/mldc/public_html/'

# -> parse some of the form
data = cgi.FieldStorage()

shortcollaboration = data['shortcollaboration'].value
submit = data['action'].value

collaboration = data['collaboration'].value
authors = data['authors'].value

if not shortcollaboration or not re.match('[a-zA-Z0-9]*$',shortcollaboration):
    print "I need the short collaboration name as an alphanumeric-only string!"
    sys.exit(0)

if '1.3' in submit:
    viewpage = htmldir + 'submit-emri.html'
elif '2.1' in submit:
    viewpage = htmldir + 'submit-galaxy.html'
elif '2.2' in submit:
    viewpage = htmldir + 'submit-enchilada.html'

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

for line in open(viewpage,'r'):
    print re.sub('%shortcollaboration',shortcollaboration,line),
