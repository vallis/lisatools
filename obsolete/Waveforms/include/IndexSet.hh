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
#define CVSHEADER "$Header: /afs/aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/include/IndexSet.hh,v 1.3 2006/04/21 18:48:34 stba Exp $" 
*/

#ifndef INDEXSETHH
#define INDEXSETHH
namespace LISAWP{ 
    /**Class IndexSet
       A class allow access to  members of matrix class
       @author R. Balasubramanian
    */
    class IndexSet{
    public:
      /** A Constructor to set up the indices based on the arguments
	  @param startIndex The beginning index
	  @param endIndex   The last index
	  @param indexStep  The step size used (can be +ve or negative);
      */
      IndexSet(unsigned startIndex, unsigned endIndex, int indexStep=1);
      /** Function to shift the index set around returns true if valid shift else false */
      bool Shift(int s);

      // declare Matrix to be a friend class
      template<typename T> friend class Matrix;      
    private:
      unsigned lower;
      unsigned upper;
      unsigned length;
      unsigned start;
      int step;
    };   
}//Namespace block ends

#endif

