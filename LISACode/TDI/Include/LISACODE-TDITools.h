// $Id: LISACODE-TDITools.h,v 1.4 2007/04/05 09:32:44 ruiloba Exp $
/*
 *  TDITools.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 *  
 *
 *  Created on 30/06/05 by Antoine PETITEAU (APC)
 *  Last modification on 30/09/05 by Antoine PETITEAU (APC)
 *
 */


/*!\ingroup tdiHand
 *\defgroup tdiTools TDITools
 *\{ 
 */
#ifndef __TDITOOLS_H
#define __TDITOOLS_H


#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-Memory.h"
using namespace std;

/*! \brief Time Delay Interferometry tools class.
 */

class  TDITools
	{
	protected:
		/*!\brief Pointer to the list of delay's lengths. */
		Memory * TDelay;
		/*! \brief Memory of the 3 delays for the current time. */
		double DelayMem[3];
		/*! \brief Delay interpolation : type */
		INTERP DelayInterpType;
		/*! \brief Delay interpolation : Number of value need  */
		double DelayInterpVal;
		/*! \brief If it's TRUE, approximations are done in order to compute delays more quickly. */
		bool RapidOption; //If it's true, the compute of the delays is approximated to accelerate compute
		
	public:
		/* Constructor */
		TDITools();
		TDITools(Memory * TDelay_n, bool RapidOption_n, INTERP DelayInterpType_n = TRU, double DelayInterpVal_n = 0);
		virtual ~TDITools();
		
		/* Access methods */
		double getDelay(int IndexDelay); // Return delay value for the specified index
		
		/*! \brief Returns RapidOption attribute. */
		bool getRapidOption(){return(RapidOption);};
		
		/*  Others methods */
		void  RefreshDelay(double tComputeDelay); // Obtain delays for the current time
		
	};

/*!\}*/
#endif // __TDITOOLS_H

// end of TDITools.h


