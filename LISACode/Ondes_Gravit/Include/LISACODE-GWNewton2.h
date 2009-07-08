// $Id: LISACODE-GWNewton2.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWNewton2.h
 *  V 1.4.1
 *  LISACode
 *
 *
 * Description :
 * -------------
 * This class (module) returns h_plus and h_cross for a binary system computed at 1 PN or 2.5 PN .
 *
 *
 *  Created on 16/11/06 by Antoine PETITEAU and Gerard AUGER (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWNEWTON2_H
#define __GWNEWTON2_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Couple.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves binary system parameters computation.
 *
 * h_plus and h_cross polarisation components are computed at 1 or 2.5 
 * Post-Newtonian approximation order.
 */
class GWNewton2 : public GW  // la class : public GW
{
	/*  Donnees
	- type calcul ordre 1 (type=1) ordre 2 (type-2)
	- m1 masse du premier
	- m2 masse du second
	- inc angle d'inclinaison
	- rdist distance en parsec
	- phcoal phase a l'origine
	- tcoal temps avant la coalescence
	- temps (temps=0 ==> calcul a tcoal)
	*/
protected :
/*! \brief Computation order type. 
 *
 * If type=1, order=1; if type=2, order=2
 */
	int    type;
/*! \brief First star mass (in solar masses). */
	double m1; // The masses of the first star in solar masses
/*! \brief Second star mass (in solar masses). */
	double m2; // The masses of the second star in solar masses
/*! \brief Time before the coalescence. */
	double tcoal; // Time before the coalescence
/*! \brief \f$ \textrm{inclination angle (} [0,2 \cdot \pi [ \textrm{)} \f$ */
	double inc; // Inclination
/*! \brief Phase before the coalescence. */
	double phcoal; // Phase at the origin
/*! \brief Distance to the source in kpc(kiloparsec). */
	double rdist; // Distance in kpc
/*! \brief Total mass. */
	double mtot; // Total mass
/*! \brief Mass difference. */
	double deltam; // Difference of mass
/*! \brief Reduced mass. */
	double mu; 
/*! \brief Mass ratio. */
	double nu;
/*! \brief cos(#inc) */
	double ci; // Cos of inclination
/*! \brief sin(#inc) */
	double si; // Sin of inclination
/*! \brief Chirp mass. */
	double cmass; // Chirp mass
/*! \brief Current computation time. */	
	double time_encour;
/*! \brief 1PN amplitude (depending on time) */
	double hint;
/*! \brief Pulsation (depending on time). */
	double omega;
/*! \brief Phase (depending on time). */
	double phi;
	
/*! \var teta
  \brief System constant. */
/*! \var lambda
  \brief System constant. */
/*! \var taud
  \brief 2.5 PN approximation parameter. */
/*! \var taud0
  \brief System parameter. */
/*! \var omega0
  \brief System parameter. */
/*! \var gw
  \brief System parameter. */
/*! \var psi
  \brief 2.5 PN parameter (depending on time). */
/*! \var a1
  \brief 2.5 PN approximation parameter. */
/*! \var a2
  \brief 2.5 PN approximation parameter. */
/*! \var a3
  \brief 2.5 PN approximation parameter. */
/*! \var a4
  \brief 2.5 PN approximation parameter. */
/*! \var a5
  \brief 2.5 PN approximation parameter. */
/*! \var a6
  \brief 2.5 PN approximation parameter. */
/*! \var b1
  \brief 2.5 PN approximation parameter. */
/*! \var b2
  \brief 2.5 PN approximation parameter. */
/*! \var b3
  \brief 2.5 PN approximation parameter. */
/*! \var b4
  \brief 2.5 PN approximation parameter. */
/*! \var a11
  \brief 2.5 PN approximation parameter. */
/*! \var a22
  \brief 2.5 PN approximation parameter. */
/*! \var b11
  \brief 2.5 PN approximation parameter. */
/*! \var c1
  \brief 2.5 PN approximation parameter. */
/*! \var c2
  \brief 2.5 PN approximation parameter. */
/*! \var c3
  \brief 2.5 PN approximation parameter. */
/*! \var d1
  \brief 2.5 PN approximation parameter. */
/*! \var d2
  \brief 2.5 PN approximation parameter. */
/*! \var e1
  \brief 2.5 PN approximation parameter. */
/*! \var e2
  \brief 2.5 PN approximation parameter. */
/*! \var e3
  \brief 2.5 PN approximation parameter. */
/*! \var e4
  \brief 2.5 PN approximation parameter. */
/*! \var e5
  \brief 2.5 PN approximation parameter. */
/*! \var a7
  \brief 2.5 PN approximation parameter. */
/*! \var f1
  \brief 2.5 PN approximation parameter. */
/*! \var f3
  \brief 2.5 PN approximation parameter. */
/*! \var f5
  \brief 2.5 PN approximation parameter. */
/*! \var f6
  \brief 2.5 PN approximation parameter. */
/*! \var f7
  \brief 2.5 PN approximation parameter. */
/*! \var f8
  \brief 2.5 PN approximation parameter. */
/*! \var f9
  \brief 2.5 PN approximation parameter. */
/*! \var f10
  \brief 2.5 PN approximation parameter. */
/*! \var a1x
  \brief 2.5 PN approximation parameter. */
/*! \var b1x
  \brief 2.5 PN approximation parameter. */
/*! \var c1x
  \brief 2.5 PN approximation parameter. */
/*! \var c2x
  \brief 2.5 PN approximation parameter. */
/*! \var d1x
  \brief 2.5 PN approximation parameter. */
/*! \var d2x
  \brief 2.5 PN approximation parameter. */
/*! \var d3x
  \brief 2.5 PN approximation parameter. */
/*! \var d4x
  \brief 2.5 PN approximation parameter. */
/*! \var d5x
  \brief 2.5 PN approximation parameter. */
/*! \var e1x
  \brief 2.5 PN approximation parameter. */
/*! \var e2x
  \brief 2.5 PN approximation parameter. */
/*! \var e3x
  \brief 2.5 PN approximation parameter. */
/*! \var e4x
  \brief 2.5 PN approximation parameter. */
/*! \var e5x
  \brief 2.5 PN approximation parameter. */
/*! \var e6x
  \brief 2.5 PN approximation parameter. */
/*! \var e7x
  \brief 2.5 PN approximation parameter. */
/*! \var e8x
  \brief 2.5 PN approximation parameter. */
/*! \var g1
  \brief 2.5 PN approximation parameter. */
	// Parameters for 2.5
	double teta,lambda,taud,taud0,omega0,gw,psi;
	double a1,a2,a3,a4,a5,a6,b1,b2,b3,b4,a11,a22,b11;
	double c1,c2,c3,d1,d2,e1,e2,e3,e4,e5,a7,f1,f3,f5,f6,f7,f8,f9,f10;
	double a1x,b1x,c1x,c2x,d1x,d2x,d3x,d4x,d5x,e1x,e2x,e3x;
	double e4x,e5x,e6x,e7x,e8x,g1;
	
	void commun (double ttime);     // Acces function
	
public:
		/* Constructor */
		GWNewton2(); // Default constructor 
	GWNewton2(double Beta_n,
			  double Lambda_n,
			  double AnglPol_n,
			  int ttype,
			  double mm1,
			  double mm2,
			  double ttcoal,
			  double iinc, 
			  double pphcoal,
			  double rrdist, 
			  double ttaud0,
			  double oomega0, 
			  double ggw); // Constructor for order 1 et 2
	
	/* Destructor*/
    ~GWNewton2 (); 
	
	// ** Access methods **
	
	/*! \brief Sets parameters specified by iP :
	 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
	 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
	 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
	 * \arg 3 -> #m1 : \f$ m_1 \f$ : Mass of object 1 (in solar masses)
	 * \arg 4 -> #m2 : \f$ m_2 \f$ : Mass of object 2 (in solar masses)
	 * \arg 5 -> #tcoal : \f$ t_{coal} \f$ : Time of coalescence (in seconds)
	 * \arg 6 -> #inc : \f$ i \f$ : Inclination angle (in radians)
	 * \arg 7 -> #phcoal : \f$ \phi_c \f$ : Phase at coalescence (in radians)
	 * \arg 8 -> #rdist : \f$ r \f$ : Distance (in kpc)
	 */
	virtual void setParam(int iP, double Param_n);
	/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
	virtual double getParam(int iP);	
	double getM1() {return(m1);};
	double getM2() {return(m2);};
	double getTcoal() {return(tcoal);};
	double getInc() {return(inc);};
	double getPhCoal() {return(phcoal);};
	double getDistance() {return(rdist);};
	void setM1(double m1_n);
	void setM2(double m2_n);
	void setTcoal(double tcoal_n);
	void setInc(double inc_n);
	void setPhCoal(double phcoal_n);
	void setDistance(double rdist_n);	
	
	
	// ** Others methods **
	/*! \brief Initialization : ... */
	void init();
	/*! \brief Return \f$ h_{+}(t) \f$ */
	double hp(double t); 
	/*! \brief Return \f$ h_{\times}(t) \f$ */
	double hc(double t); 
	/*! \brief Display ...  [for specific use or debug]\f$ */
	virtual void DispTempVal(double t, ostream * OutDisp);
	/*! \brief Return frequency */
	double fe(double t); 
	/*! \brief Return phase */
	double phase(double t); 
	
};


#endif // __GWNEWTON2_H

// end of LISACODE-GWNewton2.h
