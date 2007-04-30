#!/usr/bin/env python

import sys
import os
import re

def run(s):
    os.system(s % globals())

if len(sys.argv) < 2:
    print "Use %s ARCHIVE.tar.gz to replace the archive, fixing the time axis and removing stray nonoise files" % sys.argv[0]
    sys.exit(0)

archive = sys.argv[1]

if '.tar.gz' in archive:
    pass
elif '.tar' in archive:
    run('gzip %(archive)s')
    archive = archive + '.gz'
else:
    print "Please give filename of tarred or tarred-gzipped archive"
    sys.exit(0)

challengename = os.path.basename(re.sub('\.tar\.gz','',archive))
backup = re.sub('\.tar\.gz','-backup.tar.gz',archive)
backupxml = re.sub('\.tar\.gz','-backup.xml',archive)

prefix = os.path.dirname(__file__)
if prefix:
    prefix = prefix + '/'

# untar and make a copy
run('mkdir %(challengename)s')
run('tar zxvf %(archive)s -C %(challengename)s')
run('mv %(archive)s %(backup)s')

# fix the primary binary
run('%(prefix)sfixbinary.py %(challengename)s/%(challengename)s-0.bin %(challengename)s/%(challengename)s-0.bin')

# fix the EMRI entries in the key file
run('mv %(challengename)s/%(challengename)s.xml %(challengename)s/%(challengename)s.xml.old')
run('%(prefix)sfixemri2.py %(challengename)s/%(challengename)s.xml.old %(challengename)s/%(challengename)s.xml')
run('mv %(challengename)s/%(challengename)s.xml.old %(backupxml)s')

# remove the spurious binary if present
if os.path.isfile('%(challengename)s/%(challengename)s-nonoise-0.bin' % globals()) and ('nonoise' not in challengename):
    run('rm %(challengename)s/%(challengename)s-nonoise-0.bin')

# tar up the directory and remove it
run('tar zcvf %(archive)s %(challengename)s')
run('rm -rf %(challengename)s')
