// $Id: LISACODE-NoiseWhite.cpp,v 1.6 2007/04/20 12:05:57 ruiloba Exp $
/*
 *  LISACODE-NoiseWhite.cpp
 *  V 1.4
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-NoiseWhite.h"

/* Constructor */
/*! \brief Base constructor.
 *
 * It sets default values for class attributes and generate white noise vector (see #loadNoise).\n
 * #Sigma(\f$\sigma\f$) is computed by #setSqPSD from the root square of the PSD set to 1.0e-13.
 * \arg tStep = 0.01
 * \arg tDurAdd = 1.0
 * \arg tFirst = 5.0
 * \arg tLast = -20.0
 * \arg NoiseType = White 
*/
NoiseWhite::NoiseWhite()
{
    tStep = 0.01;
    tDurAdd = 1.0;
    tFirst = 5.0;
    tLast = -20.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    setSqPSD(1.0e-13);
    loadNoise();
	strcpy(NoiseType,"White");
}

/*! \brief Constructor.
 *
 * It sets values for class attributes and computes a white noise vector such 
 * it is done by the base constructor (NoiseWhite#NoiseWhite()). Sigma(\f$\sigma\f$)  is
 * computed from a by default PSD value.\n
 * #NoiseType attribute is set to \p White. \n
 * #Sigma(\f$\sigma\f$)  is computed by #setSqPSD from the root square of the PSD set to 1.0e-13.
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 */
NoiseWhite::NoiseWhite(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    setSqPSD(1.0e-13);
    loadNoise();
	strcpy(NoiseType,"White");
    //cout << "  - NoiseData size = " << NoiseData.size() << endl;
    //for(int i=0; i<10; i++)
    //	cout << "    NoiseData[i] = " << NoiseData[i] << endl;
	
}

/*!\brief Constructor.
 *
 * It sets values for class attributes and computes a white noise vector such 
 * it is done by the base constructor (NoiseWhite#NoiseWhite()).\n
 * #NoiseType attribute is set to \p White. 
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 * \param SqPSD Value of root square to compute #Sigma (see #setSqPSD()).
 * 
 */
NoiseWhite::NoiseWhite(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n, double SqPSD)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
    NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	setSqPSD(SqPSD);
    loadNoise();
	strcpy(NoiseType,"White");
    //cout << "  - NoiseData size = " << NoiseData.size() << endl;
    //for(int i=0; i<10; i++)
    //    cout << "    NoiseData[i] = " << NoiseData[i] << endl;
}



/* Access methods */
/*! It returns the power spectrum density (PSD) from #Sigma(\f$\sigma\f$) value.
    The equation used to calculate the PSD is:
    \f[PSD=\sigma^2 \cdot 2.0\cdot tStep\f]
    where \f$tStep\f$ is the time between to samples.
*/
double NoiseWhite::getPSD()
{
    return(Sigma*Sigma*2.0*tStep);
}

/*! It returns the root square of power spectrum density (SqPSD).
    It is computed from #Sigma(\f$\sigma\f$)  value using the equation:
     \f[SqPSD=\sigma \cdot \sqrt{2.0 \cdot tStep}\f]
    where \f$tStep\f$  is the time between to samples.
*/

double NoiseWhite::getSqPSD()
{
    return(Sigma*sqrt(2.0*tStep));
}
/*! It sets #Sigma(\f$\sigma\f$)  by giving the root square of power spectrum density (PSD).
    #Sigma(\f$\sigma\f$)  is computed by using the equation:
		\f[\sigma = \frac{SqPSD}{\sqrt{2.0 \cdot tStep}}\f]
    where \f$tStep\f$  tStep is the time between to samples.
    If \f$SqPSD\f$ is negatif a message is shown.
*/
void NoiseWhite::setSqPSD(double SqPSD)
{
    if ( SqPSD < 0.0 )
        throw invalid_argument("Noise::setPSD : Power Spectral Density cannot be negative !");
	Sigma = SqPSD/sqrt(2.0*tStep);
}


/* Others methods */
/*! \brief Initialization.
 *
 * #NoiseData size is set to #NbData. \n
 * #NoiseData is  generated as below :
 * \f[  \textrm{ for i=0,\dots,NbData } WhiteData[i] = \sigma \cdot \sqrt{- 2 \cdot log(r2)} \cdot cos(2 \cdot \pi \cdot r1) \f]
 *  where r1 and r2 are random values between 0 and 1 (using #genunf).
 * 
*/ 
void NoiseWhite::loadNoise()
{
	NoiseData.resize(NbData);
    // Generation of all data for white noise
    double r1(0.0), r2(0.0);
	//long is1, is2;
	//getsd(&is1,&is2);
	//cout << "Noise : is1 = " << is1 << " et is2 = " << is2 << endl;
    for(int i=0; i<NbData; i++){
        //r1 = (double)rand()/RAND_MAX;
        //r2 = (double)rand()/RAND_MAX;
		r1 = (double)genunf(0.0, 1.0);
        r2 = (double)genunf(0.0, 1.0);
        NoiseData[i] = Sigma*sqrt(-2.0*log(r2))*cos(2*M_PI*r1);
    }
	//getsd(&is1,&is2);
	//cout << "Noise : is1 = " << is1 << " et is2 = " << is2 << endl;
    //cout << "  - NbData = " << NbData << endl;
}

/*! \brief Noise generation (for one measurement), using Startbin input as beginning index.
 *
 * #NoiseData size is set to #NbData. \n
 * #NoiseData is  generated as below :
 * \f[  \textrm{ for i=0,\dots,StartBin } WhiteData[i] = \sigma \cdot \sqrt{- 2 \cdot log(r2)} \cdot cos(2 \cdot \pi \cdot r1) \f]
 *  where r1 and r2 are random values between 0 and 1 (using #genunf).
 */ 
void NoiseWhite::generNoise(int StartBin)
{
    double r1(0.0), r2(0.0);
    // Generation of white noise
	for(int i=StartBin; i>=0; i--){
        //r1 = (double)rand()/RAND_MAX;
        //r2 = (double)rand()/RAND_MAX;
		r1 = (double)genunf(0.0, 1.0);
        r2 = (double)genunf(0.0, 1.0);
		NoiseData[i] = Sigma*sqrt(-2.0*log(r2))*cos(2*M_PI*r1);
		//cout << NoiseData[i] << endl;
	}
	//cout << " WhiteNoise generNoise ( " << StartBin << " ) "<< endl;
}


// end of LISACODE-NoiseWhite.cpp
