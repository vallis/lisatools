// $Id: LISACODE-Mat.h,v 1.5 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACODE-Mat.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  3x3 rectangular matrices
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 19/09/05 by A.Petiteau, T.Regimbau 
 *
 */
/*!\ingroup mathTools
 *\defgroup matrix Matrix
 */
#ifndef __MAT_H
#define __MAT_H

#include <stdexcept>
#include <iostream.h>
#include <math.h>
#include "LISACODE-Vect.h"
using namespace std;



/*!\ingroup matrix
 *\brief (3x3) matrix management class.
 */
class Mat
{
public:
  /*!\brief (3x3) components */
	double p[3][3];
	
	// Constructors by default {{0,0,0},0,0,0},0,0,0}}
	Mat();
	Mat(double[3][3]);
	
	// Destructor
	~Mat();	
	
	// Display the matrix coordinates
	void display();

	// Usual operations +,-,*
	friend Mat operator+ (Mat,Mat);
	friend Mat operator- (Mat,Mat);
	friend Mat operator* (double,Mat);
	friend Vect operator* (Mat,Vect);
};
#endif //__MAT_H

// end of LISACODE-Mat.h
