#!/usr/bin/env python

__version__='$Id: $'

import lisaxml

from optparse import OptionParser

# note that correct management of the Id string requires issuing the command
# svn propset svn:keywords Id FILENAME

parser = OptionParser(usage="usage: %prog [options] XMLFILES... will create empty lisaXML files",
                      version="$Id:  $")

parser.add_option("-a", "--author",
                  type="string", dest="author", default='MLDC task force',
                  help="set author field [default: 'MLDC task force']")

parser.add_option("-c", "--comment",
                  type="string", dest="comment", default='',
                  help="set comment [default: none]")

(options, args) = parser.parse_args()

if len(args) < 1:
    parser.error("Need at least one file to touch/reset!")

for filename in args:
    lisaxml.lisaXML(filename,author=options.author,comments=options.comment).close()
