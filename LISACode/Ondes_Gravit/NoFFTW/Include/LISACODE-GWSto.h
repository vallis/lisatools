/*
 *  LISACODE-GWsto.h
 *  V 1.3.1
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Cette classe (module) donne les parametres d'une onde gravitationnelle a chaque instant.
 * Cette onde est sensé provenir de ?? avec h_plus en f^n et h_croix en f^n.
 * A chaque demande H+ et Hx sont fournient. 
 *
 *
 *  Created by Auger Gerard on 22/10/07.
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWSto_H
#define __GWSto_H


#include <stdexcept>
#include <iostream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
#include "LISACODE-Noise.h"
#include "LISACODE-NoiseWhite.h"
#include "LISACODE-NoiseFilter.h"
#include "LISACODE-NoiseFile.h"
#include "LISACODE-NoiseOof.h"
//#include "LISACODE-NoiseFilter_Sto.h"

using namespace std;
/*!\ingroup gw
 */
/*! \brief Gravitational Waves instantaneous parameters h_plus and h_cross are described in this class.*/



class GWSto : public GW
	{
	protected:
		
		double tStep;
		double tDurAdd;
		double tFirst;
		double tLast;
		double Slope;
		double Tsample; 
		double Fknee; 
		double Fmin;
		int Nb_Ageing;
		double Fac_Hp;
		double Fac_Hc;
		double Fac_norm;
		
		double tder_hp,tder_hc; 
		
		Noise * NFhp;
		Noise * NFhc;
		
		int OrderLa ;
		//double tDelMax;
		
	public:
		// ** Constructor **
		GWSto();
		
		/*! \brief Constructs an instance and initializes 
		 * \arg Beta_n   : Ecliptic latitude
		 * \arg Lambda_n : Ecliptic longtude
		 * \arg tStep_n  : Sample time
		 * \arg Slope_n  : Slope (power of frequency) : \f$ \alpha \f$ in \f$f^{\alpha} \f$
		 * \arg Fknee_n  : Minimum value where the shape is \f$f^{\alpha} \f$. 
		 * \arg Fmin_n   : Maximum value where the shape is \f$f^{\alpha} \f$.
		 * \arg Fac_Hp_n : Multiplicatif parameter on the amplitude of Hp.
		 * \arg Fac_Hc_n : Multiplicatif parameter on the amplitude of Hc.
		 */	
		GWSto(	double Beta_n,
			  double Lambda_n,
			  double tStep_n,
			  double Slope_n, 
			  double Fknee_n , 
			  double Fmin_n,
			  double Fac_Hp_n,
			  double Fac_Hc_n);
		
		
		/* Constructs an instance and initializes
		 * \arg   Beta_n         : IN Beta and Lambda of the source.
		 * \arg   Lambda_n		 :
		 * \arg   tStep_n        : In Sample.
		 * \arg   tDurAdd_n      : In Number of events created with AddNoise.
		 * \arg   tFirst_n       : In Parameter for AddNoise.
		 * \arg   tLast_n        : In  Parameter for AddNoise.
		 * \arg   tder_n         : In  Parameter for AddNoise.
		 * \arg   Slope_n        : In Slope of f^Slope_n.
		 * \arg   Tsample_n      : In In general = tStep_n.
		 * \arg   Fknee_n        : In Minimum value where the shape = f^Slope.
		 * \arg   Fmin_n         : In Maximum value where the shape = f^Slope.
		 * \arg   Nb_Ageing_n    : Number of events for sabilization.
		 * \arg   Fac_Hp_n       : In Multiplicatif parameter on the amplitude of Hp.
		 * \arg   Fac_Hc_n       : In Multiplicatif parameter on the amplitude of Hc.
		 */
		GWSto( 	double Beta_n,
			  double Lambda_n,
			  double tStep_n,
			  double tDurAdd_n,
			  double tFirst_n,
			  double tLast_n,
			  double tder_n,
			  double Slope_n, 
			  double Tsample_n, 
			  double Fknee_n , 
			  double Fmin_n,
			  int Nb_Ageing_n,
			  double Fac_Hp_n,
			  double Fac_Hc_n);
		
		// ** Access methods **
		
		/*! \brief Sets parameters specified by iP :
		 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
		 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
		 * \arg 2 -> #Slope : \f$ \alpha \f$ : Slope (power of frequency \f$f^{\alpha} \f$) 
		 * \arg 3 -> #Fknee : \f$ f_{knee} \f$ : Minimum value where the shape is \f$f^{\alpha} \f$ (in Hertz)
		 * \arg 4 -> #Fmin : \f$ f_{min}\f$ : Maximum value where the shape is \f$f^{\alpha} \f$ (in Hertz)
		 * \arg 5 -> #Fac_Hp : \f$ C_{h+} \f$ :  Multiplicatif parameter on the amplitude of \f$ h_+ \f$
		 * \arg 6 -> #Fac_Hc : \f$ C_{h\times} \f$ :  Multiplicatif parameter on the amplitude of \f$ h_{\times} \f$
		 */
		void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		double getParam(int iP);
		
		
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

#endif // __GWSto_H
// end of LISACODE-GWSto.h


