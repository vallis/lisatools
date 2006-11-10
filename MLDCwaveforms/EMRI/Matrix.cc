/*
Copyright (C) 2001  R. Balasubramanian (Sort by S. Babak)

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
#define CVSHEADER "$Header: /afs/.aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/src/Matrix.cc,v 1.5 2006/05/17 11:03:03 stba Exp $" 
*/

#include "Matrix.hh"
#include <iostream>

namespace LISAWP{
  template<class T>
  Matrix<T>::Matrix():std::vector<T>(0)
  {
    const_origin = origin = 0;
    rows=cols=subR=subC=0;
    offset=0; stride = 0;
    offsetP=0;
  }

  template<class T>
  Matrix<T>::Matrix(const unsigned c):std::vector<T>(c)
  {
    LISAWPAssert(c,"The number of elemenets must be non zero for constructor");  
    unsigned r=1;
    const_origin = origin = &((*this)[0]);
    offset=offsetP=0;
    rows=subR=r;
    cols=subC=c;
    stride = c;
  } 
  template<class T>
  Matrix<T>::Matrix(const unsigned r, const unsigned c):std::vector<T>(r*c)
  {
    LISAWPAssert(r*c,"There product of number of rows and columns must be non zero");	  
    const_origin = origin = &((*this)[0]);
    offset=offsetP=0;
    rows=subR=r;
    cols=subC=c;
    stride = c;
  } 

  template<class T>
  Matrix<T>::Matrix(const Matrix<T> &mat, unsigned nr, unsigned nc, unsigned orX, unsigned orY)
  {
    LISAWPAssert( ((orX+nr) <= mat.subR) && ((orY+nc) <= mat.subC), "Submatrix size inconsistent with parent Matrix");
    rows = mat.subR;
    cols = mat.subC;
    stride = mat.stride;
    offsetP = mat.offset;
    offset = offsetP + orX*stride + orY;
    subR = nr;
    subC = nc;
    const_origin = mat.const_origin + orX*stride + orY;;
    origin=0;    
  }

  template<class T>
  Matrix<T>::Matrix(Matrix<T> &mat, unsigned nr, unsigned nc, unsigned orX, unsigned orY)
  {
    LISAWPAssert(mat.Size() > 0, " you cant attach to a zero size Matrix ");
    LISAWPAssert( ((orX+nr) <= mat.subR) && ((orY+nc) <= mat.subC), "Submatrix size inconsistent with parent Matrix");
    rows = mat.subR;
    cols = mat.subC;
    offsetP = mat.offset;
    stride = mat.stride;
    offset = offsetP + orX*stride + orY;
    subR = nr;
    subC = nc;
    if(mat.origin){
      origin = mat.origin  + orX*stride + orY;
      const_origin=origin;
    }
    else{
      const_origin = mat.const_origin  + orX*stride + orY;
      origin=0;
    }
  }

 
  template<class T>
  Matrix<T>::Matrix(const Matrix<T> &mat):std::vector<T>(mat)

  {
    rows = mat.rows;
    cols = mat.cols;
    stride=mat.stride;
    offset=mat.offset;
    offsetP=mat.offsetP;
    subR=mat.subR;
    subC=mat.subC;
    if(mat.IsSubMatrix()==true){
      origin = mat.origin;
      const_origin = mat.const_origin;
    }
    else{
      origin = &((*this)[0]);
      const_origin=origin;
    }
  }

  template<class T>
  void Matrix<T>::ExpandEmpty( const unsigned r, const unsigned c)
  {
    LISAWPAssert((rows==0)&&(cols==0), " Matrix has to be zero size before it can be expanded");
    LISAWPAssert(r*c,"The rows and columns of the matrix to be expanded cant be zero");
    offset=offsetP=0;
    this->resize(r*c);
    origin=&((*this)[0]);
    const_origin=origin;
    rows=subR=r;
    cols=subC=c;
    stride = c;
  } 

  template<class T>
  void Matrix<T>::ExpandEmpty(const unsigned c)
  {
    unsigned r = 1;
    LISAWPAssert((rows==0)&&(cols==0), " Matrix has to be zero size before it can be expanded");
    offset=offsetP=0;
    this->resize(r*c);
    origin=&((*this)[0]);
    const_origin=origin;
    rows=subR=r;
    cols=subC=c;
    stride = c;
  }

  template<class T>
  void Matrix<T>::MakeEmpty()
  {
    this->resize(0);
    rows=subR=0;
    cols=subC=0;
    offset=0;
    offsetP=0;
    stride = 0;
    const_origin = origin=0;
  } 

  template <class T>
  bool Matrix<T>::ShiftCValid(const int c){
    LISAWPAssert(IsSubMatrix()==true,"You can only shift submatrices");
    unsigned myC = offset % stride;
    unsigned pC = offsetP % stride;
    int cc1 = c + myC + subC;
    int cc2 = myC+c;
    if( (cc1>(int)(pC+cols)) ||(cc2 < (int) pC))
      return false;
    else{
      offset += c; 
      const_origin +=c;
      if(origin)
	origin += c;
      return true;
    }	
  }

  template <class T>
  bool Matrix<T>::ShiftRValid(const int r){
    LISAWPAssert(IsSubMatrix()==true,"You can only shift submatrices");
    unsigned myR = offset/stride;
    unsigned pR = offsetP/stride;
    int rr1 = r + myR + subR;
    int rr2 = r + myR;
    int tmp = rows+pR;
    int tmp2 = pR;
    if( (rr1>tmp) || (rr2 < tmp2) )      
      return false;
    else{
      offset += r*stride;
      const_origin += r*stride;
      if(origin)
	origin += r*stride;
      
      return true;
    }	
  }

  template <class T>
  void Matrix<T>::Attach(Matrix<T> &mat, unsigned nR, unsigned nC, unsigned oX, unsigned oY) 
  {
    LISAWPAssert(this!=&mat,"You cant attach a matrix to itself");
    LISAWPAssert( ((oX+nR) <= mat.subR) && ((oY+nC) <= mat.subC), "Incorrect attachment of matrix");
    LISAWPAssert((rows==0)&&(cols==0), "You can attach only a empty matrix");
    LISAWPAssert(mat.Size()>0, " You cant attach to a zero size Matrix ");
    rows = mat.subR;
    cols = mat.subC;
    stride = mat.stride; 
    offsetP = mat.offset;
    offset = offsetP + oX*stride + oY;
    subR = nR;
    subC = nC;
    if (mat.origin){
      origin = mat.origin + oX*stride + oY;      
      const_origin=origin;
    }
    else{
      const_origin = mat.const_origin + oX*stride + oY;
      origin=0;
    }
  }

  template <class T>
  void Matrix<T>::Attach(const Matrix<T> &mat, unsigned nR, unsigned nC, unsigned oX, unsigned oY) 
  {
    LISAWPAssert(this!=&mat,"You cant attach a matrix to itself");
    LISAWPAssert( ((oX+nR) <= mat.subR) && ((oY+nC) <= mat.subC), "Incorrect attachment of matrix");
    LISAWPAssert((rows==0)&&(cols==0), "You can attach only a empty matrix");
    LISAWPAssert(mat.Size()>0, " You cant attach to a zero size Matrix ");
    rows = mat.subR;
    cols = mat.subC;
    stride = mat.stride; 
    offsetP = mat.offset;
    offset = offsetP + oX*stride + oY;
    subR = nR;
    subC = nC;
    const_origin=mat.const_origin +oX*stride + oY;
    origin = 0;
 }

  template <class T>
  void Matrix<T>::ReAttach(const Matrix<T> &mat, unsigned oX, unsigned oY) 
  {
    LISAWPAssert(IsSubMatrix() == true, " you can reattach only submatrices");
    unsigned nRows = GetNRows();
    unsigned nCols = GetNCols();
    MakeEmpty();
    Attach(mat, nRows, nCols, oX, oY);
  }
  
  template <class T>
  void Matrix<T>::ReAttach(Matrix<T> &mat, unsigned oX, unsigned oY) 
  {
    LISAWPAssert(IsSubMatrix() == true, " you can reattach only submatrices");
    unsigned nRows = GetNRows();
    unsigned nCols = GetNCols();
    MakeEmpty();
    Attach(mat, nRows, nCols, oX, oY); 
  }
  
  template <class T>
  Matrix<T>& Matrix<T>::operator=(const Matrix<T> &mat2)
  {    
    {
      if(IsSubMatrix() == false){
	if((subR!=mat2.GetNRows())||(subC!=mat2.GetNCols())){
	  MakeEmpty();
	  ExpandEmpty(mat2.GetNRows(),mat2.GetNCols());
	}
      }
      LISAWPAssert((subR==mat2.GetNRows())&&(subC==mat2.GetNCols()) , "Matrix Sizes dont match");
      LISAWPAssert(origin, "Write operation attempted on read only submatrix");
      for(unsigned i=0;i<subR;i++){
	iterator i1;
	const_iterator i2;
	i1=origin + i*stride;
	i2=mat2.const_origin + i*mat2.stride;
	for(unsigned j=0;j<subC;j++){
	  *i1=*i2;
	  i1++;
	  i2++;
	}
      }
      return *this;
    }

  }

  template <class T>
  Matrix<T> & Matrix<T>::operator=(const T &var)
  {
    LISAWPAssert(origin, "Write attempt to read only Matrix");
    for(unsigned i=0; i<subR; i++){
      iterator to = origin + i*stride;
      for(unsigned j=0; j<subC;j++)
	*to++ = var;
    }
    return *this;
  }


  template <class T>
  Matrix<T>  Matrix<T>::operator+(const Matrix<T> &mat) const 
  {
    LISAWPAssert((subR==mat.subR)&&(subC==mat.subC)," Matrix sizes must be consistent for addition");
    Matrix<T> out(subR,subC);
    for(unsigned i=0; i<subR; i++){
      const_iterator a1 = const_origin + i*stride;
      const_iterator a2 = mat.const_origin + i * mat.stride;
      iterator a3 = out.origin + i * out.stride;
      for(unsigned j=0; j < subC; j++)
	*a3++ = *a1++ + *a2++;
    }
    return out;
  }

  template <class T>
  Matrix<T> & Matrix<T>::operator+=(const Matrix<T> &mat)
  {

    LISAWPAssert(origin, "Write attempt to read only Matrix");
    LISAWPAssert((subR==mat.subR)&&(subC==mat.subC)," Matrix sizes must be consistent for addition");
    for(unsigned i=0; i<subR; i++){
      iterator to = origin + i*stride;
      const_iterator from = mat.const_origin + i * mat.stride;
      for(unsigned j=0; j<subC;j++)
	*to++ += *from++;
    }
    return *this;
  }  

  template <class T>
  Matrix<T>  Matrix<T>::operator-(const Matrix<T> &mat) const 
  {
    LISAWPAssert((subR==mat.subR)&&(subC==mat.subC)," Matrix sizes must be consistent for addition");
    Matrix<T> out(subR,subC);
    for(unsigned i=0; i<subR; i++){
      const_iterator a1 = const_origin + i*stride;
      const_iterator a2 = mat.const_origin + i * mat.stride;
      iterator a3 = out.origin + i * out.stride;
      for(unsigned j=0; j < subC; j++)
	*a3++ = *a1++  -  *a2++;
    }
    return out;
  }
  

  template <class T>
  Matrix<T> & Matrix<T>::operator-=(const Matrix<T> &mat)
  {

    LISAWPAssert(origin, "Write attempt to read only Matrix");
    LISAWPAssert((subR==mat.subR)&&(subC==mat.subC)," Matrix sizes must be consistent for addition");
    for(unsigned i=0; i<subR; i++){
      iterator to = origin + i*stride;
      const_iterator from = mat.const_origin + i * mat.stride;
      for(unsigned j=0; j<subC;j++)
	*to++ -= *from++;
    }
    return *this;
  } 


  template <class T>
  Matrix<T> Matrix<T>::operator*(const T &var) const
  {
    Matrix<T> out(subR,subC);
    for(unsigned i=0; i<subR; i++){
      const_iterator a1 = const_origin + i*stride;
      iterator a2 = out.origin + i*out.stride;
      for(unsigned j=0; j<subC;j++)
	*a2++ = *a1++ * var;
    }
    return out;
  }

  template <class T>
  Matrix<T> & Matrix<T>::operator*=(const T &var)
  {
    LISAWPAssert(origin, "Write attempt to read only Matrix");
    for(unsigned i=0; i<subR; i++){
      iterator to = origin + i*stride;
      for(unsigned j=0; j<subC;j++)
	*to++ *= var;
    }
    return *this;
  }


  template <class T>
  Matrix<T> Matrix<T>::operator*(const Matrix<T> &mat) const
  {
    LISAWPAssert((subC==mat.subR),"Matrices sizes must be consistent for multiplication");
    Matrix<T> out(subR,mat.subC);
    out.Multiply(*this, mat);
    return out;
  }

  /*void Matrix<double>::Multiply(const Matrix<double> &m1, const Matrix<double> &m2)
  {
    unsigned i,j,k;
    const double *a1,*a2;
    double  *o;
    LISAWPAssert(m1.subC==m2.subR, "Matrix Sizes must be consistent for multiplication");
    LISAWPAssert((subR==m1.subR)&&(subC==m2.subC), "The product matrix is not of right size");
    for(i=0;i<subR;i++){
      o = origin + i*stride;
      a1 = m1.const_origin + i*m1.stride;
      a2 = m2.const_origin;
      for(j=0;j<subC;j++)
	*o++ = *a1 * *a2++;
    }
    for(k=1;k<m1.subC;k++){
      for(i=0;i<subR;i++){
	o = origin + i*stride;
	a1 = m1.const_origin + i*m1.stride  + k;
	a2 = m2.const_origin + k*m2.stride;
	for(j=0;j<subC;j++)
	  *o++ += *a1 * *a2++;
      }
    }    
  }
*/

/** One shold be very carefull calling this function in single 
 * precision, as it might have not enough numerical resolution
 * (dynamical range). It is adviced to use this function with double
 * precision Matricies */
  
  template <class T>
  void Matrix<T>::Multiply(const Matrix<T> &m1, const Matrix<T> &m2)
  {
    Matrix<double> out(subR,subC);
    unsigned i,j,k;
    const_iterator a1,a2;
    double *o;
    LISAWPAssert(m1.subC==m2.subR, "Matrix Sizes must be consistent for multiplication");
    LISAWPAssert((subR==m1.subR)&&(subC==m2.subC), "The product matrix is not of right size");
    for(i=0;i<subR;i++){
      o = out.Origin() + i*out.GetStride();
      a1 = m1.const_origin + i*m1.stride;
      a2 = m2.const_origin;
      for(j=0;j<subC;j++)
	*o++ = (double)*a1 * *a2++;
    }
    for(k=1;k<m1.subC;k++){
      for(i=0;i<subR;i++){
	o = out.Origin() + i*out.GetStride();
	a1 = m1.const_origin + i*m1.stride  + k;
	a2 = m2.const_origin + k*m2.stride;
	for(j=0;j<subC;j++)
	  *o++ += (double)*a1 * *a2++;
      }
    }    
    *this=out;
  }


  template <class T>
  Matrix<int> Matrix<T>::QSort()
  {

    int i,j,k;
    const unsigned int m = 7;
    const unsigned int nstack = 50;
    int indxt, itemp, l=0;  
    int jstack=-1;
    T a;

    LISAWPAssert(subR == 1 || subC == 1, "number of rows or number of columns must be 1");
    unsigned int ir = Size()-1;
    Matrix<int> indx(ir+1);
    for (i=0; i<(int)(ir+1); i++){ 
      indx(i) = i;
    }
    Matrix<int> istack(nstack+1);
    for(;;){
      if(ir-l<m){
	for(j=l+1;j<=int(ir);j++){
	  indxt = indx(j);
	  a = (*this)(indxt);
	  for(i=j-1;i>=l;i--){
	    if((*this)(indx(i)) <= a) break;
	    indx(i+1) = indx(i);
	  }
	  indx(i+1) = indxt;
	}
	if(jstack < 0) break;
	ir = istack(jstack--);
	l = istack(jstack--);
      }
      else{
	k = (l+ir) >> 1;
	itemp = indx(k);
	indx(k) = indx(l+1);
	indx(l+1) = itemp;
	if((*this)(indx(l)) > (*this)(indx(ir))){
	itemp = indx(ir);
	indx(ir) = indx(l);
	indx(l) = itemp;
	}
	if((*this)(indx(l+1)) > (*this)(indx(ir))){
	  itemp = indx(ir);
	  indx(ir) = indx(l+1);
	  indx(l+1) = itemp;
	}
	if((*this)(indx(l)) > (*this)(indx(l+1))){
	  itemp = indx(l);
	  indx(l) = indx(l+1);
	  indx(l+1) = itemp;
	}
	i = l+1;
	j = ir;
	indxt = indx(l+1);
	a = (*this)(indxt);
	for(;;){
	  do i++; 
	  while ((*this)(indx(i)) < a);
	  do j--; 
	  while ((*this)(indx(j)) > a);
	  
	  if (j<i) break;
	  itemp = indx(i);
	  indx(i) = indx(j);
	  indx(j) = itemp;
	}
	indx(l+1) = indx(j);
	indx(j) = indxt;
	jstack +=2;
	LISAWPAssert(jstack <= (int) nstack, "nstack is too small in index sort");
	if((int)(ir-i+1) >= j-l) {
	  istack(jstack) = ir;
	  istack(jstack-1) = i;
	  ir = j-1;
	}
	else{
	  istack(jstack) = j-1;
	  istack(jstack-1) = l;
	  l = i;
	}
      }
    }
    return(indx);
  }


   
  
  template <class T>
  Matrix<T> Matrix<T>::Transpose() const
  {
    Matrix<T> out (subC,subR);
    for(unsigned i=0;i<subC;i++){      
      for(unsigned j=0; j < subR; j++){
	out(i,j) = (*this)(j,i);
      }
    }
    return out;
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &ir, const IndexSet &ic) const
  {
    LISAWPAssert((ir.upper<subR)&&(ic.upper<subC), "Index Set not of proper sizes ");
    LISAWPAssert((ir.step==1)&&(ic.step==1), "Index Sets not contiguous");
    Matrix<T> out;
    out.Attach(*this, ir.length, ic.length, ir.lower, ic.lower);
    return out;
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &ir, const IndexSet &ic) 
  {
    LISAWPAssert((ir.upper<subR)&&(ic.upper<subC), "Index Set not of proper sizes ");
    LISAWPAssert((ir.step==1)&&(ic.step==1), "Index Sets not contiguous");
    Matrix<T> out;
    out.Attach(*this, ir.length, ic.length, ir.lower, ic.lower);
    return out;
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &iSet) const
  {
    LISAWPAssert(iSet.step==1, "Index Set not contiguous");
    LISAWPAssert((subR==1) || (subC==1), "The matrix must be single column or row");
    if(subR==1){
      LISAWPAssert(iSet.upper < subC,"Index Set not of proper sizes ");
      Matrix<T> out;
      out.Attach(*this, 1, iSet.length, 0, iSet.lower);
      return out;
    }
    else{
      LISAWPAssert(iSet.upper < subR,"Index Set not of proper sizes ");
      Matrix<T> out;
      out.Attach(*this, iSet.length,  1,  iSet.lower, 0);    
      return out;
    }
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &iSet)
  {

    LISAWPAssert(iSet.step==1, "Index Set not contiguous");
    LISAWPAssert((subR==1) || (subC==1), "The matrix must be single column or row");
    if(subR==1){
      LISAWPAssert(iSet.upper < subC,"Index Set not of proper sizes ");
      Matrix<T> out;
      out.Attach(*this, 1, iSet.length, 0, iSet.lower);
      return out;
    }
    else{
      LISAWPAssert(iSet.upper < subR,"Index Set not of proper sizes ");
      Matrix<T> out;
      out.Attach(*this, iSet.length,  1,  iSet.lower, 0);    
      return out;
    }
  }


  template <class T>
  Matrix<T> Matrix<T>::operator()(unsigned r,const IndexSet &iSet) const
  {
    LISAWPAssert(iSet.step==1, "Index Set not contiguous");
    LISAWPAssert(r<subR,"illegal Row");
    LISAWPAssert(iSet.upper < subC, "Index Set not of proper sizes ");
    Matrix<T> out;
    out.Attach(*this, 1, iSet.length, r, iSet.lower);
    return out;
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(unsigned r,const IndexSet &iSet)
  {
    LISAWPAssert(iSet.step==1, "Index Set not contiguous");
    LISAWPAssert(r<subR,"illegal Row");
    LISAWPAssert(iSet.upper < subC,"Index Set not of proper sizes " );
    Matrix<T> out;
    out.Attach(*this, 1, iSet.length, r, iSet.lower);
    return out;
  }


  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &iSet, unsigned c) const
  {
    LISAWPAssert(iSet.step==1, "Index Set not contiguous");
    LISAWPAssert(c<subC,"illegal column");
    LISAWPAssert(iSet.upper < subR, "Index Set not of proper sizes");
    Matrix<T> out;
    out.Attach(*this, iSet.length, 1, iSet.lower, c);
    return out;
  }

  template <class T>
  Matrix<T> Matrix<T>::operator()(const IndexSet &iSet, unsigned c) 
  {
    LISAWPAssert(iSet.step==1,"Index Set not contiguous");
    LISAWPAssert(c<subC,"Illegal Column");
    LISAWPAssert(iSet.upper < subR, "Index Set not of proper sizes");
    Matrix<T> out;
    out.Attach(*this, iSet.length, 1, iSet.lower, c);
    return out;
  }
  
  template <class T>
  void Matrix<T>::SetValue(const IndexSet &iRow, const IndexSet &iCol, const T &val)
  {
    unsigned r,c;
    LISAWPAssert((iRow.upper < subR)&&(iCol.upper < subC),"Index Set Sizes are not correct");
    r = iRow.start;
    for(unsigned i=0;i<iRow.length;i++){
      c = iCol.start;
      for(unsigned j=0;j<iCol.length;j++){
	*(origin + r*stride + c) = val;
	c += iCol.step;
      }
      r += iRow.step;
    }
  }

  template <class T>
  void Matrix<T>::SetValue(const IndexSet &iRow, const IndexSet &iCol, const Matrix<T> &mat)
  {
    unsigned r,c;
    LISAWPAssert((iRow.upper < subR)&&(iCol.upper < subC), "Index Set Sizes are not correct");
    LISAWPAssert((iRow.length == mat.subR) && ( iCol.length == mat.subC),"Index Set Sizes are not correct");
    r = iRow.start;
    for(unsigned i=0;i<iRow.length;i++){
      c = iCol.start;
      for(unsigned j=0;j<iCol.length;j++){
	*(origin + r*stride + c) = *(mat.const_origin + i*mat.stride + j);
	c += iCol.step;
      }
      r += iRow.step;
    }
  }

  template <class T>
  Matrix<T> Matrix<T>::GetSubMatrix(const IndexSet &iRow, const IndexSet &iCol) const 
  {
    unsigned r,c;
    LISAWPAssert((iRow.upper < subR)&&(iCol.upper < subC),"Index Set Sizes are not correct");
    Matrix<T> out(iRow.length, iCol.length);
    r = iRow.start;
    for(unsigned i=0;i<iRow.length;i++){
      c = iCol.start;
      for(unsigned j=0;j<iCol.length;j++){	
	*(out.origin + i*out.stride + j ) = *(const_origin + r*stride + c);
	c += iCol.step;
      }
      r += iRow.step;
    }
    return out;
  }


  template <class T>
  void Matrix<T>::MultiplyT(const Matrix<T> &m1, const Matrix<T> &m2)
  {
    LISAWPAssert(m1.subC == m2.subC,"Matrix Sizes not consistent");
    LISAWPAssert((subR==m1.subR)&&(subR==m2.subR), "Output matrix not of correct size");
    for(unsigned i=0; i<subR; i++){
      for(unsigned j=0;j<subC; j++){
	const_iterator i1 = m1.const_origin + i*m1.stride;
	const_iterator i2 = m2.const_origin + j*m2.stride;
	double sum=0;
	for(unsigned k=0;k < m1.subC; k++)
	  sum += (*i1++ * *i2++);
	*(origin + i*stride + j) = static_cast<T> (sum);
      }
    }
  }

  template <class T>
  double Matrix<T>::NormS() const
  {
    const_iterator it;
    double sum=0.0;
    for(unsigned i=0;i<subR;i++){
      it = const_origin + i*stride;
      for(unsigned j=0;j<subC; j++){
	sum += (*it * *it);
	it++;
      }
    }
    return sum;
  }


  template <class T>
  double Matrix<T>::Sum() const
  {
    const_iterator it;
    double sum=0;
    for(unsigned i=0;i<subR;i++){
      it = const_origin + i*stride;
      for(unsigned j=0;j<subC; j++){
	sum += *it;
	it++;
      }
    }
    return sum;
  }

  template <class T>
  double Matrix<T>::Rms() const
  {
    if(Size()==1) 
      return 0.0;
    const_iterator it;
    double sum=0;
    for(unsigned i=0;i<subR;i++){
      it = const_origin + i*stride;
      for(unsigned j=0;j<subC; j++){
	sum += *it;
	it++;
      }
    }
    double mean = sum/(subR*subC);
    sum = 0.0;
    for(unsigned i=0;i<subR;i++){
      it = const_origin + i*stride;
      for(unsigned j=0;j<subC; j++){
	sum += (*it - mean) * (*it - mean);
	it++;
      }
    }
    sum /= (subR*subC - 1.0);
    return sqrt(sum);    
  }

  template <class T>
  int Matrix<T>::MinIndex() const
  {
    LISAWPAssert(Size(),"The minimum function can be called only for a non zero-size matrix");
    LISAWPAssert(subR==1||subC==1,"The minimum function can be called only for a row or column matrix");
    if(subR==1){
      T min = *const_origin;
      int ind = 0;
      for(unsigned j=1;j<Size();j++)
	if(min > *(const_origin + j)){
	  ind = j;
	  min = *(const_origin + j);
	}
      return ind;  
    }
    else{
      T min = *const_origin;
      int ind = 0;
      for(unsigned j=1;j<Size();j++)
	if(min > *(const_origin + j*stride)){
	  ind = j;
	  min = *(const_origin + j*stride);
	}
      return ind;  
    }
  }


  template <class T>
  int Matrix<T>::MaxIndex() const
  {
    LISAWPAssert(Size(),"The maximum function can be called only for a non zero-size matrix");
    LISAWPAssert(subR==1||subC==1,"The maximum function can be called only for a row or column matrix");
    if(subR==1){
      T max = *const_origin;
      int ind = 0;
      for(unsigned j=1;j<Size();j++)
	if(max < *(const_origin + j)){
	  ind = j;
	  max = *(const_origin + j);
	}
      return ind;  
    }
    else{
      T max = *const_origin;
      int ind = 0;
      for(unsigned j=1;j<Size();j++)
	if(max < *(const_origin + j*stride)){
	  ind = j;
	  max = *(const_origin + j*stride);
	}
      return ind;  
    }
  }


  template <class T>
  Matrix<T>::~Matrix()
  {
    this->resize(0);
    return;
  }

  // Template Instantiations
  template class Matrix<float>;
  template class Matrix<double>;
  template class Matrix<int>;
  template class Matrix<unsigned char>;

} // namespace LISAWP ends
