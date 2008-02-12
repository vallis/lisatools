// $Id: LISACODE-NoiseFilter.h,v 1.7 2007/04/05 09:31:44 ruiloba Exp $
/*
 *  LISACODE-NoiseFilter.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 * Creation de bruits a partir d'un bruit blanc filtrer. Le filtre est
 * de la forme : y[n] = sum(k=1, Na, alpha[k] y[n-k]) + sum(k=0, Nb, beta[k] x[n-k])
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISEFILTER_H
#define __NOISEFILTER_H


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

class NoiseFilter : public Noise
{
protected:
    /*! \brief Vector of raw data before filtering. 
     *
     * White noise data are generated with a standard gaussian.*/
    vector<double> WhiteData; // List of raw data make with standard gaussian, before filter    
    /*! \brief Filter for the white noise used to generate the final noise.*/
    Filter NFilter; 	 // Noise filter
	
public:
		/* Constructor */
		/*Doc in .cpp*/
		NoiseFilter();
    NoiseFilter(	double tStep_n,
					double tDurAdd_n,
					double tFirst_n,
					double tLast_n);
	
    NoiseFilter(	double tStep_n,
					double tDurAdd_n,
					double tFirst_n,
					double tLast_n,
					Filter NFilter_n);
    
	NoiseFilter(	double tStep_n,
					double tDurAdd_n,
					double tFirst_n,
					double tLast_n,
					vector< vector<double> > FilterAlpha_n,
					vector< vector<double> > FilterBeta_n);
	
	NoiseFilter(	double tStep_n,
					double tDurAdd_n,
					double tFirst_n,
					double tLast_n,
					vector< vector<double> > FilterAlpha_n,
					vector< vector<double> > FilterBeta_n,
					int FilterNbDataStab_n);
    //NoiseFilter(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, double PSDFactor, double PSDPower);
    //NoiseFilter(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, char * PSDFileName);
	
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

// end of LISACODE-NoiseFilter.h

