// $Id:  $
/*
 *  LISACODE-NoiseFShape.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 * Creation of noise from many filter in parallele. Structure a_-NFm f^-NFm + ... + a_-1 f^-1 + a_0 + a_1 f + a_2 f^2 + ... + a_NFp f^NFp
 *
 *
 *  Created on 31/08/07 by Antoine PETITEAU (APC)
 *  Last modification on 29/11/07 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISEFSHAPE_H
#define __NOISEFSHAPE_H


#include <stdexcept>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include "randlib.h"
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Noise.h"
#include "LISACODE-Filter.h"
using namespace std;
/*!\ingroup noise*/
/** 
* \brief Noise derived  class to treat noise filters.
*
* It creates a noise signal from a filtered white noise.
*
*/

class NoiseFShape : public Noise
{
protected:
    /*! \brief Vector of raw data before filtering. 
	*
	* White noise data are generated with a standard gaussian.*/
    vector<double> WhiteData; // List of raw data make with standard gaussian, before filter    
    /*! \brief List of filter in f.*/
    vector<Filter> FilterFp; 
	/*! \brief List of filter in 1/f.*/
	vector<Filter> FilterFm; 
	/*! \brief Temporary data for positive power of f  */
	vector< vector<double> > tmpData_p;
	/*! \brief Temporary data for negative power of f */
	vector< vector<double> > tmpData_m;
	/*! \brief Number of positive power of f (f^0, f^1, f^2, ...).*/
	int NFp;
	/*! \brief Number of negative power of f (f^-1, f^-2, ...).*/
	int NFm;
	/*! \brief Data filtered in f, f^2, f^3, ...*/
	vector< vector<double> > FpData ; 
	/*! \brief Data filtered in f^-1, f^-2, f^-3, ...*/
	vector< vector<double> > FmData ;
	/*! \brief Factor on f^0*/
	double FactF0;
	/*! \brief Factor on positive power of f*/
	double * FactFp;
	/*! \brief Factor on negative power of f*/
	double * FactFm;
	
	
public:
	/* Constructor */
	/*Doc in .cpp*/
	NoiseFShape();	
    NoiseFShape(	double tStep_n,		
					double tDurAdd_n,
					double tFirst_n,
					double tLast_n,
					int NFp_n,			// IN : Number of positive power of f  ( f^1, f^2, ...)
					int NFm_n,			// IN : Number of negative power of f (f^-1, f^-2, ...)
					double * FactF_n	// IN : Factoron f power : a_-NFm f^-NFm + ... + a_-1 f^-1 + a_0 + a_1 f + a_2 f^2 + ... + a_NFp f^NFp
					);
    
	
    /* Access methods */
	
	
    /*  Others methods */
    /*Doc in .cpp*/
    void loadNoise();  // Load noise for initialization   
    void generNoise(int StartBin); // Generation of one noise for one step.	
};

#endif // __NOISEFSHAPE_H

// end of LISACODE-NoiseFShape.h