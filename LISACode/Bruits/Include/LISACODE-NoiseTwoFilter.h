// $Id:  $
/*
 *  LISACODE-NoiseTwoFilter.h
 *  V 1.3.1
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 * Cretion of filtered noise with PSD = Filter1(WhiteNoise1) + Filter2(WhiteNoise2)
 * The two white noise are generated independently.
 *
 *  Created on 18/10/07 by Eric Plagnol (APC)
 *  Last modification on 21/12/07 by Antoine PETITEAU, Eric Plagnol (APC)
 *
 */

#ifndef __NOISEFILTER_EP_H
#define __NOISEFILTER_EP_H


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

class NoiseTwoFilter : public Noise
{
protected:
    /*! \brief Vector of raw data before filtering. 
     *
     * White noise data are generated with a standard gaussian.*/
    vector<double> WhiteData; // List of raw data make with standard gaussian, before filter    
    vector<double> WhiteData2; // List of raw data make with standard gaussian, before filter    
    vector<double> NoiseData_tmp1; // List of noise data.
    vector<double> NoiseData_tmp2; // List of noise data.
    /*! \brief Filter for the white noise used to generate the final noise.*/
    Filter NFilter; 	 // Noise filter
    Filter NFilter_2; 	 // Noise filter
	
public:
		/* Constructor */
		/*Doc in .cpp*/
		NoiseTwoFilter();
		
		NoiseTwoFilter(	double tStep_n,
				double tDurAdd_n,
				double tFirst_n,
				double tLast_n,
				Filter NFilter_n,
				Filter NFilter_n2); // ?????
	
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

// end of LISACODE-NoiseTwoFilter.h

