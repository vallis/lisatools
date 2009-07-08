// $Id: LISACODE-NoiseWhite.h,v 1.6 2007/03/27 13:26:57 lalanne Exp $
/*
 *  LISACODE-NoiseWhite.h
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  DERIV CLASS : Noise
 * ----------
 * Creation d'un bruit blanc de sigma donne. Ce sigma est obtenu a partir de la
 * Densite Spectrale de Puissance par Sigma = PSD / sqrt(tStep)
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#ifndef __NOISEWHITE_H
#define __NOISEWHITE_H


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
using namespace std;

/*!\ingroup noise*/
/** 
*  Noise derived  class to treat white noise.
* It creates a white noise using a given \f$\sigma\f$.
* The \f$\sigma\f$ value is obtained from the power spectral density 
* following the next expression:
* \f[\sigma = \sqrt{\frac{PSD}{2 \cdot tStep}} \f]
* where \f$PSD\f$ is the power spectral density and \f$tStep\f$ is the time between
* two samples.
*
*/
class NoiseWhite : public Noise
{
protected:
    /*! \warning Definition incoherent with class detailed description*/
    /*! \brief White noise standard deviation. */
    double Sigma;// Sigma of white noise : Root square Power Spectral Density (in unit.Hz-1) over root square 2.
	
public:
    /* Constructor */
    /*Doc in .cpp*/
    NoiseWhite();
    NoiseWhite(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n);
    NoiseWhite(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, double SqPSD);
	
    /* Access methods */
    /*Doc in .cpp*/
    double getPSD();
    double getSqPSD();
    void setSqPSD(double SqPSD);
	
	
    /*  Others methods */
    /*Doc in .cpp*/
    void loadNoise();    // Load noise for initialization 	
    void generNoise(int StartBin); 	// Generation of noise.
};

#endif // __NOISEWHITE_H

// end of LISACODE-NoiseWhite.h

