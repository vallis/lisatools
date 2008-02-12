// $Id: LISACODE-USOClock.h,v 1.6 2007/04/05 09:32:47 ruiloba Exp $
/*
 *  USOClock.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description : BASIC CLASS
 * ----------
 * Cette classe (module) renseigne le temps propre a chaque satellite a partir
 * du temps reel (ou temps de coordonnee). Ce temps propre est base sur 
 * l'Ultra Stable Oscillator de chaque satellite. L'USO donne un temps qui peut 
 * avoir un offset, une derive et un bruit (toujours par rapport au temps reel).
 * On peut egalement tenir compte des effets relativistes qui implique des differences
 * de temps propres entre les satellites
 *
 *
 *  Created on 20/10/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 20/10/05 by A.Petiteau, T.Regimbau 
 *
 */
 
/*!\defgroup usoClock USO clock (directory USO_Temps)
 *\{
 */ 
#ifndef __USOCLOCK_H
#define __USOCLOCK_H
 
 
#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "randlib.h"
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
using namespace std;
 
/*! \brief Ultra Stable Oscillator based satellite time is defined in this class.
 */

class USOClock 
{
protected:
/*! \brief Offset */
	double Offset;
/*! \brief Slope */
	double DerivLinearCoef;
/*! \brief USO noise in second per second */
	double SigmaNoise; // USO noise in second per second 
/*! \brief FALSE if the is no noise, else TRUE */
	bool USONoise;

	
public:
	/* Constructor */
	USOClock();
	USOClock(double Offset_n);
	USOClock(double Offset_n, double DerivLinearCoef_n, double SigmaNoise_n);
	~USOClock();
	 
	 
	/* Access methods */
	void init(double Offset_n, double DerivLinearCoef_n, double SigmaNoise_n);
/*! \brief Returns #Offset attribute */
	double getOffset() {return(Offset);};
/*! \brief Returns #DerivLinearCoef attribute */
	double getDeriv() {return(DerivLinearCoef);};
/*! \brief Returns #SigmaNoise attribute */
	double getNoise() {return(SigmaNoise);};
	
	/*  Others methods */
	double gGap(double t, double tStep);
	double gTime(double t, double tStep);
 };

/*!\} */ 
#endif // __USOCLOCK_H
 
 // end of USOClock.h
 
