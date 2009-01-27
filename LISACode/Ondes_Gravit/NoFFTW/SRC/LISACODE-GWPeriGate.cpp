// $Id: LISACODE-GWPeriGate.cpp,v 1.5 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWPeriGate.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#include "LISACODE-GWPeriGate.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg	#Freq = 0
 * \arg	#Amplhp = 0
 * \arg	#Amplhc = 0
 * \arg	#AnglPol = 0
 */
GWPeriGate::GWPeriGate() : GW()
{
	Freq = 0.0;
	Amplhp = 0.0;
	Amplhc = 0.0;
	AnglPol = 0.0;
}

/*! \brief Constructs an instance and initializes it with default values and inputs
 *
 * Inputs are checked :\a Freq_n, \a Amplhp_n and \a Amplhc_n must be positive or null.
 *
 * \arg #Beta= Beta_n (using GW constructor)
 * \arg #Lambda = Lambda_n (using GW constructor)
 * \arg #AnglPol = AnglPol_n (using GW constructor)
 * \arg	#Freq = Freq_n
 * \arg	#Amplhp = Amplhp_n
 * \arg	#Amplhc = Amplhc_n
 */
GWPeriGate::GWPeriGate(double Beta_n, double Lambda_n, double AnglPol_n, double Freq_n, double Amplhp_n, double Amplhc_n) :
GW(Beta_n, Lambda_n, AnglPol_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWPeriGate: The frequency cannot be a inferior 0 !");
	if ( Amplhp_n < 0.0 )
		throw invalid_argument("GWPeriGate: The amplitude h+ cannot be a inferior 0 !");
	if ( Amplhc_n < 0.0 )
		throw invalid_argument("GWPeriGate: The amplitude hx cannot be a inferior 0 !");
	Freq = Freq_n;
	Amplhp = Amplhp_n;
	Amplhc = Amplhc_n;
}

/*! \brief Destructor */
GWPeriGate::~GWPeriGate ()
{
	
}

// **********************
// **  Access methods  **
// **********************
void GWPeriGate::setParam(int iP, double Param_n){
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
		default:
			break;
	}
}

double GWPeriGate::getParam(int iP){
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
		default:
			return (0.0);
			break;
	}
}


/*!Input is checked: \a Freq_n must be positive or null  */
void GWPeriGate::setFreq(double Freq_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWPeriGate: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
}

void GWPeriGate::setAmplhp(double Amplhp_n)
{
	Amplhp = Amplhp_n;
}

void GWPeriGate::setAmplhc(double Amplhc_n)
{
	Amplhc = Amplhc_n;
}


// **********************
// **  Others methods  **
// **********************
/*! Nothing ! */
void GWPeriGate::init()
{
	
}



/*! \f[ \textrm{returned value}=  \left\{ \begin{array}{ll}
 * Amplhp & \textrm{ if } \frac{Freq \cdot t}{1}-ceil( \frac{Freq \cdot t}{1})<0.5 \\
 * 0 & else
 * \end{array} \right. \f]
 */
double GWPeriGate::hp(double t)
{
	return(fmod(Freq*t,1)<0.5 ? Amplhp : 0);
}


/*!  \f[ \textrm{returned value}=  \left\{ \begin{array}{ll}
 * Amplhc & \textrm{ if } \frac{Freq \cdot t}{1}-ceil( \frac{Freq \cdot t}{1})<0.5 \\
 * 0 & else
 * \end{array} \right. \f]
 */
double GWPeriGate::hc(double t)
{
	return(fmod(Freq*t,1)<0.5 ? Amplhc : 0);
}



void GWPeriGate::DispTempVal(double t, ostream * OutDisp)
{
	
}

// end of LISACODE-GWPeriGate.cpp
