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
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
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
	vector<double> TimeList;
  /*! \brief List of (h_plus,h_cross)=(hp,hc) polarisation components */
	vector<Couple> hList;
  /*! \brief Last bin read */
	int LastUsedBin;
	
public:
		/* Constructor */
		GWFile();
	GWFile(double Beta_n, double Lambda_n, double AnglPol_n);
	GWFile(double Beta_n, double Lambda_n, double AnglPol_n, char * FileName);
	~GWFile();
	
	/* Access methods */
/*! \brief Returns size of #TimeList attribute */
	unsigned int getNbStored() {return(TimeList.size());}; 
	
	/*  Others methods */
	void ReadFile(char * FileName); // Load file specified in argument
	double Interpol(double t, int type); // Interpol with Lagrange in specified type (1 -> h+ or 2 -> hx)
	double hp(double t); //Return h_plus  
	double hc(double t); //Return h_croix
	
};

#endif // __GWFILE_H

// end of LISACODE-GWFile.h

