// $Id: LISACODE-GW.h,v 1.7 2007/04/05 09:32:38 ruiloba Exp $
/*
 *  LISACODE-GW.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description : BASIC CLASS
 * ----------
 * Cette classe (module) donne les parametres de l'onde gravitationnelle a chaque instant.
 * Typiquement h_plus et h_croix
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

/*!\defgroup gw Gravitational waves (directory Ondes_Gravit)
 * \{
 */
#ifndef __GW_H
#define __GW_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
using namespace std;

/*! \brief Gravitational Waves parameters are described in this class.
 */

class GW 
{
protected:
/*! \var Beta
  \brief Source direction angle (in the heliocentric reference frame) in radians. */
/*! \var Lambda
  \brief Source direction angle (in the heliocentric reference frame) in radians. */
    double Beta, Lambda; // Angles (in radians) which define the source direction in the heliocentric reference frame
/*! \brief Source direction unit vector (in the heliocentric reference frame). */
    vector<double> DirProp; //Unit vector which define the source direction in the heliocentric reference frame
/*! \brief Polarisation angle */
	double AnglPol; // Angle of polarisation
	
public:
		/* Constructor */
		GW();
	GW(double Beta_n, double Lambda_n);
	GW(double Beta_n, double Lambda_n, double AnglPol_n);
	virtual ~GW();
	
	/* Access methods */
/*! \brief Returns #Beta attribute. */
	double getBeta() const {return(Beta);};
	void setBeta(double Beta_n);
/*! \brief Returns #Lambda attribute. */
	double getLambda() const {return(Lambda);};
/*! \brief Sets #Lambda attribute. */
	void setLambda(double Lambda_n);
/*! \brief Returns #DirProp attribute. */
	vector<double> getDirProp() const {return(DirProp);};
/*! \brief Sets #DirProp attribute. */
	void setDirProp(vector<double> DirProp_n);
/*! \brief Returns #AnglPol attribute. */
	double getAnglPol() {return(AnglPol);}; //Return polarisation angle
/*! \brief Sets #AnglPol attribute. */
	void setAnglPol(double AnglPol_n);
	
	// Others methodes
	virtual double hp(double t); //Return h_plus  
	virtual double hc(double t); //Return h_croix
	void CalculDirProp(); //Calculation components of the unit vectors from the angles
	
};

/*!\} */ 
#endif // __GW_H

// end of LISACODE-GW.h

