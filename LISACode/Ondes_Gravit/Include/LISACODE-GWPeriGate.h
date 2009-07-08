// $Id: LISACODE-GWPeriGate.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWPeriGate.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne h_plus et h_croix sous la forme d'un signal porte periodique.
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWPERIGATE_H
#define __GWPERIGATE_H


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
/*! \brief Gravitational Waves periodic gate signal.
 *
 * h_plus and h_cross polarisation componenrs are modelised as periodic gate signals.
 */
class GWPeriGate : public GW
	{
	protected:
		/*! \brief Frequency */
		double Freq; //Frequence de l'onde 
		/*! \brief hp component amplitude  */
		double Amplhp; //Amplitude de la composante h+ de l'onde
		/*! \brief hc component amplitude */
		double Amplhc; //Amplitude de la composante hx de l'onde
		/*! \brief Polarisation angle (rad)
		 *
		 * Angle between the projection of x (vernal point direction)
		 * in the wave frame and the polarisation vector
		 */
		double AnglPol; //Angle entre la projection de x (direction du point vernal) dans le plan de l'onde et i (vecteur de polarisation de l'onde) en radians 
		
		
	public:
		// ** Constructor **
		GWPeriGate();
		GWPeriGate(double Beta_n, double Lambda_n, double AnglPol_n, double Freq_n, double Amplhp_n, double Amplhc_n);
		
		~GWPeriGate();
		
		// ** Access methods **
		
		/*! \brief Sets parameters specified by iP :
		 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
		 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
		 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
		 * \arg 3 -> #Freq : \f$ f_{GW} \f$ : Frequency (in Hertz)
		 * \arg 4 -> #Amplhp : \f$ h_{0+} \f$ : Amplitude of component +
		 * \arg 5 -> #Amplhc : \f$ h_{0\times} \f$ : Amplitude of component \f$ \times \f$
		 */
		void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		double getParam(int iP);
		/*! \brief Returns #Freq attribute */
		double getFreq() const {return(Freq);};
		void setFreq(double Freq_n);
		/*! \brief Returns #Amplhp attribute */
		double getAmplhp() const {return(Amplhp);};
		void setAmplhp(double Amplhp_n);
		/*! \brief Returns #Amplhc attribute */
		double getAmplhc() const {return(Amplhc);};
		void setAmplhc(double Amplhc_n);
		
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

#endif // __GWPERIGATE_H

// end of LISACODE-GWPeriGate.h

