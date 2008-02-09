// $Id: LISACODE-GWMono.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWMono.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-GWMono.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg	#Freq = 0
 * \arg	#Amplhp = 0
 * \arg	#Amplhc = 0
 * \arg	#AnglPol = 0
 * \arg	#Phi0hp = 0
 * \arg	#Phi0hc = 0
 */
GWMono::GWMono() : GW()
{
	Freq = 0.0;
	Amplhp = 0.0;
	Amplhc = 0.0;
	AnglPol = 0.0;
	Phi0hp = 0.0;
	Phi0hc = 0.0;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * Inputs are checked :\a Freq_n must be positive or null.\n
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs.
 * \arg	#Freq = Freq_n
 * \arg	#Amplhp = Amplhp_n
 * \arg	#Amplhc = Amplhc_n
 * \arg	#Phi0hp = 0
 * \arg	#Phi0hc = 0
 */
GWMono::GWMono(	double Beta_n,
				double Lambda_n,
				double AnglPol_n,
				double Freq_n,
				double Amplhp_n,
				double Amplhc_n) :
GW(Beta_n, Lambda_n, AnglPol_n)
{
	if ( Freq_n < 0.0 )
            throw invalid_argument("GWMono: The frequency cannot be a inferior 0 !");
	/*if ( Amplhp_n < 0.0 )
            throw invalid_argument("GWMono: The amplitude h+ cannot be a inferior 0 !");
	if ( Amplhc_n < 0.0 )
            throw invalid_argument("GWMono: The amplitude hx cannot be a inferior 0 !");*/
	Freq = Freq_n;
	Amplhp = Amplhp_n;
	Amplhc = Amplhc_n;
	Phi0hp = 0.0;
	Phi0hc = 0.0;
}


/*! \brief Constructs an instance and initializes it with inputs.
 *
 * Inputs are checked :\a Freq_n must be positive or null.\n
 * GW constructor is called using \a Beta_n, \a Lambda_n and \a AnglPol_n inputs.
 * \arg	#Freq = Freq_n
 * \arg	#Amplhp = Amplhp_n
 * \arg	#Amplhc = Amplhc_n
 * \arg	#Phi0hp = Phi0hp_n
 * \arg	#Phi0hc = Phi0hc_n
 */
GWMono::GWMono(	double Beta_n,
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
		throw invalid_argument("GWMono: The frequency cannot be a inferior 0 !");
	/*if ( Amplhp_n < 0.0 )
		throw invalid_argument("GWMono: The amplitude h+ cannot be a inferior 0 !");
	if ( Amplhc_n < 0.0 )
		throw invalid_argument("GWMono: The amplitude hx cannot be a inferior 0 !");*/
	Freq = Freq_n;
	Amplhp = Amplhp_n;
	Amplhc = Amplhc_n;
	Phi0hp = Phi0hp_n;
	Phi0hc = Phi0hc_n;
}



/* Methodes d'acces */ 

/*! \brief Sets #Freq attribute.
 *
 * Input is checked:\a Freq_n must be positive or null
 */
void GWMono::setFreq(double Freq_n)
{
	if ( Freq_n < 0.0 )
		throw invalid_argument("GWMono: The frequency cannot be a inferior 0 !");
	Freq = Freq_n;
}

/*! \brief Sets #Amplhp attribute.
 */
void GWMono::setAmplhp(double Amplhp_n)
{
    /*if ( Amplhp_n < 0.0 )
        throw invalid_argument("GWMono: The amplitude h+ cannot be a inferior 0 !");*/
	Amplhp = Amplhp_n;
}

/*! \brief Sets #Amplhc attribute.
 */
void GWMono::setAmplhc(double Amplhc_n)
{
    /*if ( Amplhc_n < 0.0 )
        throw invalid_argument("GWMono: The amplitude hx cannot be a inferior 0 !");*/
	Amplhc = Amplhc_n;
}



/* Autres methodes */

/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *
 * \f[ \textrm{returned value}= Amplhp \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hp ) \f]
 */
double GWMono::hp(double t)
{
	return(Amplhp*sin(2*M_PI*Freq*t + Phi0hp));
}


/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 * \f[ \textrm{returned value}= Amplhc \cdot sin (2 \cdot \pi \cdot Freq \cdot t +Phi0hc ) \f]
 */
double GWMono::hc(double t)
{
	return(Amplhc*sin(2*M_PI*Freq*t + Phi0hc));
}


// end of LISACODE-GWMono.cpp
