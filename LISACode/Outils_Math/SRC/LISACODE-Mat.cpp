// $Id: LISACODE-Mat.cpp,v 1.4 2007/04/05 09:33:22 ruiloba Exp $
/*
 *  LISACODE-Mat.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 21/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/09/05 by A.Petiteau, T.Regimbau 
 *
 */

#include "LISACODE-Mat.h"

// Constructors by default {{0,0,0},0,0,0},0,0,0}}
/*! \brief Constructs an instance and initializes it with default value.
 *
 * \f[ Mat= 
 * \left(  \begin{array}{ccc} 
 * 0 & 0 & 0 \\ 
 * 0 & 0 & 0 \\ 
 * 0 & 0 & 0 
 * \end{array} \right) \f]
 */
Mat::Mat ()
{
	int k, l;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			p[k][l]=0.;
}

/*! \brief Constructs an instance and initializes it with A (3,3) matrix input. */
Mat::Mat (double A[3][3])
{
	int k,l;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			p[k][l]=A[k][l];
}

// Destructor
/*! \brief Destructor */
Mat::~Mat ()
{
	//cout << "destroy matrix \n";
}


// Display the matrix coordinates
/*! \brief Displays matrix components. */
void Mat::display ()
{
	int k,l;
	for (k=0; k<3; k++)
	{
		for (l=0; l<3; l++)
			cout << p[k][l] << "\t";
		cout << "\n";
	}
}


/* Usual operations +,-,*    */

// Between 2 matrices, returns a matrix

/*! \brief
 * Matrices addition. It returns matrix A+B. */
Mat operator + (Mat A, Mat B)
{
	int k,l;
	Mat C;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			C.p[k][l]= A.p[k][l] + B.p[k][l];
	return C;
}

/*! \brief
 * Matrices subtraction. It returns matrix A-B. */
Mat operator - (Mat A, Mat B)
{
	int k,l;
	Mat C;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			C.p[k][l]= A.p[k][l] - B.p[k][l];
	return C;
}

// Between a matrix and a scalar, returns a matrix

/*! \brief
 * Product between a scalar and a matrix. It returns matrix: \a f.\a A */
Mat operator * (double f, Mat A)
{
	int k,l;
	Mat B;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			B.p[k][l]= f*A.p[k][l];
	return B;
}

// between a matrix and a scalar, returns a vector


/*! \brief
 * Product between a matrix and vector. It returns vector A.v */
Vect operator * (Mat A, Vect u)
{
	int k,l;
	Vect v;
	for (k=0; k<3; k++)
		for (l=0; l<3; l++)
			v.p[k]=v.p[k]+A.p[k][l]*u.p[l];
	return v;
}


// end of LISACODE-Mat.cpp
