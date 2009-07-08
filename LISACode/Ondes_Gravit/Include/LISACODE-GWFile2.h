// $Id: LISACODE-GWFile.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWFile2.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * 
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#ifndef __GWFILE2_H
#define __GWFILE2_H


#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
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
class GWFile2 : public GW
{
protected:
	/*! \brief List of polarisation component h+ */
	Serie2 *  hps;
	/*! \brief List of polarisation component h+ */
	Serie2 *  hcs;
	/*! \brief Number of data */
	int N;
	/*! \brief Time offset and time step */
	double t0, dt, tend;
	/*! \brief Name of file which contains data */
	char FileName[256];
	/*! \brief Type of file encoding :  0->ASCII, 1->Binary*/
	int FileEncoding;
	/*! \brief Size of data in file */
	int NRec;
	/*! \brief Type of interpolation */
	INTERP InterpType;
	/*! \brief Order of interpolation */
	int InterpVal;
	
	
public:
	// ** Constructor **
	GWFile2();
	GWFile2(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName);
	GWFile2(double Beta_n, 
			double Lambda_n, 
			double AnglPol_n, 
			char * FileName, 
			int FileEncoding, 
			double TimeOffset, 
			double TimeStep, 
			int Length_n, 
			int Record_n);
	~GWFile2();
	
	// ** Access methods **
	
	/*! \brief Sets parameters specified by iP :
	 * \arg 0 -> #Beta : \f$ \beta \f$ : Ecliptic latitude (in radians) 
	 * \arg 1 -> #Lambda : \f$ \lambda \f$ : Ecliptic longitude (in radians) 
	 * \arg 2 -> #AnglPol : \f$ \psi \f$ : Polarization angle (in radians) 
	 */
	void setParam(int iP, double Param_n);
	/*! \brief Return parameters specified by iP (see function for #setParam the iP code) */
	double getParam(int iP);
	void setFileName(char * FileName_n);
	char * getFileName();
	/*! \brief Returns size of #TimeList attribute */
	unsigned int getNbStored() {return(N);}; 
	void setInterp(INTERP InterpType_n, int InterpVal_n) {InterpType = InterpType_n; InterpVal = InterpVal_n;};
	
	
	// ** Others methods **
	/*! \brief Initialization : ... */
	void init();
	/*! \brief Load ASCII file specified in argument*/
	void ReadASCIIFile();
	/*! \brief Load binary file specified in argument*/
	void ReadBinaryFile();
	/*! \brief Return \f$ h_{+}(t) \f$ */
	double hp(double t); 
	/*! \brief Return \f$ h_{\times}(t) \f$ */
	double hc(double t);
	/*! \brief Display ...  [for specific use or debug]\f$ */
	void DispTempVal(double t, ostream * OutDisp);
	
};

#endif // __GWFILE2_H

// end of LISACODE-GWFile2.h

