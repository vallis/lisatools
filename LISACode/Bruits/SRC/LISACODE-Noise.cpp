/*! \todo Define variables for the default Noise values in Noise.h and use them
* in Noise and its derivated classes.
* \todo Create a function to compute NbData from tFirst, tLast and tStep and use it
* in Noise constructors and its derivated classes.
*/

// $Id: LISACODE-Noise.cpp,v 1.8 2007/04/20 12:57:43 ruiloba Exp $
/*
 *  LISACODE-Noise.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *
 *  Created on 13/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-Noise.h"


/* Constructor */
/*! \brief Base constructor.
 *
 * It sets default values for class attributes and initializes noise vector.
 * \arg tStep = 0.01
 * \arg tDurAdd = 1.0
 * \arg tFirst = 5.0
 * \arg tLast = -20.0
 * \arg NoiseType = Generic  
*/

Noise::Noise()
{
    tStep = 0.01;
    tDurAdd = 1.0;  
    tFirst = 5.0;    
    tLast = -20.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
   NbData = (int)((tFirst-tLast)/tStep+100.0*PRECISION);
	loadNoise();
	strcpy(NoiseType,"Generic");

}  
/*! \brief Constructor.
 *
 * It sets #tStep and #tDurAdd attributes. The other attributes are set to default values and noise vector is initialized such it is done by the base constructor (Noise#Noise()).\n
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 */
Noise::Noise(double tStep_n, double tDurAdd_n)
{
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    tFirst = 5.0;
    tLast = -20.0;
    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
    NbData = (int)((tFirst-tLast)/tStep+100.0*PRECISION);
    loadNoise();
	strcpy(NoiseType,"Generic");
}
/*! \brief Constructor.
 *
 * It sets values for class attributes and initializes noise vector such it is done by the base constructor (Noise#Noise()).\n
 *  #NoiseType attribute is set to \p Generic.  
 * \param tStep_n Value of #tStep. 
 * \param tDurAdd_n Value of #tDurAdd.
 * \param tFirst_n Value of #tFirst.
 * \param tLast_n Value of #tLast.
 */

Noise::Noise(double tStep_n, double tDurAdd_n, double tFirst_n, double tLast_n)
{ 
    settStep(tStep_n);
    settDurAdd(tDurAdd_n);
    settFirst(tFirst_n);
    settLast(tLast_n);

    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbData = (int)((tFirst-tLast)/tStep+100.0*PRECISION);
    loadNoise();
	strcpy(NoiseType,"Generic");
    /*cout << "  NbData = " << NoiseData.size() << endl;
    for(int i=0; i<10; i++)
        cout << "  Noise " << i << " = " << NoiseData[i] << endl; 
	*/
}


/*! \brief Destructor.
 *
 * It does not any particular action.
*/
Noise::~Noise()
{
	
}

/* Access methods */ 
/*! \brief It verifies the type of the noise (#NoiseType).
 *
 * \param SubmitType String contaning the expected noise type.
 * \return It returns \p true if the Noise object has the expected type. \p false otherwise.
 */ 
bool Noise::TestType(char * SubmitType) 
{
	if(strcmp(SubmitType,NoiseType) == 0)
		return(true);
	else
		return(false);
}
/*! \brief It sets #tStep value.
 *
 *It verifies that input argument is positive, otherwise it shows an error message.
 */
void Noise::settStep(double tStep_n)
{
	if ( tStep_n < 0.0 )
		throw invalid_argument("Noise::settStep : Step in time in seconds between two stored data cannot be negative !");  
	tStep = tStep_n;
}
/*! \brief It sets #tDurAdd value and #NbBinAdd.
 *
 * It verifies that the input argument is a positif factor of #tStep, 
 * otherwise it shows an error message.
*/
void Noise::settDurAdd(double tDurAdd_n)
{

	/*! \todo Make a function to verify that a value is an integer.
	 *   and replace code here after.
	*/

	double x(fabs(tDurAdd_n/tStep));
	if((fabs(floor(x)-x) > PRECISION)&&(fabs(ceil(x)-x) > PRECISION))
		throw invalid_argument("Noise::settDurAdd : The duration of noise production must be an integer of physical time step !");
	tDurAdd = tDurAdd_n;

	/*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	NbBinAdd = (int)floor(tDurAdd/tStep+100.0*PRECISION);
	//cout << " tDurAdd = " << tDurAdd << " , tStep = " << tStep << " ==> NbBinAdd = " << NbBinAdd << endl;
}
/*! \brief It sets #tFirst.
 *
 * It verifies that the input argument is a positif factor of #tStep, 
 * otherwise it shows an error message.
 */
void Noise::settFirst(double tFirst_n)
{
	/*! \todo Make a function to verify that a value is an integer.
	 *   and replace code here after.
	*/
	double x(fabs(tFirst_n/tStep));
	if((fabs(floor(x)-x) > PRECISION)&&(fabs(ceil(x)-x) > PRECISION))
		throw invalid_argument("Noise::settFirst : The first time of noise storage must be an integer of physical time step !");
	tFirst = tFirst_n;
}
/*! \brief It sets #tLast.
 * It verifies that the input argument is a positif factor of #tStep, 
 * otherwise it shows an error message.
 */

void Noise::settLast(double tLast_n)
{
	/*! \todo Make a function to verifie that a value is an integer.
	 *   and replace code here after.
	*/
	double x(fabs(tLast_n/tStep));
	if((fabs(floor(x)-x) > PRECISION)&&(fabs(ceil(x)-x) > PRECISION))
		throw invalid_argument("Noise::settLast : The last time of noise storage must be an integer of physical time step !");
	tLast = tLast_n;
}




/* Others methods */

/*! \brief It initializes noise vector #NoiseData.
*
* It inserts a vector of #NbData zeros at the beginig of #NoiseData.
*/
void Noise::loadNoise()
{
	NoiseData.insert(NoiseData.begin(), NbData, 0.0);
}

/*! \brief Default noise generation.
*
* It sets to zero all the samples before \p StartBin.
*  \param StartBin Index before which the values will be set to 0.
*/
void Noise::generNoise(int StartBin)
{
    for(int i=StartBin; i>=0; i--)
		NoiseData[i] = 0.0;
	//cout << " Default generNoise ( " << StartBin << " ) "<< endl;
}

/*! \brief Appends null noise data corresponding to one measurement into #NoiseData attribute.
*
* #NbBinAdd (corresponding to #tDurAdd) zeros are inserted at the begining of #NoiseData noise vector.
*
*/

void Noise::addNoise()
{
    NoiseData.insert(NoiseData.begin(), NbBinAdd, 0.0);
	generNoise(NbBinAdd-1);
	NoiseData.resize(NbData);
}



/*! \brief It returns the noise value for the specified delay.
 *
 * It transforms \p tDelay value into an index value in #NoiseData.\n 
 * The noise is extracted directly from noise vector (#NoiseData) if the specified delay corresponds to a bin. 
 * Otherwise, the noise value is interpolated by a Lagrange 7th-order interpolation.\n
 * \param tDelay Delay between noise computation time and current time in seconds (must be negative). \n
 *
 * \return If \p tDelay is out of the time range or the values needed for the interpolation are not present,
 * an error message is shown.
*/
double Noise::getNoise(double tDelay) const
{
    /*!WARNING : tDelay must be inferior than #tFirst*/
    //WARNING : tDelay must be inferior than tFirst

    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	double xbin((tFirst-tDelay)/tStep);
	int bin((int)floor(xbin+100.0*PRECISION));
	
	//cout << endl;
	//cout << "  tDelay = " << tDelay << endl;
	//cout << "  tFirst-tDelay = " << tFirst-tDelay << endl;
	//cout << "  xbin = " << xbin << endl;
	//cout << "  bin = " << bin << endl;
	//cout << "  fabs((int)(xbin+0.5)-xbin) = " << fabs((int)(xbin+0.5)-xbin) << endl;
	//cout << "  (fabs((int)(xbin+0.5)-xbin) < PRECISION) = " << (fabs((int)(xbin+0.5)-xbin) < PRECISION) << endl;
	//if(fmod((tFirst-tDelay)/tStep,1.0) < PRECISION){

        /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	if(fabs((int)(xbin+0.5)-xbin) < PRECISION){
		if((bin<0)||(bin > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		//cout << "  --> resE = " << NoiseData[bin] << endl;
		return(NoiseData[bin]);
	}else{
		/*!\todo Replace code by call to InterLagrange or its optimised function*/
		//Lagrange interpolation
		int order(7);
		double res(0.0), Pk(0.0);
		int ordermin((int)(floor(double(order+1)/2.0)));
		if((bin<0)||(bin+1 > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");   
		int kmin(bin-ordermin+1), kmax(bin+(order+1-ordermin));
		//cout << "  int(NoiseData.size())-1 = " << int(NoiseData.size())-1 << endl;
		//cout << "  kmin = " << kmin << " , kmax = " << kmax << endl;
		if(kmin < 0)
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		if(kmax > int(NoiseData.size())-1)
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		for(int k=kmin; k<=kmax; k++){
			Pk = 1.0;
			for(int j=kmin; j<=kmax; j++){
				if(j!=k)
					Pk *= (tDelay-tFirst+j*tStep)/((j-k)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
			}
			res += NoiseData[k]*Pk;
		}
		//cout << "  --> resI = " << res << endl;
		return(res);
		
		
		/*		
			//Linear interpolation
			if((bin<0)||(bin+1 > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		///cout << " Interp : bin " << bin << " = " << NoiseData[bin] << " ac " << (double(bin+1)+(tDelay-tFirst)/tStep);
		///cout << "  , bin " << bin+1 << " = " << NoiseData[bin+1] << " ac " << ((tFirst-tDelay)/tStep-double(bin)) << endl;
		///cout << " Interp : Res = " <<(double(bin+1)+(tDelay-tFirst)/tStep)*NoiseData[bin] + ((tFirst-tDelay)/tStep-double(bin))*NoiseData[bin+1] << endl;
		return((double(bin+1)+(tDelay-tFirst)/tStep)*NoiseData[bin] + ((tFirst-tDelay)/tStep-double(bin))*NoiseData[bin+1]);
		*/
	}
}

/*! \brief It returns the noise value for the specified delay.
 *
 * It transforms \p tDelay value into an index value in #NoiseData.\n 
 * The noise is extracted directly from noise vector (#NoiseData) if the specified delay corresponds to a bin. 
 * Otherwise, the noise value is interpolated by a Lagrange 7th-order interpolation.\n
 * \param tDelay Delay between noise computation time and current time in seconds (must be negative). \n
 *
 * \return If \p tDelay is out of the time range or the values needed for the interpolation are not present,
 * an error message is shown.
*/
double Noise::getNoise(double tDelay, int order) const
{
    /*!WARNING : tDelay must be inferior than #tFirst*/
    //WARNING : tDelay must be inferior than tFirst

    /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	double xbin((tFirst-tDelay)/tStep);
	int bin((int)floor(xbin+100.0*PRECISION));
	
	//cout << endl;
	//cout << "  tDelay = " << tDelay << endl;
	//cout << "  tFirst-tDelay = " << tFirst-tDelay << endl;
	//cout << "  xbin = " << xbin << endl;
	//cout << "  bin = " << bin << endl;
	//cout << "  fabs((int)(xbin+0.5)-xbin) = " << fabs((int)(xbin+0.5)-xbin) << endl;
	//cout << "  (fabs((int)(xbin+0.5)-xbin) < PRECISION) = " << (fabs((int)(xbin+0.5)-xbin) < PRECISION) << endl;
	//if(fmod((tFirst-tDelay)/tStep,1.0) < PRECISION){

        /*! \todo Make a function to compute a int from a double. Is there a reason to no call rint ?*/
	if(fabs((int)(xbin+0.5)-xbin) < PRECISION){
		if((bin<0)||(bin > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		//cout << "  --> resE = " << NoiseData[bin] << endl;
		return(NoiseData[bin]);
	}else{
		/*!\todo Replace code by call to InterLagrange or its optimised function*/
		//Lagrange interpolation
		//int order(7);
		double res(0.0), Pk(0.0);
		int ordermin((int)(floor(double(order+1)/2.0)));
		if((bin<0)||(bin+1 > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");   
		int kmin(bin-ordermin+1), kmax(bin+(order+1-ordermin));
		//cout << "  int(NoiseData.size())-1 = " << int(NoiseData.size())-1 << endl;
		//cout << "  kmin = " << kmin << " , kmax = " << kmax << endl;
		if(kmin < 0)
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		if(kmax > int(NoiseData.size())-1)
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		for(int k=kmin; k<=kmax; k++){
			Pk = 1.0;
			for(int j=kmin; j<=kmax; j++){
				if(j!=k)
					Pk *= (tDelay-tFirst+j*tStep)/((j-k)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
			}
			res += NoiseData[k]*Pk;
		}
		//cout << "  --> resI = " << res << endl;
                /*for(int i=0; i<NbData; i++){
		  cout << "   i,noisedata = " << i <<"  " << tFirst-i*tStep << " " << NoiseData[i] << endl ;
		  }
cout.precision(15);
cout << " tFirst,t_interpol,data_interpol = " << bin << "  "<< tFirst << "  " << tDelay << "  " <<tFirst+tDelay << "  " << res << endl ;
throw;*/
		return(res);
		
		
		/*		
			//Linear interpolation
			if((bin<0)||(bin+1 > int(NoiseData.size())-1))
			throw invalid_argument("Noise::getNoise : The required bin does not exist !");
		///cout << " Interp : bin " << bin << " = " << NoiseData[bin] << " ac " << (double(bin+1)+(tDelay-tFirst)/tStep);
		///cout << "  , bin " << bin+1 << " = " << NoiseData[bin+1] << " ac " << ((tFirst-tDelay)/tStep-double(bin)) << endl;
		///cout << " Interp : Res = " <<(double(bin+1)+(tDelay-tFirst)/tStep)*NoiseData[bin] + ((tFirst-tDelay)/tStep-double(bin))*NoiseData[bin+1] << endl;
		return((double(bin+1)+(tDelay-tFirst)/tStep)*NoiseData[bin] + ((tFirst-tDelay)/tStep-double(bin))*NoiseData[bin+1]);
		*/
	}
}

/*
 int Noise::unusable(double t) const
 {
	 if (t >= tMax)
		 return(0);
	 else
		 return(1);
 }
 */

// end of LISACODE-Noise.cpp
