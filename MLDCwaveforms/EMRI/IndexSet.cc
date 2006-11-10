/*
Copyright (C) 2001  R. Balasubramanian

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/******************  CVS info ************************ 
#define CVSTAG "$Name:  $"
#define CVSHEADER "$Header: /afs/aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/src/IndexSet.cc,v 1.3 2006/04/21 18:40:08 stba Exp $" 
*/

#include "IndexSet.hh"
#include "Macros.hh"

namespace LISAWP {
  IndexSet::IndexSet(unsigned s, unsigned e, int d)
  {
    if(e==s){
      lower=upper=s;
      length = 1;
      start = s;
      step=0;
      return;
    }
    if(s<e){      
      LISAWPAssert(d>0,"the step must be positive");
      start =s;
      lower=s;
      step = d;
      length=0;
      unsigned element = s;
      while(element<=e){
	length++;
	upper=element;
	element+=d;
      }
      return;
    }
    else{
      LISAWPAssert(d<0," The step must be negative");
      unsigned element = s;
      step=d;
      start = s;
      upper=s;
      length=0;
      while(element>=e){
	length++;
	lower=element;
	element+=d;
      }
    }
    return;
  }
  bool IndexSet::Shift(int s)
  {
    if(s==0)
      return true;
    int l = lower;
    if((s<0)&&(l+s<0))
      return false;
    lower+=s;
    upper+=s;
    start+=s;
    return true;
  }

}// namespace ends;
