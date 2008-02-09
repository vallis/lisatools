// $Id: LISACODE-Vect.h,v 1.5 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACODE-Vect.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  3-D vectors
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 19/09/05 by A.Petiteau, T.Regimbau 
 *
 */
/*!\ingroup mathTools
 *\defgroup vect Vector
 * \{
 */
#ifndef __VECT_H
#define __VECT_H

#include <stdexcept>
#include <iostream.h>
#include <math.h>

/*!\brief 3 components vector management class.
 */
class Vect
{
public:
  /*!\brief 3 components. */
	double p[3];
	
	// Constructor by default {0,0,0}
	Vect(); 
	Vect(double[3]);
	
	// Destructor
	~Vect();
	
	// Display the vector coordinates
	void display(); 
	
	//returns the norme
	double norme();
	
	// Returns the unit vector
	Vect unit();
	
	// usual operations +, - ,* and /
	friend Vect operator+ (Vect,Vect);
	friend Vect operator- (Vect,Vect);
	friend double operator* (Vect,Vect);//vector scalar product
	friend Vect operator* (double,Vect);
	friend Vect operator* (Vect,double);
	friend Vect operator/ (Vect,double);
		
};
/*! \}*/
#endif //__VECT_H

// end of LISACODE-Vect.h
