// $Id: LISACODE-Couple.cpp,v 1.5 2007/04/20 12:31:01 ruiloba Exp $
/*
 *  LISACODE-Couple.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 19/09/05 by A.Petiteau, T.Regimbau 
 *
 */

#include "LISACODE-Couple.h"

// Constructors by default {0,0}
/*! \brief Constructs an instance and initializes it with default values: (0,0).
 */
Couple::Couple ()
{
	x=0.;
	y=0.;
}

/*! \brief Constructs an instance and initializes it with inputs.
 *
 * \arg	x = xvalue input
 * \arg	y = yvalue input
 */
Couple::Couple (double xvalue, double yvalue)
{
	x=xvalue;
	y=yvalue;
}

// Destructor
/*! \brief Destructor */
Couple::~Couple ()
{
	//cout << "destroy Couple \n";
}


/* Usual operations +, - ,* and / */

// Between 2 Couples
/*! \brief 2 couples addiction. */
Couple operator + (Couple z1, Couple z2)
{
	Couple z;
	z.x=z1.x+z2.x;
	z.y=z1.y+z2.y;
	return z;
}

/*! \brief 2 couples subtraction. */
Couple operator - (Couple z1, Couple z2)
{
	Couple z;
	z.x=z1.x-z2.x;
	z.y=z1.y-z2.y;
	return z;
}

/*! \brief Product of elements of two couples. */
Couple operator * (Couple z1,Couple z2)
{
	Couple z;
	z.x=z1.x*z2.x;
	z.y=z1.y*z2.y;
	return z;
}



// Between a scalar and a Couple

/*! \brief Product of a couple by a scalar. */
Couple operator * (double a, Couple z1)
{
	Couple z;
	z.x=a*z1.x;
	z.y=a*z1.y;
	return z;
}

/*! \brief Product of a couple by a scalar. */
Couple operator * (Couple z1,double a)
{
	Couple z;
	z.x=a*z1.x;
	z.y=a*z1.y;
	return z;
}
/*! \brief Division of a couple by a scalar .*/
Couple operator / (Couple z1,double a)
{
	Couple z;
	z.x=z1.x/a;
	z.y=z1.y/a;
	return z;
}


// end of LISACODE-Couple.cpp
