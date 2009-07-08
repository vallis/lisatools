// $Id: LISACODE-GWCusp.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
// template for a new GW (copied on GWMono)
/*
 *  LISACODE-GWCusp.h
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

#ifndef __GWCUSP_H
#define __GWCUSP_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "fftw3.h"
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
 */
/*! \brief Gravitational Waves instantaneous parameters \f$ h_+ \f$ and \f$ h_{\times} \f$ are described in this class.
 */
class GWCusp : public GW
	{
	protected:
		// *************** Parameters of system ***************
		
		/*! \brief Amplitude in ()
		 *
		 * \f$ A = {G \mu L^{2/3} \over D_{L}}\f$ 
		 * with 
		 * \arg \f$ G \f$ : Newton constant
		 * \arg \f$ \mu \f$ : String mass per unit length
		 * \arg \f$ L \f$ : Size of the feature that produces cusp
		 * \arg \f$ D_{L} \f$ : Luminosity distance
		 */
		double Amplitude; 
		/*! \brief Central time  */
		double CentralTime; 
		/*! \brief Maximum frequency */
		double MaximumFrequency;
		
		
		// *************** Time parameters ***************
		/*! \brief Time offset */
		double Toffset;
		/*!  \brief Duration of observation */
		double Tobs;
		/*!  \brief Time step */
		double Tstep;
		/*! \brief Time for ... */
		double T0, Tpad;
		
		// *************** Variables used for computation ***************
		/*!  \brief Number of samples */
		long NtDat;
		/*!  \brief Time data of \f$ h(t) \f$ */
		double * th;
		/*!  \brief Number of samples */
		long NfDat;
		/*!  \brief Fourier data of \f$ h(f) \f$ */
		fftw_complex * fh;
		/*!  \brief FFTW plan \f$ */
		fftw_plan FTRevPlan ;
		/*!  \brief Duration of the burst and ... \f$ */
		double Tburst, Tback;
		
		// *************** Internal constants ***************
		double Flow, mTPI, m43; 
		
		
	public:
		// ** Constructor **
		GWCusp();
		GWCusp(double TStep_n, double Tobs_n, double Tpad_n, double Toffset_n);
		GWCusp(double Beta_n, 
			   double Lambda_n, 
			   double AnglPol_n,
			   double Amplitude_n,
			   double CentralTime_n,
			   double MaximumFrequency_n,
			   double TStep_n,
			   double Tobs_n,
			   double Tpad_n,
			   double Toffset_n);
		~GWCusp();
		
		// ** Access methods **
		
		/*! \brief Sets parameters specified by iP :
		 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
		 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
		 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
		 * \arg 3 -> #Amplitude : \f$ A \f$ : Amplitude (in \f$ Hz^{1/3} \f$)
		 * \arg 4 -> #CentralTime : \f$ t_{cen} \f$ : Central time (in seconds)
		 * \arg 5 -> #MaximumFrequency : \f$ f_{max} \f$ : Maximum frequency (in Hertz)
		 */
		void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		double getParam(int iP);
		
		
		// ** Others methods **
		/*! \brief Initialization of constants */
		void initConst();
		/*! \brief Allocation of memories */
		void AllocMem();
		/*! \brief Initialization : Compute GW strain in Fourier domain and compute the inverse Fourier transform */
		void init();
		/*! \brief Return \f$ h_{+}(t) \f$ */
		double hp(double t); 
		/*! \brief Return \f$ h_{\times}(t) \f$ */
		double hc(double t); 
		/*! \brief Display ...  [for specific use or debug]\f$ */
		void DispTempVal(double t, ostream * OutDisp);
		
		
	};

#endif // __GWCUSP_H

// end of LISACODE-GWCusp.h

