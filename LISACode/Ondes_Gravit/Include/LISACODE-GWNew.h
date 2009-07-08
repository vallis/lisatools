// $Id: LISACODE-GWNew.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
// template for a new GW (copied on GWMono)
/*
 *  LISACODE-GWNew.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne les parametres de l'onde gravitationnelle a chaque instant.
 * Typiquement h_plus et h_croix
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWNEW_H
#define __GWNEW_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
 */
/*! \brief Gravitational Waves instantaneous parameters h_plus and h_cross are described in this class.
 */
class GWNew : public GW
	{
	protected:
		/*! \brief Frequency */
		double Freq; 
		/*! \brief h_{+} polarisation component amplitude  */
		double Amplhp; 
		/*! \brief h_{x} polarisation component amplitude */
		double Amplhc; 
		/*! \brief Polarisation angle (rad)
		 *
		 * Angle between the projection of x (vernal point direction)
		 * in the wave frame and the polarisation vector
		 */
		double AnglPol; // Angle between the projection of x (direction of vernal point) in the wave frame and the polarisation vector in radians
		
		/*! \brief Initial phase of h_plus polarisation component.  */
		double Phi0hp; // Initial phase for h+ component
		/*! \brief Initial phase of h_cross polarisation component.  */
		double Phi0hc; // Initial phase for hx component	
		
	public:
		// ** Constructor **
		GWNew();
		GWNew(	double Beta_n,
			   double Lambda_n,
			   double AnglPol_n,
			   double Freq_n,
			   double Amplhp_n,
			   double Amplhc_n);
		GWNew(	double Beta_n, 
			   double Lambda_n, 
			   double AnglPol_n,
			   double Freq_n,
			   double Amplhp_n,
			   double Amplhc_n,
			   double Phi0hp_n,
			   double Phi0hc_n);
		~GWNew();
		
		// ** Access methods **
		
		/*! \brief Sets parameters specified by iP :
		 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
		 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
		 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
		 * \arg 3 -> #Freq : \f$ f_{GW} \f$ : Frequency (in Hertz)
		 * \arg 4 -> #Amplhp : \f$ h_{0+} \f$ : Amplitude of component +
		 * \arg 5 -> #Amplhc : \f$ h_{0\times} \f$ : Amplitude of component \f$ \times \f$
		 * \arg 6 -> #Phi0hp : \f$ \phi_{0+} \f$ : Initial phase of component + (in radians)
		 * \arg 7 -> #Phi0hc : \f$ \phi_{0\times} \f$ : Initial phase of component \f$ \times \f$ (in radians)
		 */
		void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		double getParam(int iP);
		double getFreq() const {return(Freq);};
		double getAmplhp() const {return(Amplhp);};
		double getAmplhc() const {return(Amplhc);};
		double getAnglPol() const {return(AnglPol);};
		double getPhi0hp() const {return(Phi0hp);};
		double getPhi0hc() const {return(Phi0hc);};
		void setFreq(double Freq_n);
		void setAmplhp(double Amplhp_n);
		void setAmplhc(double Amplhc_n);
		void setAnglPol(double AnglPol_n);
		void setPhi0hp(double Phi0hp_n) { Phi0hp = Phi0hp_n;};
		void setPhi0hc(double Phi0hc_n) { Phi0hc = Phi0hc_n;};
		
		// ** Others methods **
		/*! \brief Initialization : ... */
		void init();
		/*! \brief Return \f$ h_{+}(t) \f$ */
		double hp(double t); 
		/*! \brief Return \f$ h_{\times}(t) \f$ */
		double hc(double t); 
		/*! \brief Display ...  [for specific use or debug]\f$ */
		virtual void DispTempVal(double t, ostream * OutDisp);
	};

#endif // __GWNEW_H

// end of LISACODE-GWNew.h

