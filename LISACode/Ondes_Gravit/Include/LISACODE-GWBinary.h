// $Id: LISACODE-GWBinary.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWBinary.h
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  This class (module) returns h_plus and h_cross for a monochromatic binary system.
 *
 *
 *  Created on 01/06/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 06/06/06 by A.Petiteau
 *
 */

#ifndef __GWBINARY_H
#define __GWBINARY_H


#include <stdexcept>
#include <iostream.h>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Couple.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves parameters for a monochromatic binary system are defined in this class.
 */
class GWBinary : public GW
{
protected :
/*! \var M1
  \brief First star mass (in solar masses) */
/*! \var M2
  \brief Second star mass (in solar masses) */
    double M1, M2; // The masses of the two stars in solar masses
    /*! \brief orbital frequency (Hertz) */
    double forb; // Rotationnal frequency (Hertz)
/*! \var inc
  * \brief \f$ \textrm{inclination angle (} [0,2 \cdot \pi [ \textrm{)} \f$ */
/*! \var phi0
  \brief \f$ \textrm{initial phase (} [0,2 \cdot \pi [ \textrm{)} \f$ */
    double inc, phi0; //Angles of inclination and initial phase between [0,2 Pi]
    /*! \brief Distance to the source in kpc (kiloparsec) */
    double r; //Distance to the source in kpc
/*! \var Ap
  \brief Polarized amplitude */
/*! \var Ac
  \brief Polarized amplitude */
    double Ap, Ac; //Polarized amplitudes 
	
	
public:
		/* Constructor */
		GWBinary(); // Default constructor : AMCVn
	
	// Takes as argument the parameters specific to a given binary system
	//  and returns the polarized amplitudes A+ and Ax
    GWBinary (double Beta_n,
			  double Lambda_n,
			  double AnglPol_n,
			  double bM1,
			  double bM2,
			  double bforb,
			  double binc,
			  double bphi0,
			  double br); 
	
	/* Destructor*/
    ~GWBinary (); 
	
	/* Access methods */	
/*! \brief Returns #M1 attribute */
	double getM1() {return(M1);};
/*! \brief Returns #M2 attribute */
	double getM2() {return(M2);};
/*! \brief Returns #forb attribute */
	double getForb() {return(forb);};
/*! \brief Returns #inc attribute */
	double getInc() {return(inc);};
/*! \brief Returns #phi0 attribute */
	double getPhi0() {return(phi0);};
/*! \brief Returns #r attribute */
	double getDistance() {return(r);};
	void setM1(double M1_n);
	void setM2(double M2_n);
	void setForb(double forb_n);
	void setInc(double inc_n);
	void setPhi0(double phi0_n);
	void setDistance(double r_n);	
	
	
	/*  Others methods */
	void init(); // Compute the polarized amplitudes A+ and Ax
	double hp(double t); //Return h_plus  
	double hc(double t); //Return h_croix	
						 // Returns the signal h+ and hx
						 // Takes the time in s as argument
	Couple hbin(double t);
	
};


#endif // __GWBINARY_H

// end of LISACODE-GWBinary.h
