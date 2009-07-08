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
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWBINARY_H
#define __GWBINARY_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves parameters for a monochromatic binary system are defined in this class.
 */
class GWBinary : public GW
{
protected :
	// *************** Parameters of system ***************
	/*! \brief First star mass (in solar masses) */
    double M1;
	/*! \brief Second star mass (in solar masses) */
	double M2; 
    /*! \brief orbital frequency (Hertz) */
    double forb;
	/*! \f$ \textrm{inclination angle (} [0,2 \cdot \pi [ \textrm{)} \f$ */
	double inc;
	/*! \brief \f$ \textrm{initial phase (} [0,2 \cdot \pi [ \textrm{)} \f$ */
    double phi0;
    /*! \brief Distance to the source in kpc (kiloparsec) */
    double r; 
	
	// *************** Used variables ***************
	/*! \brief Polarized amplitude */
	double Ap;
	/*! \brief Polarized amplitude */
    double Ac; 
	
public:
		// ** Constructor **
		GWBinary();
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
	
	// ** Destructor**
    ~GWBinary (); 
	
	// ** Access methods **	
	/*! \brief Sets parameters specified by iP :
	 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
	 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
	 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
	 * \arg	3 -> #M1 : \f$ m_1 \f$ Mass of object 1 (in solar masses)
	 * \arg	4 -> #M2 : \f$ m_2 \f$ Mass of object 2 (in solar masses)
	 * \arg	5 -> #r : \f$ r \f$ Distance (in kpc)
	 * \arg	6 -> #phi0 : \f$ \phi_0 \f$ Initial phase (in radians)
	 * \arg	7 -> #forb : \f$ f_{orb} \f$ :Orbital frequency (in Hertz)
	 * \arg	8 -> #inc : \f$ i \f$ Inclination (in radians)
	 */
	void setParam(int iP, double Param_n);
	/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
	double getParam(int iP);
	double getM1() {return(M1);};
	double getM2() {return(M2);};
	double getForb() {return(forb);};
	double getInc() {return(inc);};
	double getPhi0() {return(phi0);};
	double getDistance() {return(r);};
	void setM1(double M1_n);
	void setM2(double M2_n);
	void setForb(double forb_n);
	void setInc(double inc_n);
	void setPhi0(double phi0_n);
	void setDistance(double r_n);	
	
	
	// **  Others methods **
	/*! \brief Initialization : Sets polarized amplitudes attributes #Ap and #Ac. */
	void init();
	/*! \brief Return \f$ h_{+}(t) \f$ */
	double hp(double t);	  
	/*! \brief Return \f$ h_{\times}(t) \f$ */
	double hc(double t);
	/*! \brief Display ...  [for specific use or debug] */
	void DispTempVal(double t, ostream * OutDisp);
	
};


#endif // __GWBINARY_H

// end of LISACODE-GWBinary.h
