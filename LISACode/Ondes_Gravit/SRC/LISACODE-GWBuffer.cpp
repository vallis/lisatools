// $Id: LISACODE-GWFile.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWFile.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 27/05/09 by Antoine PETITEAU (AEI)
 *  Last modification on 27/05/09 by Antoine PETITEAU (AEI)
 *
 */

#include "LISACODE-GWBuffer.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * Inherited attributes from GW are set to default values. 
 * \arg	#LastUsedBin = 0
 */
GWBuffer::GWBuffer() : 
GW()
{
	SrcGW = NULL;
	hps = NULL;
	hcs = NULL;
	N = 0;
	t0 = 0.0;
	dt = 1.0;
	tExtra = 900.0;
	
	InterpType = LAG;
	InterpVal = 4;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs as arguments
 * \arg	#LastUsedBin = 0
 * #TimeList and #hList attributes are filled using #ReadASCIIFile method with \a FileName input as argument
 */
GWBuffer::GWBuffer(GW * SrcGW_n, double t0_n, double dt_n, double tend, double tExtra_n, INTERP InterpType_n, int InterpVal_n) : 
GW(SrcGW_n->getBeta(), SrcGW_n->getLambda(), SrcGW_n->getAnglPol())
{
	t0 = t0_n;
	dt = dt_n;
	tExtra = tExtra_n;
	N = (int)(floor((tend-t0+2.0*tExtra)/dt));
	SrcGW = SrcGW_n;
	InterpType = InterpType_n;
	InterpVal = InterpVal_n;
	hps = new Serie2(t0-tExtra, dt, N);
	hcs = new Serie2(t0-tExtra, dt, N);
	hps->allocAll();
	hcs->allocAll();
	NParam = SrcGW->getNParam();
}


GWBuffer::GWBuffer(double t0_n, double dt_n, double tend, double tExtra_n, INTERP InterpType_n, int InterpVal_n) : 
GW(0.0, 0.0, 0.0)
{
	t0 = t0_n;
	dt = dt_n;
	tExtra = tExtra_n;
	N = (int)(floor((tend-t0+2.0*tExtra)/dt));
	SrcGW = NULL;
	InterpType = InterpType_n;
	InterpVal = InterpVal_n;
	hps = new Serie2(t0-tExtra, dt, N);
	hcs = new Serie2(t0-tExtra, dt, N);
	hps->allocAll();
	hcs->allocAll();
	NParam = 3;
}


/*! \brief Destructor
 */
GWBuffer::~GWBuffer()
{
	if(hps != NULL)
		free(hps);
	if(hcs != NULL)
		free(hcs);
}

// **********************
// **  Access methods  **
// **********************

void GWBuffer::setParam(int iP, double Param_n)
{
	if(SrcGW){
		SrcGW->setParam(iP, Param_n);
	}else{
		switch (iP) {
			case 0:
				Beta = Param_n;
				break;
			case 1:
				Lambda = Param_n;
				break;
			case 2:
				AnglPol = Param_n;
				break;
			default:
				break;
		}
	}
}

double GWBuffer::getParam(int iP){
	if(SrcGW)
		return(SrcGW->getParam(iP));
	switch (iP) {
		case 0:
			return(Beta);
			break;
		case 1:
			return(Lambda);
			break;
		case 2:
			return(AnglPol);
			break;
		default:
			break;
	}
}




// **********************
// **  Others methods  **
// **********************

void GWBuffer::init()
{
	double t;
	time_t tstart, tcur;
	time(&tstart);
	//cout << "GWBuffer::init() dt=" << dt << "s ..."; fflush(stdout);
	//cout << Beta << " " << Lambda << " " << AnglPol << endl;
	if(SrcGW){
		Beta = SrcGW->getBeta();
		Lambda = SrcGW->getLambda();
		AnglPol = SrcGW->getAnglPol();
	}
	//cout << Beta << " " << Lambda << " " << AnglPol << endl;
	CalculDirProp();
	if(SrcGW){
		SrcGW->init();
		for(int i=0; i<N; i++){
			t = t0 - tExtra + i*dt;
			hps->setBinValue(i, SrcGW->hp(t) );
			hcs->setBinValue(i, SrcGW->hc(t) );
			//if((i>2306894)&&(i<2306910)){
			//	cout.precision(13);
			//	cout << t << " " << SrcGW->hp(t) << " " << SrcGW->hc(t) << endl;
			//}
			
		}
		/*
		 ofstream tmpGW("tmpGW.txt");
		 for(int i=0; i<N; i++){
		 t = t0 - tExtra i*dt;
		 tmpGW.precision(13);
		 tmpGW << t << " " << hps->gData(t, InterpType, InterpVal) << " " << hcs->gData(t, InterpType, InterpVal) << endl;
		 //if((i>2306880)&&(i<2306920)){
		 //	cout.precision(13);
		 //	cout << t << " " << SrcGW->hp(t) << " " << SrcGW->hc(t) << " " << hps->gData(t, InterpType, InterpVal) << " " << hcs->gData(t, InterpType, InterpVal) << endl;
		 //}
		 }
		 tmpGW.close();
		 */
		time(&tcur);
		
		// ** Define frequency from GWs
		FreqMin = 1.0e30;
		FreqMax = 1.0e-30;
		if(SrcGW->getFreqMin() < FreqMin)
			FreqMin = SrcGW->getFreqMin();
		if((FreqMax>=0.0)&&((SrcGW->getFreqMax() > FreqMax)||(SrcGW->getFreqMax() < 0.0)))
			FreqMax = SrcGW->getFreqMax();
	}
	//cout << " (" << tcur-tstart << " s) --> OK." << endl;
}




/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *
 * returned value = result of #Interpol method with t input time and 1 as arguments
 */
double GWBuffer::hp(double t)
{	
	//return(SrcGW->hp(t));
	return(hps->gData(t, InterpType, InterpVal));
}


/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 * \return Result of #Interpol method with \a t input time and 2 as arguments.
 */
double GWBuffer::hc(double t)
{
	//return(SrcGW->hc(t));
	return(hcs->gData(t, InterpType, InterpVal));
}



void GWBuffer::DispTempVal(double t, ostream * OutDisp)
{
	
}


void GWBuffer::sethphcAll(double * hp_n, double * hc_n, double tstart, int Npts)
{
	int istart((int)((tstart-(t0-tExtra))/dt));
	int iend(istart+Npts);
	for(int i=0; i<istart; i++){
		hps->setBinValue(i,0.0);
		hcs->setBinValue(i,0.0);
	}
	for(int i=istart, j=0; i<iend; i++, j++){
		hps->setBinValue(i,hp_n[j]);
		hcs->setBinValue(i,hc_n[j]);
	}
	for(int i=iend; i<N; i++){
		hps->setBinValue(i,0.0);
		hcs->setBinValue(i,0.0);
	}
}

void GWBuffer::sethphcBin(double hp_n, double hc_n, int i)
{
	hps->setBinValue(i,hp_n);
	hcs->setBinValue(i,hc_n);
}

void GWBuffer::sethphcNull()
{
	for(int i=0; i<N; i++){
		hps->setBinValue(i,0.0);
		hcs->setBinValue(i,0.0);
	}
}



// end of LISACODE-GWBuffer.cpp
