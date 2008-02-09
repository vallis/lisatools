// $Id: LISACODE-GW.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GW.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-GW.h"

/* Constructeur */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg	#Beta = 0
 * \arg	#Lambda = 0
 * \arg	#AnglPol = 0
 * \arg	#CalculDirProp method is called to fill #DirProp attribute
 */
GW::GW()
{
	DirProp.resize(3);
	Beta = 0.0;
	Lambda = 0.0;
	AnglPol = 0.0;
	CalculDirProp();
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * Inputs are checked:
 * \f[ \beta_n \in [-\frac{\pi}{2},-\frac{\pi}{2}] \f]
 * \f[ \lambda_n \in [0,2 \cdot \pi] \f]
 *
 * \arg	#Beta = Beta_n
 * \arg	#Lambda = Lambda_n
 * \arg	#AnglPol = 0
 * \arg	#CalculDirProp method is called to fill #DirProp attribute
 */
GW::GW(double Beta_n, double Lambda_n)
{
	if ((Beta_n < -1*M_PI/2.0)||(Beta_n > 1.0*M_PI/2.0))
		throw invalid_argument("GW: The Beta angle does not lie between - PI/2 and PI/2 !");
	if ((Lambda_n<0.0)||(Lambda_n>2*M_PI))
		throw invalid_argument("GW: The Lambda angle does not lie between 0 and 2*PI !");
	DirProp.resize(3);
	Beta = Beta_n;
	Lambda = Lambda_n;
	CalculDirProp();
	AnglPol = 0.0;
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * Inputs are checked:
 * \f[ \beta_n \in [-\frac{\pi}{2},-\frac{\pi}{2}] \f]
 * \f[ \lambda_n \in [0,2 \cdot \pi] \f]
 * \f[ AnglPol_n \in [0,2 \cdot \pi] \f]
 *
 * \arg	#Beta = Beta_n
 * \arg	#Lambda = Lambda_n
 * \arg	#AnglPol = AnglPol_n
 * \arg	#CalculDirProp method is called to fill #DirProp attribute
 */
GW::GW(double Beta_n, double Lambda_n, double AnglPol_n)
{
	if ((Beta_n < -1*M_PI/2.0)||(Beta_n > 1.0*M_PI/2.0))
		throw invalid_argument("GW: The Beta angle does not lie between - PI/2 and PI/2 !");
	if ((Lambda_n<0.0)||(Lambda_n>2*M_PI))
		throw invalid_argument("GW: The Lambda angle does not lie between 0 and 2*PI !");
	if ((AnglPol_n<0.0)||(AnglPol_n>2*M_PI))
		throw invalid_argument("GW: The angle of polarisaation does not lie between 0 and 2*PI !");
	DirProp.resize(3);
	Beta = Beta_n;
	Lambda = Lambda_n;
	CalculDirProp();
	AnglPol = AnglPol_n;
}

/*! \brief Destructor
 */
GW::~GW()
{
	
}


/* Methodes d'acces */ 
/*! \brief Sets #Beta and #DirProp attributes.
 *
 * Input is checked:
 * \f[ \beta_n \in [-\frac{\pi}{2},-\frac{\pi}{2}] \f]
 *
 * #CalculDirProp method is called to fill #DirProp attribute
 */
void GW::setBeta(double Beta_n)
{
	if ((Beta_n < -1*M_PI/2.0)||(Beta_n > -1.0*M_PI/2.0))
		throw invalid_argument("GW: The Beta angle does not lie between - PI/2 and PI/2 !");
	Beta = Beta_n;
	CalculDirProp();
}

/*! \brief Sets #Lambda and #DirProp attributes.
 *
 * Input is checked:
 * \f[ \lambda_n \in [0,2 \cdot \pi] \f]
 *
 * #CalculDirProp method is called to set #DirProp attribute
 */
void GW::setLambda(double Lambda_n)
{
	if ((Lambda_n<0.0)||(Lambda_n>2*M_PI))
		throw invalid_argument("GW: The Lambda angle does not lie between 0 and 2*PI !");
	Lambda = Lambda_n;
	CalculDirProp();
}

/*! \brief Sets #DirProp, #Lambda and #Beta attributes.
 *
 * Input is checked: \a DirProp_n must be a 3 components vector.
 *
 * Computations:
 * \f$ DirProp_{norm} \f$ is normalized DirProp_n
 * \arg	\f[ \beta = asin(DirProp_{norm}[2]) \f]
 * \arg	\f[ \lambda = mod(atan(\frac{-DirProp_{norm}[1]}{-DirProp_{norm}[0]}),2 \cdot \pi) \f]
 * 
 */
void GW::setDirProp(vector<double> DirProp_n)
{
	double a, sa;
	if (DirProp_n.size() != 3)
		throw invalid_argument("GW:The provided vector does not have 3 components!");
	a = DirProp_n[0]*DirProp_n[0]+DirProp_n[1]*DirProp_n[1]+DirProp_n[2]*DirProp_n[2];
	if ((a-1.0) > PRECISION){
		sa = sqrt(a);
		DirProp_n[0] /= sa;
		DirProp_n[1] /= sa;
		DirProp_n[2] /= sa;
	}
		
	DirProp = DirProp_n;
	Beta = asin(-1*DirProp[2]);
	Lambda = atan2(-1*DirProp[1] , -1*DirProp[0]);
	if (Lambda < 0.0)
		Lambda += 2*M_PI;
	/*
	Beta = asin(-1*DirProp[2]);
	if (fabs(cos(Beta)) < PRECISION) {
		Lambda = 0.0;
	} else {
		Lambda = acos(-1*DirProp[0]/cos(Beta));
		if (-1*DirProp[1]/cos(Beta) < 0.0)
			Lambda = 2.0*M_PI - Lambda;
	}
	 */
}

/*! \brief Sets #AnglPol attribute.
 *
 * Input is checked:
 * \f[ AnglPol_n \in [0,2 \cdot \pi] \f]
 */
void GW::setAnglPol(double AnglPol_n) 
{
	if ((AnglPol_n<0.0)||(AnglPol_n>2*M_PI))
		throw invalid_argument("GW: The angle of polarisaation does not lie between 0 and 2*PI !");
	AnglPol = AnglPol_n;
}


/* Autres methodes */
/*! \brief Gives h_plus polarisation component, depending on time \a t input.
 *
 * Virtual unused method. Returned value is constant : 1.
 */
double GW::hp(double t)
{
	return(1.0);
}


/*! \brief Gives h_cross polarisation component, depending on time \a t input.
 *
 * Virtual unused method. Returned value is constant : 0.
 */
double GW::hc(double t)
{
	return(0.0);
}

/*! \brief Computes #DirProp attribute components, depending on #Lambda and #Beta attributes.
 *
 * \f[ DirProp=\left( \begin{array}{l}
 * -cos(\lambda) \cdot cos(\beta) \\
 * -sin(\lambda) \cdot cos(\beta) \\
 * -sin(\beta)  \end{array} \right) \f]
 */
void GW::CalculDirProp() 
{
	DirProp[0] = -1*cos(Lambda)*cos(Beta);
	DirProp[1] = -1*sin(Lambda)*cos(Beta);
	DirProp[2] = -1*sin(Beta);
}



// end of LISACODE-GW.cpp
