// $Id: LISACODE-GWFile.h,v 1.6 2007/04/20 12:28:19 ruiloba Exp $
/*
 *  LISACODE-GWFile.h
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

#ifndef __GWFILE_H
#define __GWFILE_H


#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Couple.h"
#include "LISACODE-GW.h"
using namespace std;
/*!\ingroup gw
*/
/*! \brief Gravitational Waves file management.
 *
 * Temporal polarization constraints are read from file.
 */
class GWFile : public GW
{
protected:
  /*! \brief Vector of time values associated to components samples */
	double * TimeList;
  /*! \brief List of (h_plus,h_cross)=(hp,hc) polarisation components */
	Couple *  hList;
  /*! \brief Last bin read */
	int LastUsedBin;
  /*! \brief Number of data */
	int NbDat;
	/*! \brief Name of file which contains data */
	char FileName[256];
	/*! \brief Type of file encoding :  0->ASCII, 1->Binary*/
	int FileEncoding;
	/*! \brief Time offset and time step */
	double TimeOffset, TimeStep;
	/*! \brief Size of data in file */
	int Length, Records; 
	 
public:
	// ** Constructor **
	GWFile();
	GWFile(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName);
	GWFile(double Beta_n, 
	       double Lambda_n, 
	       double AnglPol_n, 
	       char * FileName, 
	       int FileEncoding, 
	       double TimeOffset, 
	       double TimeStep, 
	       int Length_n, 
	       int Records_n);
	~GWFile();
	
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
	unsigned int getNbStored() {return(NbDat);}; 
	
	
	// ** Others methods **
	/*! \brief Initialization : ... */
	void init();
	/*! \brief Load ASCII file specified in argument*/
	void ReadASCIIFile(char * FileName, double TimeOffset, double TimeStep); // Load ASCII file specified in argument
	/*! \brief Load binary file specified in argument*/
	void ReadBinaryFile(char * FileName, double TimeOffset, double TimeStep, int Length, int Record);
	/*! \brief Interpol with Lagrange in specified type (1 -> h+ or 2 -> hx)  */
	double Interpol(double t, int type);
	/*! \brief Return \f$ h_{+}(t) \f$ */
	double hp(double t); 
	/*! \brief Return \f$ h_{\times}(t) \f$ */
	double hc(double t);
	/*! \brief Display ...  [for specific use or debug]\f$ */
	void DispTempVal(double t, ostream * OutDisp);
	
};

#endif // __GWFILE_H

// end of LISACODE-GWFile.h

