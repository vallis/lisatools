/*! \todo loadNoise and generateNoise have confuse names in relation to the activities of the methods.
*/

// $Id: LISACODE-NoiseFilter.cpp,v 1.5 2007/03/29 15:16:50 lalanne Exp $
/*
 *  LISACODE-NoiseFilter.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 22/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 22/10/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-NoiseFilter.h"


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
NoiseFilter::NoiseFilter()
{
    tStep = 0.01;
    tDurAdd = 1.0;
    tFirst = 5.0;
    tLast = -20.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    vector< vector<double> > alpha_tmp(0);
	vector< vector<double> > beta_tmp(0);
	vector<double> TmpVect(1);
	TmpVect.push_back(1.0);
	beta_tmp.push_back(TmpVect);
	NFilter.init(alpha_tmp, beta_tmp, 0);
    loadNoise();
	strcpy(NoiseType,"Filter");
}

/* Constructor */
/*!\brief Constructor.
 *
 * It sets some class attributes. The other attributes are set 
 * to default values (see NoiseFilter#NoiseFilter()). 
 * It acts like the base constructor (NoiseFilter#NoiseFilter()).
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
*/

NoiseFilter::NoiseFilter(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    vector< vector<double> > alpha_tmp(0);
	vector< vector<double> > beta_tmp(0);
	vector<double> TmpVect(1);
	TmpVect.push_back(1.0);
	beta_tmp.push_back(TmpVect);
	NFilter.init(alpha_tmp, beta_tmp, 0);
    loadNoise();
    ///cout << "  - WhiteData size = " << WhiteData.size() << endl;
    ///cout << "  - NoiseData size = " << NoiseData.size() << endl;
    ///for(int i=0; i<10; i++)
    ///	cout << "    WhiteData[i] = " << WhiteData[i] << "  -    NoiseData[i] = " << NoiseData[i] << endl;
	strcpy(NoiseType,"Filter");
}
/*! \brief Constructor.
 *
 * It sets class attributes including the noise filter. 
 * It applies the filter to a white noise to generate the final noise (see #loadNoise)).
 *  #NoiseType attribute is set to \p File.
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 * \param NFilter_n Filter for #NFilter.
*/

NoiseFilter::NoiseFilter(	double tStep_n,
							double tDurAdd_n,
							double tFirst_n,
							double tLast_n,
							Filter NFilter_n)
{
    //cout << "In Noise Filter" << endl;
	settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);


	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	NFilter.Copy(NFilter_n);
    loadNoise();
	strcpy(NoiseType,"Filter");
}
/*! \brief Constructor.
 *
 * It sets some class attributes and provides the alpha (Filter#alpha) and beta (Filter#beta) parameters of the noise filter (#NFilter). It initializes the filter using alpha and applies it to a white noise to generate the final noise (#loadNoise).
 *  #NoiseType attribute is set to \p File.   
 *  NbDataStab parameter of #NFilter  is set to 0 (see Filter#NbDataStab).
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 * \param FilterAlpha_n Vector of alpha paramaters for the #NFilter (see Filter#alpha).
 * \param FilterBeta_n Vector of beta paramaters for the #NFilter (see Filter#beta).
*/

NoiseFilter::NoiseFilter(	double tStep_n,
							double tDurAdd_n,
							double tFirst_n,
							double tLast_n,
							vector< vector<double> > FilterAlpha_n,
							vector< vector<double> > FilterBeta_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);

    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	NFilter.init(FilterAlpha_n, FilterBeta_n, 0);
    loadNoise();
	strcpy(NoiseType,"Filter");
}
/*! \brief Constructor.
 *
 * It sets some class attributes and provides the parameters of the noise filter (#NFilter). 
 * It initializes the filter using alpha, beta and FilterNbDataStab and applies it to a white
 * noise to generate the final noise (#loadNoise).
 *  #NoiseType attribute is set to \p File.  
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 * \param FilterAlpha_n Vector of alpha paramaters for the #NFilter (see Filter#alpha).
 * \param FilterBeta_n Vector of beta paramaters for the #NFilter (see Filter#beta).
 * \param FilterNbDataStab_n Paramater for the #NFilter (see Filter#NbDataStab).
*/
NoiseFilter::NoiseFilter(	double tStep_n,
							double tDurAdd_n,
							double tFirst_n,
							double tLast_n,
							vector< vector<double> > FilterAlpha_n,
							vector< vector<double> > FilterBeta_n,
							int FilterNbDataStab_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	NFilter.init(FilterAlpha_n, FilterBeta_n, FilterNbDataStab_n);
    loadNoise();
	strcpy(NoiseType,"Filter");
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
void NoiseFilter::loadNoise()
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

void NoiseFilter::generNoise(int StartBin)
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
}


// end of LISACODE-NoiseFilter.cpp
