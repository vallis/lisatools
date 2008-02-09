// $Id:  $
/*
 *  LISACODE-NoiseFShape.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 31/08/07 by Antoine PETITEAU (APC)
 *  Last modification on 31/08/07 by Antoine PETITEAU (APC)
 *
 */

#include "LISACode-NoiseFShape.h"

/* Constructor */
/*!\brief Base constructor.
*
* It sets default values for class attributes.  (#loadNoise).
* \arg tStep = 0.5
* \arg tDurAdd = 1.0
* \arg tFirst = 5.0
* \arg tLast = -20.0
* \arg NoiseType = Filter 
* \arg alpha coefficients of #NFilter = 0 (see Filter#alpha)
* \arg beta parameter of #NFilter = 1 (see Filter#beta)
* \arg NbDataStab parameter of #NFilter = 0 (see Filter#NbDataStab)
*/
NoiseFShape::NoiseFShape()
{
    tStep = 0.5;
    tDurAdd = 1.0;
    tFirst = 5.0;
    tLast = -20.0;
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
	
	NFp = 0;
	NFm = 0;
	
	FactFp = NULL;
	FactFm = NULL;
	FactF0 = 1.0;
	FilterFp.resize(0);
	tmpData_p.resize(0);
	FilterFm.resize(0);
	tmpData_m.resize(0);
	
    loadNoise();
	strcpy(NoiseType,"FShape");
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

NoiseFShape::NoiseFShape(	double tStep_n,
							double tDurAdd_n,
							double tFirst_n,
							double tLast_n,
							int NFp_n,
							int NFm_n,
							double * FactF_n
							)
{
	
	vector< vector<double> > alphaFp, alphaFm;
	vector< vector<double> > betaFp, betaFm;
	
	alphaFm.resize(1);
	betaFm.resize(1);
	alphaFm[0].resize(1);
	betaFm[0].resize(2);
	alphaFp.resize(1);
	betaFp.resize(1);
	alphaFp[0].resize(1);
	betaFp[0].resize(2);
	
	
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);
	NbData = (int)((tFirst-tLast)/tStep+100*PRECISION);
    
	NFp = NFp_n;
	NFm = NFm_n;
	
	FactFp = (double*)malloc(NFp*sizeof(double));
	FactFm = (double*)malloc(NFm*sizeof(double));
	for(int iF=0; iF<NFm; iF++)
		FactFm[iF] = FactF_n[NFm-1-iF];
	FactF0 = FactF_n[NFm];
	for(int iF=0; iF<NFp; iF++)
		FactFp[iF] = FactF_n[NFm+1+iF];
	FilterFp.resize(NFp);
	tmpData_p.resize(NFp);
	FilterFm.resize(NFm);
	tmpData_m.resize(NFm);
	
	// Display
	cout << "Filter : " ;
	for(int i=NFm-1; i>=0; i--)
		cout << " + " << FactFm[i] << " f^-" << i+1 ;
	cout << " + " << FactF0 ;
	for(int i=0; i<NFp; i++)
		cout << " + " << FactFp[i] << " f^" << i+1 ;
	cout << endl;
	
	// Filter f
	alphaFp[0][0]	= -1.0;
	betaFp[0][0]	= 1.0 / (M_PI * tStep) ;
	betaFp[0][1]	= -1.0 * betaFp[0][0];
	for(int iF=0; iF<NFp; iF++)
		FilterFp[iF].init(alphaFp, betaFp, 10);
	
	// Filter 1/f
	alphaFm[0][0]	= 1.0;
	betaFm[0][0]	= M_PI*tStep ;
	betaFm[0][1]	= 1.0 * betaFm[0][0]	;
	
	for(int iF=0; iF<NFm; iF++)
		FilterFm[iF].init(alphaFm, betaFm, 10);
	
	
	loadNoise();
    ///cout << "  - WhiteData size = " << WhiteData.size() << endl;
    ///cout << "  - NoiseData size = " << NoiseData.size() << endl;
    ///for(int i=0; i<10; i++)
    ///	cout << "    WhiteData[i] = " << WhiteData[i] << "  -    NoiseData[i] = " << NoiseData[i] << endl;
	strcpy(NoiseType,"FShape");
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
void NoiseFShape::loadNoise()
{
	int NbDataStab(2*NbData); //More bin that necessary for stabilization of the filter
    int StartBin(NbDataStab); 
	
	// Value of NbDataStab and StartBin
	for(int iF=0; iF<NFp; iF++){
		NbDataStab	+= FilterFp[iF].getNbDataStab() + FilterFp[iF].getDepth();
		StartBin	-= FilterFp[iF].getDepth();
	}
	for(int iF=0; iF<NFm; iF++){
		NbDataStab	+= FilterFm[iF].getNbDataStab() + FilterFm[iF].getDepth();
		StartBin	-= FilterFm[iF].getDepth();
	}
	
	//cout << "  - NbDataStab = " << NbDataStab << endl;
	//cout << "  - StartBin   = " << StartBin << endl;
	
	// Resize table
	WhiteData.resize(NbDataStab, 0.0);
	NoiseData.resize(NbDataStab, 0.0);
	for(int iF=0; iF<NFp; iF++)
		tmpData_p[iF].resize(NbDataStab, 0.0);
	for(int iF=0; iF<NFm; iF++)
		tmpData_m[iF].resize(NbDataStab, 0.0);
	
    // Generation of all data for white noise
    double r1(0.0), r2(0.0);
    for(int i=0; i<StartBin; i++){
        r1 = (double)genunf(0.0, 1.0);
        r2 = (double)genunf(0.0, 1.0);
		WhiteData[i] = sqrt(-1.0*log(r2)/tStep)*cos(2*M_PI*r1);
    }
    
	// Generation of all filtered data
	if(NFp >= 1)
		FilterFp[0].App(StartBin, WhiteData, tmpData_p[0]);
	if(NFm >= 1)
		FilterFm[0].App(StartBin, WhiteData, tmpData_m[0]);
	for(int iF=1; iF<NFp; iF++)
		FilterFp[iF].App(StartBin, tmpData_p[iF-1], tmpData_p[iF]);
	for(int iF=1; iF<NFm; iF++)
		FilterFm[iF].App(StartBin, tmpData_m[iF-1], tmpData_m[iF]);
	
	// Generation of all noise data by sum of filtered data
    for(int i=0; i<StartBin; i++){
		NoiseData[i] += FactF0 * WhiteData[i];
		for(int iF=0; iF<NFp; iF++)
			NoiseData[i] += FactFp[iF] * tmpData_p[iF][i];
		for(int iF=0; iF<NFm; iF++)
			NoiseData[i] += FactFm[iF] * tmpData_m[iF][i];
	}
	
	
	// Delete of last data in excess
    WhiteData.resize(NbData);
    NoiseData.resize(NbData);
	for(int i=0; i<NFp; i++)
		tmpData_p[i].resize(NbData);
	for(int i=0; i<NFm; i++)
		tmpData_m[i].resize(NbData);
	
	/*
	cout << "  NbData = " << NbData << endl;
	cout << "  NbBinAdd = " << NbBinAdd << endl;
	for(int i=0; i<NbData; i++){
		cout << "   i = " << i << " :  ";
		cout.precision(15);
		cout << WhiteData[i];
		for(int iF=0; iF<NFp; iF++)
			cout << " \t " << tmpData_p[iF][i];
		for(int iF=0; iF<NFm; iF++)
			cout << " \t " << tmpData_m[iF][i];
		cout << " \t " << NoiseData[i];
		cout << endl;
	}
	 */
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

void NoiseFShape::generNoise(int StartBin)
{
    double r1(0.0), r2(0.0);
	
	// Resize table
	WhiteData.insert(WhiteData.begin(), NbBinAdd, 0.0);
	for(int iF=0; iF<NFp; iF++)
		tmpData_p[iF].insert(tmpData_p[iF].begin(), NbBinAdd, 0.0);
	for(int iF=0; iF<NFm; iF++)
		tmpData_m[iF].insert(tmpData_m[iF].begin(), NbBinAdd, 0.0);
	
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
	
    // Generation of all filtered data
	if(NFp >= 1)
		FilterFp[0].App(StartBin, WhiteData, tmpData_p[0]);
	if(NFm >= 1)
		FilterFm[0].App(StartBin, WhiteData, tmpData_m[0]);
	for(int iF=1; iF<NFp; iF++)
		FilterFp[iF].App(StartBin, tmpData_p[iF-1], tmpData_p[iF]);
	for(int iF=1; iF<NFm; iF++)
		FilterFm[iF].App(StartBin, tmpData_m[iF-1], tmpData_m[iF]);
	
	// Generation of all noise data by sum of filtered data
    for(int i=StartBin; i>=0; i--){
		NoiseData[i] += FactF0 * WhiteData[i];
		for(int iF=0; iF<NFp; iF++)
			NoiseData[i] += FactFp[iF] * tmpData_p[iF][i];
		for(int iF=0; iF<NFm; iF++)
			NoiseData[i] += FactFm[iF] * tmpData_m[iF][i];
	}
	
	/*
	cout << " Apres filtrage :" << endl;
	for(int i=0; i<10; i++){
		cout << "   i = " << i << " :  ";
		cout.precision(15);
		cout << WhiteData[i];
		for(int iF=0; iF<NFp; iF++)
			cout << " \t " << tmpData_p[iF][i];
		for(int iF=0; iF<NFm; iF++)
			cout << " \t " << tmpData_m[iF][i];
		cout << " \t " << NoiseData[i];
		cout << endl;
	}
	 */
	 
	
    // Delete of last data
    WhiteData.resize(NbData);
	for(int i=0; i<NFp; i++)
		tmpData_p[i].resize(NbData);
	for(int i=0; i<NFm; i++)
		tmpData_m[i].resize(NbData);
}




// End of LISACODE-NoiseFShape.cpp