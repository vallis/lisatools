#!/usr/bin/python
##
## Copyright (C) 2007 John T Whelan, Reinhard Prix
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with with program; see the file COPYING. If not, write to the 
##  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
##  MA  02111-1307  USA
##

from pylab import *
text.usetex = True
from matplotlib import rc
rc('text', usetex=True)
from scipy import unwrap
from scipy.special import erfinv
from scipy.special import erf
from optparse import *

## ----- function definitions

def mapProjY(lat,lon):
    mysign = sign(lat);
    # Change 0 values to 1
    mysign = sign(mysign + 0.1);
    return 0.5 * cos(lat) * sin(lon);

def mapProjX(lat,lon):
    mysign = sign(lat);
    # Change 0 values to 1
    mysign = sign(mysign + 0.1);
    return mysign * 0.5 * ( mysign * cos(lat) * cos(lon) - 1.3 );

def rawProjY(r,lon):
    mysign = sign(r);
    return 0.5 * abs(r) * sin(lon);

def rawProjX(r,lon):
    mysign = sign(r);
    return mysign * 0.5 * ( mysign * abs(r) * cos(lon) - 1.3 );

def radstr(deg):
    if deg == 0:
        rs = '$0$';
    else:
        divi = gcd(abs(deg),180);
        denom = 180/divi;
        numer = deg/divi;
        if denom == 1:
            if numer == 1:
                rs = '$\pi$'
            elif numer == -1:
                rs = '$-\pi$'
            else:
                rs = '$%d\pi$' % numer;
        elif numer == 1:
            rs = '$\pi/%d$' % denom;
        elif numer == -1:
            rs = '$-\pi/%d$' % denom;
        else:
            rs = '$%d\pi/%d$' % (numer,denom);
    return rs

def gcd(a, b):
    if a > b: a, b = b, a
    while a > 0: a, b = (b % a), a
    return b
lcm = lambda a, b: a * b / gcd(a, b)

# ----- Begin script
usage = usage = """\
skyMap.py [options]

  Make a sky map

  ----------------------------------------------------------------------------
  --cand-file:   Not actually read in, but its name is used to deduce
                 the names of other files  
  --out-prefix:  Prefix for generated figures
  --extra-title: Extra text for title of figures
  --min-freq:    Lower frequency for plots
  --max-freq:    Upper frequency for plots
  ----------------------------------------------------------------------------   
"""

##############################################################################
parser = OptionParser( usage=usage, version= "%prog CVS $Id: skyMap.py,v 1.2 2007/09/13 12:45:05 whelan Exp $")

# used letters:
# fiot
# F

parser.add_option("-i","--cand-file",action="store",type="string",
                  dest="resname");

parser.add_option("-o","--out-prefix",action="store",type="string",
                  dest="outname");

parser.add_option("-t","--extra-title",action="store",type="string",
                  dest="xtit");

parser.add_option("-f","--min-freq",action="store",type="float",
                  dest="minfreq");

parser.add_option("-F","--max-freq",action="store",type="float",
                  dest="maxfreq");

(opts,args) = parser.parse_args();
resname = opts.resname;

try:
    slind = resname.rindex('/');
    resdir = resname[:slind];
    bname = resname[(slind+1):];
except ValueError:
    resdir = '.';
    bname = resname;

if ( opts.outname ):
    outname = opts.outname;
else:
    try:
        dind = bname.rindex('.dat');
        outname = bname[:dind];
    except ValueError:        
        outname = bname;

if ( opts.xtit ):
    xtit = opts.xtit + ' ';
else:
    xtit = '';

if ( opts.minfreq ):
    minfreq = opts.minfreq;
else:
    minfreq = 0;

if ( opts.maxfreq ):
    maxfreq = opts.maxfreq;
else:
    maxfreq = 20;

# ----- challenge 2.1
falsea = load(resdir + '/FALSE_' + bname, comments="%");

missed = load(resdir + '/MISSED40_' + bname, comments="%");

matched = load(resdir + '/MATCHED_' + bname, comments="%");


matchedCand = matched[arange(0,shape(matched)[0],2),:];
matchedKey = matched[arange(1,shape(matched)[0],2),:];

matchedInBand = find( ( matchedCand[:,0] >= minfreq/1e3 )
                      & ( matchedCand[:,0] <= maxfreq/1e3 ) );

sys.stdout.write( "%d found sources with %.3fmHz < %.3fmHz\n"
                  % (shape(matchedInBand)[0],minfreq,maxfreq) );

matchedCand = matchedCand[matchedInBand,:];
matchedKey = matchedKey[matchedInBand,:];

missedInBand = find( ( missed[:,0] >= minfreq/1e3 )
                      & ( missed[:,0] <= maxfreq/1e3 ) );

sys.stdout.write( "%d missed sources with %.3fmHz < %.3fmHz and 2F > 40\n"
                  % (shape(missedInBand)[0],minfreq,maxfreq) );

missed = missed[missedInBand,:];

falseaInBand = find( ( falsea[:,0] >= minfreq/1e3 )
                      & ( falsea[:,0] <= maxfreq/1e3 ) );

sys.stdout.write( "%d false alarms with %.3fmHz < %.3fmHz\n"
                  % (shape(falseaInBand)[0],minfreq,maxfreq) );

falsea = falsea[falseaInBand,:];

freqsmHz = matchedCand[:,0]*1e3;

Braun = [.5, .2, .2];

## ----- skyplot
figure(figsize=(6,3));
# axes([0.1,0.9-0.1,0.1,0.9-0.1]);
alphacirc = arange(0,2*pi+pi/100,pi/100);
alpha0 = 0;
missedX = mapProjX(missed[:,1],missed[:,2]-alpha0);
missedY = mapProjY(missed[:,1],missed[:,2]-alpha0);
matchedKeyX = mapProjX(matchedKey[:,1],matchedKey[:,2]-alpha0);
matchedKeyY = mapProjY(matchedKey[:,1],matchedKey[:,2]-alpha0);
# ghostKeyX = mapProjX(-matchedKey[:,1],matchedKey[:,2]-alpha0);
# ghostKeyY = mapProjY(-matchedKey[:,1],matchedKey[:,2]-alpha0);
# matchedCandX = mapProjX(matchedCand[:,1],matchedCand[:,2]-alpha0);
# matchedCandY = mapProjY(matchedCand[:,1],matchedCand[:,2]-alpha0);
# ghostCandX = mapProjX(-matchedCand[:,1],matchedCand[:,2]-alpha0);
# ghostCandY = mapProjY(-matchedCand[:,1],matchedCand[:,2]-alpha0);
plot ( missedX, missedY, 'rx');
plot ( matchedKeyX, matchedKeyY, 'b+');
if size(falsea):
    falseaX = mapProjX(falsea[:,1],falsea[:,2]-alpha0);
    falseaY = mapProjY(falsea[:,1],falsea[:,2]-alpha0);
    plot ( falseaX, falseaY,'ro', mfc='None', mec=Braun);
if size(falsea):
    legend(['missed (key)','found (key)','false (cand)'],loc=(0.37,-0.13),numpoints=1);
else:
    legend(['missed (key)','found (key)'],loc=(0.37,-0.13),numpoints=1);
# North and South Poles
plot([mapProjX(pi/2,0),mapProjX(-pi/2,0)],
     [mapProjY(pi/2,0),mapProjY(-pi/2,0)],'k+');
# Equator
x = mapProjX(1e-6,alphacirc);
y = mapProjY(1e-6,alphacirc);
plot(x,y,'k-',linewidth=2);
x = mapProjX(-1e-6,alphacirc);
y = mapProjY(-1e-6,alphacirc);
plot(x,y,'k-',linewidth=2);
# Parallels of latitude
for delta in arange(pi/6,pi/2,pi/6):
    x = mapProjX(delta,alphacirc);
    y = mapProjY(delta,alphacirc);
    plot(x,y,'k:');
    x = mapProjX(-delta,alphacirc);
    y = mapProjY(-delta,alphacirc);
    plot(x,y,'k:');
# Lines of longitude
for alphadeg in arange(0,360,30):
    alpha = alphadeg * pi / 180.;
    r = arange(1e-6,2,1.1);
    x = rawProjX(r,alpha-alpha0);
    y = rawProjY(r,alpha-alpha0);
    plot(x,y,'k:');
    r = arange(1e-6,2,1.1);
    x = rawProjX(-r,alpha-alpha0);
    y = rawProjY(-r,alpha-alpha0);
    plot(x,y,'k:');    
    if (remainder(alphadeg,180) and not remainder(alphadeg,90)):
        x = rawProjX(1.2,alpha-alpha0);
        y = rawProjY(1.2,alpha-alpha0);
        text(x,y,radstr(alphadeg),
             horizontalalignment='center',verticalalignment='center');
        x = rawProjX(-1.2,alpha-alpha0);
        y = rawProjY(-1.2,alpha-alpha0);
        text(x,y,radstr(alphadeg),
             horizontalalignment='center',verticalalignment='center');

x = rawProjX(1.2,pi);
y = rawProjY(1.2,pi);
text(x,y,'$\lambda=\pi$',
     horizontalalignment='center',verticalalignment='center');
x = rawProjX(-1.1,pi);
y = rawProjY(-1.1,pi);
text(x,y,'$\pi$',
     horizontalalignment='center',verticalalignment='center');
x = rawProjX(1.1,0);
y = rawProjY(1.1,0);
text(x,y,'$0$',
     horizontalalignment='center',verticalalignment='center');
x = rawProjX(-1.1,0);
y = rawProjY(-1.1,0);
text(x,y,'$0$',
     horizontalalignment='center',verticalalignment='center');
x = rawProjX(1.5,pi);
y = rawProjY(1.5,pi);
text(x,y,'Northern Hemisphere',
     horizontalalignment='center',verticalalignment='center',rotation=90);
x = rawProjX(-1.5,0);
y = rawProjY(-1.5,0);
text(x,y,'Southern Hemisphere',
     horizontalalignment='center',verticalalignment='center',rotation=-90);
title (xtit + "Challenge 2.1 Sky Positions");
axis([-1.3,1.3,-0.55,0.55]);
axis('equal');
axis('off');
savefig(outname + 'skyMap.eps');
savefig(outname + 'skyMap.png');
