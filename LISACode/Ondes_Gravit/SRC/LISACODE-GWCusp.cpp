// $Id: LISACODE-GWCusp.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
// template for a new GW (copied on GWMono)
/*
 *  LISACODE-GWCusp.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#include "LISACODE-GWCusp.h"

// *****************
// *  Constructor  *
// *****************


GWCusp::GWCusp() : GW()
{
	Amplitude = 10.0e-10;
	CentralTime = 1.0e-6;
	MaximumFrequency = 0.1;
	Tstep = 1.0;
	//Toffset = 0.0;
	Tobs = Tstep*pow(2.0,22.0);
	Tpad = 900.0;
	Toffset = 0.0;
	
	NParam = 6;
	
	initConst();
	AllocMem();
	init();
}


GWCusp::GWCusp(double TStep_n, double Tobs_n, double Tpad_n, double Toffset_n) : GW()
{
	Amplitude = 10.0e-10;
	CentralTime = 1.0e-6;
	MaximumFrequency = 0.1;
	Tstep = TStep_n;
	//Toffset = Toffset_n;
	Tobs = Tobs_n;
	Tpad = Tpad_n;
	Toffset = Toffset_n;
	
	NParam = 6;
	
	initConst();
	AllocMem();
	init();
}

GWCusp::GWCusp(double Beta_n, 
			   double Lambda_n, 
			   double AnglPol_n,
			   double Amplitude_n,
			   double CentralTime_n,
			   double MaximumFrequency_n,
			   double TStep_n,
			   double Tobs_n,
			   double Tpad_n,
			   double Toffset_n) :
GW(Beta_n, Lambda_n, AnglPol_n)
{
	Amplitude = Amplitude_n;
	CentralTime = CentralTime_n;
	MaximumFrequency = MaximumFrequency_n;
	Tstep = TStep_n;
	//Toffset = Toffset_n;
	Tobs = Tobs_n;
	Tpad = Tpad_n;
	Toffset = Toffset_n;
	
	NParam = 6;
	
	initConst();
	AllocMem();
	init();
}


GWCusp::~GWCusp()
{
	
}

// **********************
// **  Access methods  **
// **********************

void GWCusp::setParam(int iP, double Param_n){
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
		case 3:
			Amplitude = Param_n;
			break;
		case 4:
			CentralTime = Param_n;
			break;
		case 5:
			MaximumFrequency = Param_n;
			break;
		default:
			break;
	}
}

double GWCusp::getParam(int iP){
	switch (iP) {
		case 0:
			return (Beta);
			break;
		case 1:
			return (Lambda);
			break;
		case 2:
			return (AnglPol);
			break;
		case 3:
			return (Amplitude);
			break;
		case 4:
			return (CentralTime);
			break;
		case 5:
			return (MaximumFrequency);
			break;
		default:
			return (0.0);
			break;
	}
}



// **********************
// **  Others methods  **
// **********************
void GWCusp::initConst()
{
	Flow = 1.0e-5;
	mTPI = -2.0*M_PI;
	m43 = -4.0/3.0;
}


void GWCusp::AllocMem()
{
	NtDat = 2*(int)(floor(Tobs/Tstep+0.1));
	NfDat = NtDat/2+1;
	
	th = (double*) malloc(NtDat*sizeof(double));
	fh = (fftw_complex*) malloc(NfDat*sizeof(fftw_complex));
	
	FTRevPlan = fftw_plan_dft_c2r_1d(NtDat, fh, th, FFTW_ESTIMATE);
}


void GWCusp::init()
{
	double Ts;
	double f;
	double FTh;
	
	//cout << "Initialisation of cusp (Bet=" << Beta << ", Lam=" << Lambda << ", Psi=" << AnglPol << ", Amp=" << Amplitude << ", tC=" << CentralTime << ", MaxF=" << MaximumFrequency << ")... "; fflush(stdout);
	// ** Duration of burst with pad . Frequency step is 1/Tburst
	Tburst = NtDat*Tstep;
	// **
	T0 = Tburst/4.0;
	// ** 
	Ts = T0 + CentralTime - Toffset;
	// ** 
	Tback = T0 - Tpad;
	
	// ** Initialisation of the first frequency value at 0
	fh[0][0] = 0.0;
	fh[0][1] = 0.0;
	
	cout << "( NtDat = " << NtDat << " ,  Tburst = " << Tburst <<  " ,  CentralTime = " << CentralTime << " ,  T0 = " << T0 << " ,  Ts = " << Ts << " , Tback = " << Tback << " )" << endl;
	
	// ** Loop on frequency
	for(int i=1; i<NfDat; i++){
		f = i/Tburst; // Frequency
		FTh = Amplitude * pow(f,m43) * pow(1.0 + (Flow/f)*(Flow/f) , -4);
		// Suppression of frequency > MaximumFrequency
		if (f > MaximumFrequency)
			FTh *= exp(1.0 - f/MaximumFrequency);
		fh[i][0] = -FTh * cos(mTPI*f*Ts);
		fh[i][1] = -FTh * sin(mTPI*f*Ts);
		//printf("%d : %e %e %e\n", i , FTh, fh[i][0], fh[i][1]);
	}
	
	// ** Imaginary part of the last frequency value at 0 
	fh[NfDat-1][1] = 0.0;
	
	
	// ** Output of FFT for checking
	/*
	FILE * Outfile;
	Outfile = fopen("FTDataLC.txt","w");
	for(int i=0; i<NfDat; i++){
		f = (double)(i)/Tburst;
		fprintf(Outfile, "%e %e %e\n", f, sqrt(fh[i][0]*fh[i][0]+fh[i][1]*fh[i][1]), atan2(fh[i][1], fh[i][0]));
	}
	fclose(Outfile);
	*/
	
	
	// ** Compute inverse of Fourier transform
	fftw_execute(FTRevPlan);
	
	/*
	Outfile = fopen("tDataLC.txt","w");
	for(int i=0; i<NtDat; i++){
		fprintf(Outfile, "%d %e\n", i, th[i]);
	}
	fclose(Outfile);
	*/
	
	// ** Normalization
	for(int i=0; i<NtDat; i++){
		th[i] /= Tstep;
	}
	
	 
	//cout << " --> OK" << endl;
}


/*!
 * \f[ \textrm{returned value}= Amplhp \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hp ) \f]  
 */
double GWCusp::hp(double t)
{
	//double tcur(Tback+t);
	double tcur(T0+t-Toffset);
	if((tcur>=0.0)&&(tcur<Tburst-2.0*Tstep)){
		int i((int)(floor(tcur/Tstep)));
		//cout << t << " " << tcur << " " << th[i] << " " << th[i+1] << " " <<((th[i+1]-th[i]) * (tcur/Tstep-i) + th[i]) / (double)(NtDat) << endl;
		return( ((th[i+1]-th[i]) * (tcur/Tstep-i) + th[i]) / (double)(NtDat) );
		//return(th[i] / (double)(NtDat) );
	}else{
		return(0.0);
	}
}


/*! 
 * \f[ \textrm{returned value}= Amplhc \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hc ) \f]
 */
double GWCusp::hc(double t)
{
	return(0.0);
}


void GWCusp::DispTempVal(double t, ostream * OutDisp)
{
	
}

// end of LISACODE-GWCusp.cpp
