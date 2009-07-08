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
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
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
#include "LISACODE-Tools.h"
//#include "LISACODE-Couple.h"

using namespace std;

/*! \brief Gravitational Waves parameters are described in this class.
 */

class GW 
	{
	protected:
		Tools * MT;
		/*! \var Beta
		 \brief Ecliptic latitude Source direction angle (in the heliocentric reference frame) in radians. */
		double Beta;
		/*! \var Lambda
		 \brief Ecliptic longitude Source direction angle (in the heliocentric reference frame) in radians. */
		double Lambda; 
		/*! \brief Source direction unit vector (in the heliocentric reference frame). */
		vector<double> DirProp; 
		/*! \brief Polarisation angle */
		double AnglPol;
		/*! \brief Number of parameters */
		int NParam;
		/*! \brief Minimal and maximal frequency of GW (-1.0 --> undefined) */
		double FreqMin, FreqMax;
		
	public:
		// ** Constructor **
		/*! \brief Constructs an instance and initializes it with default values. */
		GW();
		/*! \brief Constructs an instance and initializes it with inputs for sky location. */
		GW(double Beta_n, double Lambda_n);
		/*! \brief Constructs an instance and initializes it with inputs for sky location and polarization angle. */
		GW(double Beta_n, double Lambda_n, double AnglPol_n);
		/*! \brief Destructor */
		virtual ~GW();
		
		// ** Access methods **
		/*! \brief Sets parameters specified by iP :
		 * \arg	0 -> #Beta : Ecliptic latitude (in radians) 
		 * \arg 1 -> #Lambda : Ecliptic longitude (in radians) 
		 * ... [Redefine in derived class]
		 */
		virtual void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		virtual double getParam(int iP);
		int getNParam(){return(NParam);};
		double getBeta() const {return(Beta);};
		void setBeta(double Beta_n);
		double getLambda() const {return(Lambda);};
		void setLambda(double Lambda_n);
		vector<double> getDirProp() const {return(DirProp);};
		/*! \brief Sets #DirProp, #Lambda and #Beta attributes. */
		void setDirProp(vector<double> DirProp_n);
		double getAnglPol() {return(AnglPol);}; //Return polarisation angle
		void setAnglPol(double AnglPol_n);
		
		double getFreqMin() {return(FreqMin);};
		double getFreqMax() {return(FreqMax);};
		
		// ** Others methods **
		/*! \brief Initialization : ... */
		virtual void init();
		/*! \brief Return \f$ h_{+}(t) \f$ */
		virtual double hp(double t); 
		/*! \brief Return \f$ h_{\times}(t) \f$ */
		virtual double hc(double t); 
		/*! \brief Display ...  [for specific use or debug]\f$ */
		virtual void DispTempVal(double t, ostream * OutDisp);
		/*! \brief Computes components of the unit vectors #DirProp from the angles #Lambda and #Beta */
		void CalculDirProp();
		
		void setTools(Tools * MT_n) {delete MT; MT = MT_n;};
		
	};

/*!\} */ 
#endif // __GW_H

// end of LISACODE-GW.h

