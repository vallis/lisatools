// $Id: LISACODE-Filter.h,v 1.6 2007/03/29 15:15:10 lalanne Exp $
/*
 *  Filter.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  TOOLS CLASS
 * ----------
 * Applique un filtre sur des donnees
 *
 *
 *  Created on 25/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 25/10/05 by Antoine PETITEAU (APC)
 *
 */

/*!\ingroup mathTools
 *\defgroup filter Filter
 * \{
 */
#ifndef __FILTER_H
#define __FILTER_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-EllipticFilter.h"

using namespace std;

/*!\brief filter management class.
 */
class Filter 
{
protected:
	/*! \brief Alpha parameters list. */
	vector< vector<double> > alpha;
	/*! Beta parameters list. */
	vector< vector<double> > beta;
	/*! \brief Number of data for stabilization. */
	int NbDataStab;
	/*! \brief Temporary data. */
 	vector< vector<double> > TmpData;
	
public:
	/* Constructor */
	Filter();
	
	Filter(	vector< vector<double> > alpha_n,
			vector< vector<double> > beta_n);
	
	Filter(	vector< vector<double> > alpha_n,
			vector< vector<double> > beta_n,
			int NbDataStabilization_n);
	
	Filter( double fe,    // sampling frequency [Hz]
			double at,    // attenuation [dB]
			double bp,    // oscillations in bandwidth [dB]
			double fb,    // low transition frequency [Hz]
			double fa);    // high transition frequency [Hz]
			
			
	
	~Filter();
	
	/* Access methods */
	void init(	vector< vector<double> > alpha_n,
				vector< vector<double> > beta_n,
				int NbDataStabilization_n);
    int getDepth();
	int getNbDataStab();
  /*! \brief Returns #alpha attribute.*/
	vector< vector <double> > getAlpha(){return(alpha);};
  /*! \brief Returns #beta attribute.*/
	vector< vector <double> > getBeta(){return(beta);};
	
	/*  Others methods */
	void App(int StartBin, const vector<double> & RawData, vector<double> & FilterData); 
};

/*!\}*/
#endif // __FILTER_H

// end of Filter.h

