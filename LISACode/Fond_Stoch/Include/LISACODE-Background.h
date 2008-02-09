// $Id: LISACODE-Background.h,v 1.7 2007/03/29 15:16:17 lalanne Exp $
/*
 *  LISACODE-Background.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description : BASIC CLASS
 * ----------
 * Cette classe (module) donne le signal dnu/nu du fond sur les phasemetres a chaque instant.
 *
 *
 *  Created on 30/05/06 by Antoine PETITEAU (APC)
 *  Last modification on 30/05/06 by Antoine PETITEAU (APC)
 *
 */

#ifndef __BACKGROUND_H
#define __BACKGROUND_H


#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Geometry.h"

using namespace std;
/*!\defgroup bg Background (directory Background)
 *\{
*/

/*! \brief Background signal received by phasemeters is described in this class.
 *
 * Background signal depends on satellites position given by #LISAGeo attribute. 
 */
class Background
{
protected:

/*! \brief LISA orbit description.
 */
    Geometry * LISAGeo;

	
	
public:
	/* Constructor */
	Background();
	Background(Geometry * LISAGeo_n);
	virtual ~Background();
	
	/* Access methods */
	void setGeometry(Geometry * LISAGeo_n);

	
	/* Others methods */
	virtual double deltanu(int iSC, int IndirSens, double t);
	
};
/*!\}*/
#endif // __BACKGROUND_H

// end of LISACODE-Background.h

