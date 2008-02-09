// $Id: LISACODE-Vect.cpp,v 1.4 2007/04/05 09:33:22 ruiloba Exp $
/*
 *  LISACODE-Vect.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 21/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/09/05 by A.Petiteau, T.Regimbau 
 *
 */

#include "LISACODE-Vect.h"

// Constructor by default {0,0,0}
/*! \brief Constructs an instance and initializes it with default values.
 *
 * p attribute is set
 * \f[ p=  \left(  \begin{array}{c}  0  \\  0  \\  0   \end{array} \right) \f]
 */
Vect::Vect ()
{
	int k;
	for(k=0;k<3;k++)
		p[k]=0.;
}

/*! \brief Constructs an instance and initializes it with t input.
 *
 * p attribute is set to t input
 */
Vect::Vect (double t[3])
{
	int k;
	for(k=0;k<3;k++)
		p[k]=t[k];
}

// Destructor
/*! \brief Destructor.*/
Vect::~Vect ()
{
	//cout << "destroy vector \n";
}


// Display the vector coordinates
/*! \brief Displays vector components. */
void Vect::display ()
{
	int k;
	for(k=0;k<3;k++)
		cout << p[k]  << "\t ";
	cout << "\n";
}


// Returns the norme
/*! \brief Returns vector norm
 *
 * \f[ \textrm{returned value}= \sqrt{(\overrightarrow{p} \cdot \overrightarrow{p})} \f]
 *
 */
double Vect::norme ()
{
	int k;
	double norm=0.;
	
	for(k=0;k<3;k++)
		norm=norm+p[k]*p[k];
	norm=sqrt(norm);
	
	return norm;
}

// Returns the unit vector
/*! \brief Returns unit vector
 * 
 * #norme method is called
 * \f[ \textrm{returned value}= \frac{\overrightarrow{p}}{\sqrt{(\overrightarrow{p} \cdot \overrightarrow{p})}} \f]
 */
Vect Vect::unit ()
{
	int k;
	double norm=0.;
	Vect vhat;
	for(k=0;k<3;k++)
		norm=norm+p[k]*p[k];
	norm=sqrt(norm);
	for(k=0;k<3;k++)
		vhat.p[k]=p[k]/norm;
	return vhat;
}


/* Usual operations +, - ,* and /  */

// Between 2 vectors, returns a vector
/*! \brief
 * Vectors addition : returns vector u+v. */
Vect operator + (Vect u, Vect v)
{
	int k;
	Vect w;
	for(k=0;k<3;k++)
		w.p[k]=u.p[k]+v.p[k];
	return w;
}

/*! \brief
 * Vectors subtraction : returns vector u-v. */
Vect operator - (Vect u, Vect v)
{
	int k;
	Vect w;
	for(k=0;k<3;k++)
		w.p[k]=u.p[k]-v.p[k];
	return w;
}


// vector scalar product, returns a scalar
/*! \brief
 * Vectors scalar product, returns a scalar.
 *
 * \f[ \textrm{returned value}= \overrightarrow{u} \cdot \overrightarrow{v} \f]
 */
double operator * (Vect u, Vect v)
{
	int k;
	double prod=0.;
	for (k=0;k<3;k++)
		prod=prod+u.p[k]*v.p[k];
	return prod;
}

// Between a scalar and a vector, returns a vector
/*! \brief
 * Vector and  scalar product, returns a vector.
 *
 * \f[ \textrm{returned value}= a \cdot \overrightarrow{u} \f]
 */
Vect operator * (double a, Vect u)
{
	int k;
	Vect v;
	for(k=0;k<3;k++)
		v.p[k]=u.p[k]*a;
	return v;
}

/*! \brief
 * Vector and  scalar product, returns a vector.
 *
 * \f[ \textrm{returned value}= a \cdot \overrightarrow{u} \f]
 */
Vect operator * (Vect u, double a)
{
	int k;
	Vect v;
	for(k=0;k<3;k++)
		v.p[k]=u.p[k]*a;
	return v;
}

/*! \brief
 * Vector and  scalar division, returns a vector.
 *
 * \f[ \textrm{returned value}= \frac{\overrightarrow{u}}{a} \f]
 */
Vect operator / (Vect u, double a)
{
	int k;
	Vect v;
	for(k=0;k<3;k++)
		v.p[k]=u.p[k]/a;
	return v;
}
