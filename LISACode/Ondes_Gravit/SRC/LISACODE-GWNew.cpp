// template for a new GW (copied on GWMono)
/*
 *  LISACODE-GWNew.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#include "LISACODE-GWNew.h"

// *****************
// *  Constructor  *
// *****************

/*! 
 * \arg	#Freq = 0
 * \arg	#Amplhp = 0
 * \arg	#Amplhc = 0
 * \arg	#AnglPol = 0
 * \arg	#Phi0hp = 0
 * \arg	#Phi0hc = 0
 */
GWNew::GWNew() : GW()
{
	Freq = 0.0;
	Amplhp = 0.0;
	Amplhc = 0.0;
	AnglPol = 0.0;
	Phi0hp = 0.0;
	Phi0hc = 0.0;
}

/*! 
 * Inputs are checked :\a Freq_n must be positive or null.\n
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs.
 * \arg	#Freq = Freq_n
 * \arg	#Amplhp = Amplhp_n
 * \arg	#Amplhc = Amplhc_n
 * \arg	#Phi0hp = 0
 * \arg	#Phi0hc = 0
 */
GWNew::GWNew(	double Beta_n,
				double Lambda_n,
				double AnglPol_n,
				double Freq_n,
				double Amplhp_n,
				double Amplhc_n) :
GW(Beta_n, Lambda_n, AnglPol_n)
{
	if ( Freq_n < 0.0 )
            throw invalid_argument("GWNew: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
	Amplhp = Amplhp_n;
	Amplhc = Amplhc_n;
	Phi0hp = 0.0;
	Phi0hc = 0.0;
}


/*! 
 * Inputs are checked :\a Freq_n must be positive or null.\n
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs.
 * \arg	#Freq = Freq_n
 * \arg	#Amplhp = Amplhp_n
 * \arg	#Amplhc = Amplhc_n
 * \arg	#Phi0hp = Phi0hp_n
 * \arg	#Phi0hc = Phi0hc_n
 */
GWNew::GWNew(	double Beta_n,
				double Lambda_n,
				double AnglPol_n,
				double Freq_n,
				double Amplhp_n,
				double Amplhc_n,
				double Phi0hp_n,
				double Phi0hc_n) :
GW(Beta_n, Lambda_n, AnglPol_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWNew: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
	Amplhp = Amplhp_n;
	Amplhc = Amplhc_n;
	Phi0hp = Phi0hp_n;
	Phi0hc = Phi0hc_n;
}

GWNew::~GWNew()
{
	
}

// **********************
// **  Access methods  **
// ********************** 

void GWNew::setParam(int iP, double Param_n){
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
			Freq = Param_n;
			break;
		case 4:
			Amplhp = Param_n;
			break;
		case 5:
			Amplhc = Param_n;
			break;
		case 6:
			Phi0hp = Param_n;
			break;
		case 7:
			Phi0hc = Param_n;
			break;
		default:
			break;
	}
}

double GWNew::getParam(int iP){
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
			return (Freq);
			break;
		case 4:
			return (Amplhp);
			break;
		case 5:
			return (Amplhc);
			break;
		case 6:
			return (Phi0hp);
			break;
		case 7:
			return (Phi0hc);
			break;
		default:
			return (0.0);
			break;
	}
}


/*!  Input is checked:\a Freq_n must be positive or null  */
void GWNew::setFreq(double Freq_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWNew: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
}

void GWNew::setAmplhp(double Amplhp_n)
{
	Amplhp = Amplhp_n;
}

void GWNew::setAmplhc(double Amplhc_n)
{
	Amplhc = Amplhc_n;
}



// **********************
// **  Others methods  **
// **********************
void GWNew::init()
{
	
}

/*!
 * \f[ \textrm{returned value}= Amplhp \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hp ) \f]
 */
double GWNew::hp(double t)
{
	return(Amplhp*sin(2*M_PI*Freq*t + Phi0hp));
}


/*! 
 * \f[ \textrm{returned value}= Amplhc \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hc ) \f]
 */
double GWNew::hc(double t)
{
	return(Amplhc*sin(2*M_PI*Freq*t + Phi0hc));
}


void GWNew::DispTempVal(double t, ostream * OutDisp)
{
	
}


// end of LISACODE-GWNew.cpp
