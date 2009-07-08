// $Id:  $
/*
 *  LISACODE-NoiseOof.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 *
 *
 *  Created on 30/11/07 by Antoine PETITEAU (APC)
 *  Last modification on 30/11/07 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISEOOF_H
#define __NOISEOOF_H


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
* It creates a noise signal from a filtererd white noise.
* The filter is given by the next expression:
* \f[ y[n] = \sum_{k=1}^{N_a}{alpha[k] y[n-k]} + \sum_{k=0}^{N_b}{beta[k]x[n-k]}
 \f]
* where \f$N_a\f$ is the number of poles and \f$N_b\f$ is the number of zeros in the Z-transform.
*
*/

class NoiseOof : public Noise
{
protected:
    /*! \brief Vector of raw data before filtering. 
     *
     * White noise data are generated with a standard gaussian.*/
    vector<double> WhiteData; // List of raw data make with standard gaussian, before filter    
	 /*! \brief Minimal and maximal frequency for the range where the filter have the good shape.*/
	double fmin, fmax;
	 /*! \brief Powerof 1/f --> Shape : 1/f^alpha.*/
	double alpha;
	 /*! \brief Filter for the white noise used to generate the final noise.*/
    Filter NFilter; 	 // Noise filter
    int Nb_Ageing;
	
public:
		/* Constructor */
		/*Doc in .cpp*/
	NoiseOof();
	
    NoiseOof(	double tStep_n,		// IN : Time step
				double tDurAdd_n,	// IN : Time add for each adding of noise
				double tFirst_n,	// IN : Time of the first bin (>0)
				double tLast_n,		// IN : Time of the last bin (<0)
				double fmin,		// IN : Minimal frequency for the range where the filter have the good shape
				double fmax,		// IN : Maximal frequency for the range where the filter have the good shape
				double alpha,		// IN : Power of f  --> Shape : f^alpha
				int Nb_Ageing	// IN : Number of events before stabilization. 
				);
    
	
    /* Access methods */
	/*! \brief It returns the list of the alpha parameters (see Filter#alpha) of the filter #NFilter.*/ 
	vector< vector <double> > getFilterAlpha(){return(NFilter.getAlpha());};
	/*! \brief It returns the list of the beta parameters (see Filter#beta) of the filter #NFilter.*/ 
	vector< vector <double> > getFilterBeta(){return(NFilter.getBeta());};

	
    /*  Others methods */
    /*Doc in .cpp*/
    void loadNoise();  // Load noise for initialization   
    void generNoise(int StartBin); // Generation of one noise for one step.	
};

#endif // __NOISEFILTER_H

// end of LISACODE-NoiseOof.h

