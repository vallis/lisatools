// $Id: LISACODE-GWBuffer.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWBuffer.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 27/05/09 by Antoine PETITEAU (AEI)
 *  Last modification on 27/05/09 by Antoine PETITEAU (AEI)
 *
 */

#ifndef __GWBUFFER_H
#define __GWBUFFER_H


#include <stdexcept>
#include <iostream.h>
#include <fstream>
#include <vector.h>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Serie2.h"
#include "LISACODE-GW.h"

using namespace std;
/*!\ingroup gw
 */
/*! \brief Gravitational Waves file management.
 *
 * Temporal polarization constraints are read from file.
 */
class GWBuffer : public GW
	{
	protected:
		/*! \brief GW pointer on gravitational wave source */
		GW * SrcGW;
		/*! \brief List of h+ data */
		Serie2 * hps;
		/*! \brief List of hx data */
		Serie2 * hcs;
		
		/*! \brief Number of data */
		int N;
		/*! \brief Time offset */
		double t0;
		/*! \brief Time step */
		double dt;
		
		/*! \brief Extra time : need for taking into account Doppler effect  */
		double tExtra;
		
		INTERP InterpType;
		int InterpVal;
		
		
	public:
		// ** Constructor **
		GWBuffer();
		GWBuffer(GW * SrcGW_n, double t0_n, double dt_n, double tend, double tExtra_n, INTERP InterpType_n = LAG, int InterpVal_n = 4);
		GWBuffer(double t0_n, double dt_n, double tend, double tExtra_n, INTERP InterpType_n = LAG, int InterpVal_n = 4);
		~GWBuffer();
		
		// ** Access methods **
		/*! \brief Sets parameters specified by iP : bridge with parameter of SrcGW */
		void setParam(int iP, double Param_n);
		/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
		double getParam(int iP);
		double gettShift() {return(t0-tExtra);};
		
		// ** Others methods **
		/*! \brief Initialization : Compute and store hp hc */
		void init();
		/*! \brief Return \f$ h_{+}(t) \f$ */
		double hp(double t); 
		/*! \brief Return \f$ h_{\times}(t) \f$ */
		double hc(double t);
		/*! \brief Display ...  [for specific use or debug]\f$ */
		void DispTempVal(double t, ostream * OutDisp);
		
		void sethphcAll(double * hp_n, double * hc_n, double tstart, int Npts);
		void sethphcBin(double hp_n, double hc_n, int i);
		void sethphcNull();
		
	};

#endif // __GWBUFFER_H

// end of LISACODE-GWBuffer.h

