/*! \todo loadNoise and generateNoise have confuse names in relation to the activities of the methods.
*/

// $Id:  $
/*
 *  LISACODE-NoiseTwoFilter.cpp
 *  V 1.3.1
 *  LISACode
 *
 *
 *  Created on 18/10/07 by Eric Plagnol (APC)
 *  Last modification on 21/12/07 by Antoine PETITEAU, Eric Plagnol (APC)
 *
 */

#include "LISACODE-NoiseTwoFilter.h" 


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
NoiseTwoFilter::NoiseTwoFilter()
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
	NFilter_2.init(alpha_tmp, beta_tmp, 0);
	loadNoise();
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
 * \param NFilter_n2 Filter for #NFilter.
*/
NoiseTwoFilter::NoiseTwoFilter(	double tStep_n,
				double tDurAdd_n,
				double tFirst_n,
				double tLast_n,
				Filter NFilter_n,
				Filter NFilter_n2)
{
    //cout << "In Noise Filter" << endl;
	settStep(tStep_n);
	settDurAdd(tDurAdd_n);
	settFirst(tFirst_n);
	settLast(tLast_n);
	
	
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	NFilter = NFilter_n;
	NFilter_2 = NFilter_n2; // ?????
	loadNoise();
	strcpy(NoiseType,"NoiseTwoFilter"); // ?????
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
void NoiseTwoFilter::loadNoise()
{
	int NbDataStab(2*NbData + NFilter.getNbDataStab() + NFilter.getDepth()); //More bin that necessary for stabilization of the filter
    int StartBin(NbDataStab-NFilter.getDepth());
    int order_ep=3;//=0,1,2,ou 3 (3=1+2) 
	//cout << "  - NbDataStab = " << NbDataStab << endl;
	WhiteData.resize(NbDataStab, 0.0);
	WhiteData2.resize(NbDataStab, 0.0);
	NoiseData.resize(NbDataStab, 0.0);
	NoiseData_tmp1.resize(NbDataStab, 0.0);
	NoiseData_tmp2.resize(NbDataStab, 0.0);
	
    // Generation of all data for white noise
    double r1(0.0), r2(0.0);
    for(int i=0; i<StartBin; i++){
      
      r1 = (double)genunf(0.0, 1.0);
      r2 = (double)genunf(0.0, 1.0);
      WhiteData[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
      r1 = (double)genunf(0.0, 1.0);
      r2 = (double)genunf(0.0, 1.0);
      WhiteData2[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
      //WhiteData[i] = 1.0;
    }
    
    // Generation of all noise data by filtering
    cout << endl << "loadNoide : The noise is filtered with Filter_MLDC (in LISACode_NoiseTwoFilter.cpp)" << endl ;
    NFilter.App(StartBin, WhiteData, NoiseData_tmp1);
    NFilter_2.App(StartBin, WhiteData2, NoiseData_tmp2);
    /*    for(int i=0; i<10; i++){
      cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData_tmp[i] << "     " << NoiseData[i] << endl;
      }*/
    
    if(order_ep == 1){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp1[i] ; 
      }
    }      
    if(order_ep == 2){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp2[i] ; 
      }
    }      
    if(order_ep == 3){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp1[i] + NoiseData_tmp2[i] ;
      }
    }
    // Delete of last data in excess
    WhiteData.resize(NbData);
    WhiteData2.resize(NbData);
    NoiseData_tmp1.resize(NbData);
    NoiseData_tmp2.resize(NbData);
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

void NoiseTwoFilter::generNoise(int StartBin)
{
    double r1(0.0), r2(0.0); 
    int print_ep=0 ;
    int order_ep=3;//=0,1,2,ou 3 (3=1+2) 

    WhiteData.insert(WhiteData.begin(), NbBinAdd, 0.0);
    WhiteData2.insert(WhiteData2.begin(), NbBinAdd, 0.0);
    //    cout << "GenerNoise : startbin,nbBinAdd"   << "   " << StartBin << "   " << NbBinAdd << endl;
    // Generation of white noise
    for(int i=StartBin; i>=0; i--){
      r1 = (double)genunf(0.0, 1.0);
      r2 = (double)genunf(0.0, 1.0);
      WhiteData[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
      r1 = (double)genunf(0.0, 1.0);
      r2 = (double)genunf(0.0, 1.0);
      WhiteData2[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
    }
    if(print_ep != 0){
      cout << " Avant filtrage EP, order_ep :" << order_ep  << endl;
      cout << " i, whitenoise, NoiseData, tmp1, tmp2 " << endl ;
      for(int i=0; i<10; i++){
	cout << "   i = " << i << " :   " << WhiteData[i] << "     " << NoiseData[i] << "     " << NoiseData_tmp1[i] << "     " << NoiseData_tmp2[i] << endl;
      }
    }
    // Generation of noise data by filtering

    if(order_ep == 1 || order_ep == 3){  // 1/f
      NFilter.App(StartBin, WhiteData, NoiseData_tmp1);  // generates 1/f noise
      if(print_ep != 0){
	cout << " Apres  filtrage 1/f : order_ep :" << order_ep << endl;
	for(int i=0; i<10; i++){
	  cout << "   i = " << i << " :   " <<WhiteData[i] << " :   " << NoiseData_tmp1[i] << endl;
	}
      }
    }
    if(order_ep >= 2){ // 1/f²
      NFilter_2.App(StartBin, WhiteData2, NoiseData_tmp2);  // should generate 1/f² noise
      if(print_ep != 0){
	cout << " Apres 1er filtrage 1/f², order_ep :" << order_ep  << endl;
	for(int i=0; i<10; i++){
	  cout << "   i = " << i << " :   " << WhiteData2[i] << "     " << NoiseData_tmp2[i]  << endl;
	}
      }
    }
    if(order_ep == 1){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp1[i] ; 
      }
    }      
    if(order_ep == 2){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp2[i] ; 
      }
    }      
    if(order_ep == 3){
      for(int i=StartBin; i>=0; i--){
	NoiseData[i] = NoiseData_tmp1[i] + NoiseData_tmp2[i] ;
      }
      if(print_ep != 0){
	cout << " On ajoute 1/f + 1/f², order_ep :" << order_ep  << endl;
	for(int i=0; i<10; i++){
	  cout << "   i = " << i << " :   " << NoiseData[i] << endl;
	}
      }
    }    
    // Delete of last data
    WhiteData.resize(NbData);
    WhiteData2.resize(NbData);
    //    NoiseData_tmp.resize(NbData);
    //	throw invalid_argument("NoiseTwoFilter : Eric a arrete le programe en 1 !");
}


// end of LISACODE-NoiseTwoFilter.cpp
