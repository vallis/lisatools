// $Id:  Exp $
/*
 *  LISACODE-NoiseOof.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 30/11/07 by Antoine PETITEAU (APC)
 *  Last modification on 30/11/07 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-NoiseOof.h"


/* Constructor */
/*!\brief Base constructor.
 *
 * It sets default values for class attributes. It initializes the filter and applies it to a white noise to generate the final noise (#loadNoise).
 * \arg tStep = 0.01
 * \arg tDurAdd = 1.0
 * \arg tFirst = 5.0
 * \arg tLast = -20.0
 * \arg NoiseType = Filter 
 * \arg alpha coefficients of #NFilter = 0 (see Filter#alpha)
 * \arg beta parameter of #NFilter = 1 (see Filter#beta)
 * \arg NbDataStab parameter of #NFilter = 0 (see Filter#NbDataStab)
*/
NoiseOof::NoiseOof()
{
    tStep = 0.01;
    tDurAdd = 1.0;
    tFirst = 5.0;
    tLast = -50.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	
    vector< vector<double> > alpha_tmp(0);
	vector< vector<double> > beta_tmp(0);
	vector<double> TmpVect(1);
	TmpVect.push_back(1.0);
	beta_tmp.push_back(TmpVect);
	NFilter.init(alpha_tmp, beta_tmp, 0);
    loadNoise();
	strcpy(NoiseType,"Oof");
}

/* Constructor */
/*!\brief Constructor.
 *
 * It sets some class attributes. The other attributes are set 
 * to default values (see NoiseOof#NoiseOof()). 
 * It acts like the base constructor (NoiseOof#NoiseOof()).
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast. 
 * \param fmin_n Value of #fmint.
 * \param fmax_n Value of #fmax. 
 * \param alpha_n Value of #alpha.

*/

NoiseOof::NoiseOof(	double tStep_n,		// IN : Time step
					double tDurAdd_n,	// IN : Time add for each adding of noise
					double tFirst_n,	// IN : Time of the first bin (>0)
					double tLast_n,		// IN : Time of the last bin (<0)
					double fmin_n,		// IN : Minimal frequency for the range where the filter have the good shape
					double fmax_n,		// IN : Maximal frequency for the range where the filter have the good shape
					double alpha_n,		// IN : Power of f  --> Shape : f^alpha
					int Nb_Ageing_n	    // IN : Number of events before stabilization. 
					)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
	fmin = fmin_n;
	fmax = fmax_n;
	alpha = alpha_n;
	Nb_Ageing = Nb_Ageing_n;
/*
	cout << "fmin  = " << fmin << endl;
	cout << "fmax  = " << fmax << endl;
	cout << "alpha = " << alpha << endl;
*/
	
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);

	
	
	vector< vector<double> > alpha_tmp(0);
	vector< vector<double> > beta_tmp(0);
	
	//  Calcul des poles.
	
	double w0,w1,wmax,wmin;
	double p0,z0,dp;
	double p,z,pcourant,zcourant,dpcourant,den,tStep_inv;
	int nproc;
	
	w0   = 2*M_PI*fmin;
	w1   = 2*M_PI*fmax;
	wmax = log10(w1);
	wmin = log10(w0);

//	printf(" w0 = %f , w1 = %f ,  wmax = %f, wmin = %f, tStep = %f  \n", w0,w1, wmax,wmin,tStep);	

	tStep_inv = 1./tStep;

	nproc = (int)((wmax-wmin)*2.+log10(tStep_inv));  // Nombres de poles.


	alpha_tmp.resize(nproc);
	beta_tmp.resize(nproc);
		
	dp = (wmax-wmin)/nproc;
	p0 = wmin+(1+alpha/2)/2*dp;
	z0 = p0-alpha/2*dp;
	pcourant = p0;
	zcourant = z0;
	dpcourant = 0.;

	for(int i=0;i<nproc;i++)
	{ 
		p = pcourant+dpcourant;
		dpcourant =	(wmax-wmin)/nproc;
		z = p-alpha/2*dp;
		w0 = (pow(10,p)/2)*tStep;
		w1 = (pow(10,z)/2)*tStep;
		pcourant = p;
		zcourant = z;
		den = 1+w0;
		alpha_tmp[i].resize(1);
		beta_tmp[i].resize(2);
		alpha_tmp[i][0]=(1-w0)/den;
		beta_tmp[i][0]=(1+w1)/den;
		beta_tmp[i][1]=-(1-w1)/den;


//		printf(" filtre %d : alpha0 = %f , beta0 = %f ,  beta1 = %f \n", i, alpha_tmp[i][0], beta_tmp[i][0], beta_tmp[i][1]);

		
	} 
		
		
	NFilter.init(alpha_tmp, beta_tmp, Nb_Ageing);
	
	//NFilter.init(alpha_tmp, beta_tmp, 0);
    loadNoise();
    ///cout << "  - WhiteData size = " << WhiteData.size() << endl;
    ///cout << "  - NoiseData size = " << NoiseData.size() << endl;
    ///for(int i=0; i<10; i++)
    ///	cout << "    WhiteData[i] = " << WhiteData[i] << "  -    NoiseData[i] = " << NoiseData[i] << endl;
	strcpy(NoiseType,"Oof");
}


/* Access methods */


/* Others methods */
/*! \brief Initialization.
 *
 * Number of bins must be adjusted depending on filter stabilization :
 * \f[ NbDataStab= 2 \cdot  NbData + NbDataStab_{NFilter} + Max(size(\alpha_{NFilter}),size(\beta_{NFilter})) \f]
 * #WhiteData size and #NoiseData size are set to NbDataStab.
 *
 * WhiteData is  generated as a standard gaussian :
 * \f[  \textrm{ for i=0,\dots,Startbin } WhiteData[i] = \sqrt{-\frac{log(r1)}{tStep} \cdot cos(2 \cdot \pi \cdot r1)} \f]
 *  where r1 and r2 are random values between 0 and 1 (using #genunf).
 * 
 * Noise data are generated using Filter::App method with  StartBin, WhiteData, and NoiseData arguments.\n
 * Then last data are deleted : #WhiteData size and #NoiseData size are set to #NbData.
 *
 */
void NoiseOof::loadNoise()
{
	int NbDataStab(2*NbData + NFilter.getNbDataStab() + NFilter.getDepth()); //More bin that necessary for stabilization of the filter
    int StartBin(NbDataStab-NFilter.getDepth());
	//cout << "  - NbDataStab = " << NbDataStab << endl;
	WhiteData.resize(NbDataStab, 0.0);
	NoiseData.resize(NbDataStab, 0.0);
	
    // Generation of all data for white noise
    double r1(0.0), r2(0.0);
    for(int i=0; i<StartBin; i++){
        //r1 = (double)rand()/RAND_MAX;
        //r2 = (double)rand()/RAND_MAX;

        r1 = (double)genunf(0.0, 1.0);
        r2 = (double)genunf(0.0, 1.0);
		//WhiteData[i] = tStep*sqrt(-2.0*log(r2))*cos(2*M_PI*r1);
		WhiteData[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
		//WhiteData[i] = 1.0;
    }
    
	// Generation of all noise data by filtering
	NFilter.App(StartBin, WhiteData, NoiseData);
    
	// Delete of last data in excess
    WhiteData.resize(NbData);
    NoiseData.resize(NbData);
	//cout << "  NbData = " << NbData << endl;
	//cout << "  NbBinAdd = " << NbBinAdd << endl;
	/*for(int i=0; i<NbData; i++){
		cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData[i] << endl;
	}*/
}

/*! \brief Noise generation (for one measurement), using Startbin input as beginning index.
 *
 * NbBinAdd data are inserted in #WhiteData. \n
 * #WhiteData is  generated as a standard gaussian :
 * \f[  \textrm{ for i=0,\dots,Startbin } WhiteData[i] = \sqrt{-\frac{log(r1)}{tStep} \cdot cos(2 \cdot \pi \cdot r1)} \f]
 *  where r1 and r2 are random values between 0 and 1 (using #genunf).
 * 
 * Noise data are generated using Filter::App method with  StartBin, WhiteData, and NoiseData arguments.\n
 * Then last data are deleted : #WhiteData size and #NoiseData size are set to #NbData.
 *
 */

void NoiseOof::generNoise(int StartBin)
{
    double r1(0.0), r2(0.0);
	WhiteData.insert(WhiteData.begin(), NbBinAdd, 0.0);
	/*cout << " Avant bruit :" << endl;
	for(int i=0; i<10; i++){
		cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData[i] << endl;
	}*/
	// Generation of white noise
	for(int i=StartBin; i>=0; i--){
        //r1 = (double)rand()/RAND_MAX;
        //r2 = (double)rand()/RAND_MAX;
		r1 = (double)genunf(0.0, 1.0);
        r2 = (double)genunf(0.0, 1.0);
        //WhiteData[i] = tStep*sqrt(-2.0*log(r2))*cos(2*M_PI*r1);
		WhiteData[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
	}
	/*cout << " Avant filtrage :" << endl;
	for(int i=0; i<10; i++){
		cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData[i] << endl;
	}*/
    // Generation of noise data by filtering
	NFilter.App(StartBin, WhiteData, NoiseData);
	
	/*cout << " Apres filtrage :" << endl;
	for(int i=0; i<10; i++){
		cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData[i] << endl;
	}*/
	
    // Delete of last data
    WhiteData.resize(NbData);
    /*for(int i=0; i<NbData; i++){
		cout << "   i,noisedata = " << i << "  " << NoiseData[i] << endl;
	}
	throw;*/
}


// end of LISACODE-NoiseOof.cpp
