// $Id: LISACODE-GWPeriGate.cpp,v 1.5 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWPeriGate.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-GWPeriGate.h"

/* Constructor */

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



/* Methodes d'acces */ 

/*! \brief Sets #Freq attribute.
 *
 * Input is checked: \a Freq_n must be positive or null
 */
void GWPeriGate::setFreq(double Freq_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWPeriGate: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
}

/*! \brief Sets #Amplhp attribute.
 *
 * Input is checked: Amplhp_n must be positive or null.
 */
void GWPeriGate::setAmplhp(double Amplhp_n)
{
    if ( Amplhp_n < 0.0 )
        throw invalid_argument("GWPeriGate: The amplitude h+ cannot be a inferior 0 !");
	Amplhp = Amplhp_n;
}

/*! \brief Sets #Amplhc attribute.
 *
 * Input is checked: Amplhc_n must be positive or null.
 */
void GWPeriGate::setAmplhc(double Amplhc_n)
{
    if ( Amplhc_n < 0.0 )
        throw invalid_argument("GWPeriGate: The amplitude hx cannot be a inferior 0 !");
	Amplhc = Amplhc_n;
}


/* Autres methodes */

/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *
 * \f[ \textrm{returned value}=  \left\{ \begin{array}{ll}
* Amplhp & \textrm{ if } \frac{Freq \cdot t}{1}-ceil( \frac{Freq \cdot t}{1})<0.5 \\
* 0 & else
* \end{array} \right. \f]
 */
double GWPeriGate::hp(double t)
{
	return(fmod(Freq*t,1)<0.5 ? Amplhp : 0);
}


/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 * \f[ \textrm{returned value}=  \left\{ \begin{array}{ll}
* Amplhc & \textrm{ if } \frac{Freq \cdot t}{1}-ceil( \frac{Freq \cdot t}{1})<0.5 \\
* 0 & else
* \end{array} \right. \f]
 */
double GWPeriGate::hc(double t)
{
	return(fmod(Freq*t,1)<0.5 ? Amplhc : 0);
}


// end of LISACODE-GWPeriGate.cpp
