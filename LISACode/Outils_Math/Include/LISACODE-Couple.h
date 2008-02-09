// $Id: LISACODE-Couple.h,v 1.6 2007/04/05 09:32:42 ruiloba Exp $
/*
 *  LISACODE-Couple.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  Couple of double
 *
 *
 *  Created on 21/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 21/09/05 by A.Petiteau, T.Regimbau 
 *
 */
/*!\ingroup mathTools
 *\defgroup couple Couple
 *\{
 */
#ifndef __COUPLE_H
#define __COUPLE_H

#include <stdexcept>
#include <iostream.h>
#include <math.h>
using namespace std;


/*! \brief Couple management class.	
 */
class Couple
{
public:
/*! \var x
  \brief First component.	
 */
/*! \var y
  \brief Second component.	
 */
	double x,y;
	
    // Constructors by default {0,0}
	Couple(); 
	Couple(double,double); 
	
	// Destructor
	~Couple(); 
	
	// Usual operations +, - ,* and /
	friend Couple operator+ (Couple,Couple);
	friend Couple operator- (Couple,Couple);
/*! \brief ?? where operator* (Couple,Couple) is defined? */
	friend Couple operator* (Couple,Couple);
	friend Couple operator* (double,Couple);
	friend Couple operator* (Couple,double);
	friend Couple operator/ (Couple,double);
};

/*! \}*/
#endif //__COUPLE_H

// end of LISACODE-Couple.h
