/*
Copyright (C) 2001, 2005 R.Balasubramanian & S. Babak

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
#define CVSHEADER "$Header: /afs/.aei-potsdam.mpg.de/u/stba/cvs/LISA/LISAWP/base/include/Matrix.hh,v 1.5 2006/05/17 11:03:03 stba Exp $" 
*/

#ifndef MATRIXHH
#define MATRIXHH
#include <vector>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include "IndexSet.hh"
#include "Macros.hh"

namespace LISAWP{

  /// Class Matrix: The Basic DataContainer for LISAWP

  /** Class Matrix
      @author R. Balasubramanian, (S. Babak for sort,... method)

      This is the basic datatype for all computations. Time series and Frequency series will be 
      stored as rows in the matrix. We implement the Matrix as a template class and we
      explicitly instantiate Matrix<float>, Matrix<double> and Matrix<int>. 

      The most important feature of this class which distinguishes this class from most other
      Matrix classes is that this Matrix class doubles as both a container of data as well as
      a {\em smart} Pointer to a submatrix of another matrix. Thus a certain amount of care is
      needed in using this matrix. The advantage is that the the  Submatrix and Matrix operations
      are identical.

  */
  template <class T> class Matrix:protected std::vector<T>{
  public:
    /**
       Default Constructor; Creates an empty matrix
    */
    Matrix();
    /**
       Constructor of a r by c matrix
       @param r The number of rows
       @param c The Number of columns       
    */
    Matrix(unsigned r, unsigned c); 
    /**
       Constructor for a row vector ; assumes rows=1;
       @param c The Number of columns
    */
    explicit Matrix(unsigned c);
    /** Copy constructor 
     Please be aware that the copy constructor does not necessarily construct a matrix independent
    of the original Matrix. If the original matrix is a submatrix, it creates a copy of the submatrix
    and if it was a matrix then it creates a duplicate copy and that 2 matrices are truly independent*/
    Matrix(const Matrix<T> &);
    /** A constructor specifically for a constant Submatrix. Please note that an attempt to change the 
	contents of submatrix of a constant matrix yields a runtime error and not a compile time error.
	@param mat A matrix or submatrix to attach to
	@param nr  The row size of this submatrix
	@param nc  the col size of this submatrix
	@param orX The row position of origin to submatrix
	@param orY the col position of origin to submatrix
    */
    Matrix(const Matrix<T> &mat, unsigned nr, unsigned nc, unsigned orX=0, unsigned orY=0);
    /** A constructor specifically for attaching one matrix to another. The constructed matrix
	is a submatrix of the parent matrix with the following properties
	@param mat A matrix or submatrix to attach to
	@param nr  The row size of this submatrix
	@param nc  the col size of this submatrix
	@param orX The row position of origin to submatrix
	@param orY the col position of origin to submatrix
    */
    Matrix(Matrix<T> &mat, unsigned nr, unsigned nc, unsigned orX=0, unsigned orY=0);
    /** Typedef for a Matrix<T>::iterator */
    typedef T *iterator;
    /** Typedef for an Matrix<T>::const_iterator for read only matrices */
    typedef const T *const_iterator;
    /// Get Number of Rows
    inline unsigned GetNRows() const { return subR;}
    /// Get Number of Cols
    inline unsigned GetNCols() const { return subC;}
    /// Return the stride 
    inline unsigned GetStride() const { return stride;}
    /// Return Size of matrix/Submatrix
    inline unsigned Size() const { return subC*subR;} 
    /// Return an iterator to the begining of the Matrix/Submatrix
    inline iterator Origin() const { return origin;};
    /// Return an const_iterator to the begining of the Matrix/Submatrix
    inline const_iterator ConstOrigin() const { return const_origin;};
    /// Return an iterator to the end of the Matrix/Submatrix
    inline iterator End() const { return origin + (subR-1)*stride + subC;}
    /// Return an const_iterator to the end of the Matrix/Submatrix
    inline const_iterator ConstEnd() const { return const_origin + (subR-1)*stride + subC;}
    /** Return an iterator to the beginning of ever row
	@param i The row number
     */
    inline iterator RowOrigin(unsigned i) const {
      LISAWPAssert(i<subR,"Row number Incorrect");
      LISAWPAssert(origin,"This is a const matrix");
      return (origin + i *stride);}
    /** Return an iterator to just beyond the end  of every row
	@param i The row number
     */
    inline iterator RowEnd(unsigned i) const {
      LISAWPAssert(i<subR, "Row number incorrect");
      LISAWPAssert(origin,"This is a const matrix");
      return (origin + i *stride+subC);}
    /** Return an const iterator to the beginning of ever row
	@param i The row number
     */
    inline const_iterator ConstRowOrigin(unsigned i) const {
      LISAWPAssert(i<subR, "Row number incorrect");
      return (const_origin + i*stride);}
    /** Return an const iterator to just beyond the end  of every row
	@param i The row number
    */
    inline const_iterator ConstRowEnd(unsigned i) const {
      LISAWPAssert(i<subR, "Row number incorrect");
      return (const_origin + i *stride+subC);}
    /** Resize Matrix. The matrix must be initially empty and it must NOT be a submatrix 
     @param r the number of rows
     @param c the number of cols
    */
    void ExpandEmpty(const unsigned r, const unsigned c);
    /** Resize the Matrix  for a row matrix.  The matrix must be initially empty and it must NOT be a submatrix 
     @param c The number of cols */
    void ExpandEmpty(const unsigned c);
    /**  Make the matrix NULL*/
    void MakeEmpty();
    /// Return true if it is a submatrix and false otherwise
    inline bool IsSubMatrix() const { 
      if(!(this->size()) && (Size())) return (true); else return false;}
    /** Shift the SubMatrix by so many columns; return true if valid else false 
	@param c the number of columns to shift 
    */
    bool ShiftCValid(const int c);
    /** Shift the SubMatrix by so many rows; return true if valid else false
	@param r the number of rows to shift 
    */
    bool ShiftRValid(const int r);    
    /** Attach this matrix as a submatrix of the CONSTANT argument matrix
	@param mat  A reference to a matrix of which this is a submatrix
	@param unsigned nR The number of rows of sub Matrix
	@param unsigned nC The number of cols of sub Matrix
	@param unsigned oX The row (default 0) where the submatrix is located
	@param unsigned oY The column (default 0) where the submatrix is located	
    */
    void  Attach(const Matrix<T> &mat, unsigned nR, unsigned nC, unsigned oX=0, unsigned oY=0);
     /** Attach this matrix as a submatrix of the argument matrix
	@param mat  A reference to a matrix of which this is a submatrix
	@param unsigned nR The number of rows of sub Matrix
	@param unsigned nC The number of cols of sub Matrix
	@param unsigned oX The row (default 0) where the submatrix is located
	@param unsigned oY The column (default 0) where the submatrix is located	
    */
    void  Attach(Matrix<T> &mat, unsigned nR, unsigned nC, unsigned oX=0, unsigned oY=0); 
    /** Re Attach this Sub matrix as a submatrix of the another CONSTANT matrix
	(read only)
	@param mat  A reference to a matrix of which this is a submatrix
	@param unsigned oX The row where the submatrix is located
	@param unsigned oY The column where the submatrix is located	
    */
    void  ReAttach(const Matrix<T> &mat, unsigned oX, unsigned oY); 
    /** Re Attach this Sub matrix as a submatrix of the another matrix
	@param mat  A reference to a matrix of which this is a submatrix
	@param unsigned oX The row where the submatrix is located
	@param unsigned oY The column where the submatrix is located	
    */
    void  ReAttach(Matrix<T> &mat, unsigned oX, unsigned oY);
    /** Conversion operator.   Conversion function from single element Matrix to T
     */
    operator T() const {
      LISAWPAssert(Size()==1, "Matrix Size is not unity");
      return *const_origin;}
    /** Templated function to compute a particular function on
	each element of a matrix 
	@param func A pointer to a function which returns double/float/int and takes input double/float/int respectively 
    */
    template <class ARGTYPE>
    void Compute(ARGTYPE (*func)(ARGTYPE)){
      LISAWPAssert(origin,"Attempt to alter constant Matrix/Submatrix");
      for(unsigned i=0;i<subR;i++){
	//Matrix<T>::iterator it = RowOrigin(i);
	   iterator it = RowOrigin(i);
	for(unsigned j=0;j<subC;j++){
	  *it = (T) func((ARGTYPE)*it);
	  it++;
	}
      }
    }

    /// The equality operator for the Matrix    
    /**
       Equality operator
       @param mat2 : The matrix which is the source matrix
     */
    Matrix<T>& operator=(const Matrix<T> &mat2);


    /** Matrix assignment when Matrices of different types are equated*/    
    template <class ARGTYPE>    
    Matrix<T>& operator=(const Matrix<ARGTYPE> &mat2){
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
	const ARGTYPE *i2;
	i1=origin + i*stride;
	i2=mat2.ConstOrigin() + i*mat2.GetStride();
	for(unsigned j=0;j<subC;j++){
	  //cout << " i="<<i << " j=" << j << " *i2=" << *i2 << endl;
	  *i1=(T)*i2;
	  i1++;
	  i2++;
	}
      }
      return *this;
    }

    /** scalar assignment operator */
    Matrix<T>& operator=(const T&);
   /** matrix addition operator */
    Matrix<T> operator+(const Matrix<T>&) const; 
    /** matrix += operator */
    Matrix<T>& operator+=(const Matrix<T>&); 
   /** matrix Subtraction operator */
    Matrix<T> operator-(const Matrix<T>&) const; 
    /** matrix -= operator */
    Matrix<T>& operator-=(const Matrix<T>&);   
   /** matrix scalar Multiplication operator */
    Matrix<T> operator*(const T&) const;
   /** matrix scalar rescaling (*=) operator */
    Matrix<T>& operator*=(const T&);    
   /** matrix Multiplication operator */
    Matrix<T> operator*(const Matrix<T>&) const;   
    /** Matrix Multiplication Function ; used by operator 
     @param m1 the first matrix
     @param m2 the second matrix */
    void Multiply(const Matrix<T>&m1, const Matrix<T>&m2);
    /** Transpose of a matrix */
    Matrix<T> Transpose() const ;

    /** Sorting elements of matrix in ascending order, return index table
	matrix itself stays unchanged. Uses quick sort algorithm .
	ATTENTION: The indices are starting from 0 to size-1 */
    Matrix<int> QSort();

       
    /* Sorting elements using Heapsort 
    Matrix<int> HSort();
    */
    /** Member Access Function*/ 
    inline T& operator()(const unsigned r, const unsigned c)  {LISAWPAssert((r<subR)&&(c<subC), "Indices out of range");
    return *(origin+r*stride+c);}
    /** Member Access Function
     @param r The row number
     @param c The column number*/ 
    inline const T& operator()(const unsigned r, const unsigned c) const {
      LISAWPAssert((r<subR)&&(c<subC), "Indices out of range");
      return *(const_origin+r*stride+c);}
    /** Member Access Function assuming row or column vector
     @param i The row/column number*/ 
    inline T& operator()(const unsigned i)  {if(subR==1) return (*this)(0,i); else return (*this)(i,0);}
    /** Member Access Function
     @param i The row/column number*/ 
    inline const T&  operator()(const unsigned i) const {if(subR==1) return (*this)(0,i); else return (*this)(i,0);}
    /** Return Matrices based on the input Index Sets or  unsigned int
     MUST BE CONTIGUOUS INDEX SETS
    @param i1 The index set for the rows
    @param i2 The index set for columns*/
    Matrix<T> operator()(const IndexSet &i1, const IndexSet &i2) const;
    /** Return Matrices based on the input Index Sets or  unsigned int
     MUST BE CONTIGUOUS INDEX SETS
    @param i1 The index set for the rows
    @param i2 The index set for columns*/
    Matrix<T> operator()(const IndexSet &i1, const IndexSet &i2);
    /** Return Matrices based on the input Index Sets or  unsigned int
     @param i1 The indexset for the row
     @param j The column number*/
    Matrix<T> operator()(const IndexSet &i1, unsigned j) const;
    /** Return Matrices based on the input Index Sets or  unsigned int
	@param i1 The indexset for the row
	@param j The column number*/
    Matrix<T> operator()(const IndexSet &i1, unsigned j); 
    /** Return Constant Matrices based on the input Index Sets or  unsigned int
	@param i1 The indexset for the columns
	@param j The row number*/
    Matrix<T> operator()(unsigned j, const IndexSet &i1) const;
    /** Return Matrices based on the input Index Sets or  unsigned int
	@param i1 The indexset for the columns
	@param j The row number*/
    Matrix<T> operator()(unsigned j, const IndexSet &i1) ;
    /** Return Matrices based on the input Index Set Assumes a row or column vector. Returns a 
	constant submatrix
    @param i1 The index set for the row/column*/
    Matrix<T> operator()(const IndexSet &i1) const;
      /** Return Matrices based on the input Index Set Assumes a row or column vector. Returns a 
	  submatrix
    @param i1 The index set for the row/column*/
    Matrix<T> operator()(const IndexSet &i1) ;  
    /** Set a subset of matrix values described by index sets to a constant
	Index Set values need not be contiguous
	@param iRow the row index set
	@param iCol the column index set
	@param val  the constant value
     */
    void SetValue(const IndexSet &iRow, const IndexSet &iCol, const T &val);
    /** Set a subset of matrix values described by index sets to  a matrix
	Index Set values need not be contiguous
	@param iRow the row index set
	@param iCol the column index set
	@param matrix  The matrix of values
     */
    void SetValue(const IndexSet &iRow, const IndexSet &iCol, const Matrix<T> &mat);
    /** Get a subset of matrix values described by index sets to  a matrix 
	Index Set values need not be contiguous
	@param iRow the row index set
	@param iCol the column index set
	@param matrix  The matrix of values
     */
    Matrix<T> GetSubMatrix( const IndexSet &iRow, const IndexSet &iCol) const ;
    /** computes the norm squared of the matrix */
    double NormS() const;
    /** Compute the Sum of the elements of the Matrix */
    double Sum() const;
    /** Compute the RMS of the elements of the Matrix */
    double Rms() const;
    /** Compute the minimum of the matrix and return the index of the result*/
    int MinIndex() const;
    /** Compute the maximum of the matrix and return the index of the result*/
    int MaxIndex() const;
    /** Matrix Multiplication Function ; but defined to multiply rows
	so that we can multiply a  MXN matrix with a PXN matrix to produce an
	MXP matrix
	@param m1 the first matrix
	@param m2 the second matrix */
    void MultiplyT(const Matrix<T> &, const Matrix<T> &);
    /** The destructor */
    ~Matrix();
  protected:
    iterator origin; // A pointer to the beginning of the matrix/submatrix. is 0 for constant submatrices
    const_iterator const_origin;  // A const_pointer to the beginning of the matrix
    unsigned rows,cols; // The number of rows/colums of the parent matrix
    unsigned subR,subC; // The number of rows/cols of the current matrix/submatrix
    unsigned stride;    // The difference between the beginning of 2 rows
    unsigned offset;    // The difference between the origin/const_origin with the origin of the forefather matrix
    unsigned offsetP;   // The offset of the parent matrix. This is required to do shifting operations 
  }; 

  /// Dump the Matrix
  template <class T>
  std::ostream & operator<<(std::ostream &s, const Matrix<T> &mat)
  {
    for(unsigned i=0;i<mat.GetNRows();i++){
      typename Matrix<T>::const_iterator aa = mat.ConstOrigin() + i * mat.GetStride();
      for(unsigned j=0;j<mat.GetNCols();j++){
	s << *aa++ << "   " ;
      }
      s << std::endl;
    }
    return s;
  } 
}
#endif
