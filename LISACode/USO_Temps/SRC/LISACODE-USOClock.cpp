// $Id: LISACODE-USOClock.cpp,v 1.5 2007/04/05 09:33:27 ruiloba Exp $
/*
 *  USOClock.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 20/10/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 20/10/05 by A.Petiteau, T.Regimbau 
 *
 */

#include "LISACODE-USOClock.h"

/* Constructor */
/*! \brief Constructs an instance and initializes it with zero value for all attributes.
 *
 * Attributes are : 
 * \arg #Offset = 0
 * \arg #DerivLinearCoef = 0
 * \arg #SigmaNoise = 0
 * \arg #USONoise = FALSE
 */
USOClock::USOClock()
{
	init(0.0, 0.0, 0.0);
}


/*! \brief Constructs an instance and initializes it using Offset_n input.
 *
 * Attributes are :
 * \arg #Offset = Offset_n
 * \arg #DerivLinearCoef = 0
 * \arg #SigmaNoise = 0
 * \arg #USONoise = FALSE
 */
USOClock::USOClock(double Offset_n)
{
	init(Offset_n, 0.0, 0.0);
}


/*! \brief Constructs an instance and initializes it using Offset_n, DerivLinearCoef_n and SigmaNoise_n inputs.
 *
 * Attributes are :
 * \arg #Offset = Offset_n
 * \arg #DerivLinearCoef = DerivLinearCoef_n
 * \arg #SigmaNoise = SigmaNoise_n
 * \arg #USONoise = TRUE if \f$ SigmaNoise > 10^{-20} \f$, else FALSE
 */
USOClock::USOClock(double Offset_n, double DerivLinearCoef_n, double SigmaNoise_n)
{
	init(Offset_n, DerivLinearCoef_n, SigmaNoise_n);
}


/*! \brief Destructor. */
USOClock::~USOClock()
{

}


/* Access methods */
/*! \brief Sets attributes using Offset_n, DerivLinearCoef_n and SigmaNoise_n inputs.
 *
 * Set attributes are :
 * \arg Offset = Offset_n
 * \arg DerivLinearCoef = DerivLinearCoef_n
 * \arg SigmaNoise = SigmaNoise_n
 * \arg USONoise = TRUE if \f$ SigmaNoise > 10^{-20} \f$, else FALSE
 */
void USOClock::init(double Offset_n, double DerivLinearCoef_n, double SigmaNoise_n)
{
	Offset = Offset_n;
	DerivLinearCoef = DerivLinearCoef_n;
	SigmaNoise = SigmaNoise_n;
	USONoise = false;
	if(SigmaNoise > 1.0e-20)
		USONoise = true;
}



/*  Others methods */
/*! \brief Computes gap using t and tStep inputs and attributes.
 *
 * \f[ gGap = Offset + DerivLinearCoef \cdot t \f]
 * If there is noise, TimeNoise must be added
 * \f[ TimeNoise = (SigmaNoise \cdot tStep) \cdot \sqrt{(-2.0 \cdot log(r2)) \cdot cos(2 \cdot \pi \cdot r1))} \f]
 * where \f$ r1 \f$ and \f$ r2 \f$ are random values between 0 and 1.
 * \return
 * gGap
 */
double USOClock::gGap(double t, double tStep)
{
	double TimeNoise(0.0);
	if(USONoise){
		//double r1((double)rand()/RAND_MAX);
		//double r2((double)rand()/RAND_MAX);
		double r1((double)genunf(0.0, 1.0));
		double r2((double)genunf(0.0, 1.0));
		TimeNoise = (SigmaNoise*tStep)*(sqrt(-2.0*log(r2))*cos(2*M_PI*r1));
	}
	//cout << " USO : Offset = " << Offset << " , DerivLinearCoef = " << DerivLinearCoef << " , SigmaNoise = " << SigmaNoise << " , USONoise = " << USONoise << endl;
	//cout << " --> return = " << Offset + DerivLinearCoef*t + TimeNoise << endl; 
	return(Offset + DerivLinearCoef*t + TimeNoise);
}


/*  Others methods */
/*! \brief Computes time using t and tStep inputs and #gGap method.
 *
 * \return
 * \f[ t + gGap(t,tStep) \f]
 */
double USOClock::gTime(double t, double tStep)
{
	return(t + gGap(t,tStep));
}

// end of USOClock.cpp
