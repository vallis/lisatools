// $Id: LISACODE-GW.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GW.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 15/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (APC-AEI)
 *
 */

#include "LISACODE-GW.h"

// *****************
// *  Constructor  *
// *****************

/*! 
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

/*! Inputs are checked:
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

/*! Inputs are checked:
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


GW::~GW()
{
	
}

// **********************
// **  Access methods  **
// **********************

void GW::setParam(int iP, double Param_n){
	switch (iP) {
		case 0:
			Beta = Param_n;
			break;
		case 1:
			Lambda = Param_n;
			break;
		default:
			break;
	}
}

double GW::getParam(int iP){
	switch (iP) {
		case 0:
			return (Beta);
			break;
		case 1:
			return (Lambda);
			break;
		default:
			return (0.0);
			break;
	}
}


/*! Input is checked:
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

/*! Input is checked:
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

/*! Input is checked: \a DirProp_n must be a 3 components vector.
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

/*! * Input is checked:
 * \f[ AnglPol_n \in [0,2 \cdot \pi] \f]
 */
void GW::setAnglPol(double AnglPol_n) 
{
	if ((AnglPol_n<0.0)||(AnglPol_n>2*M_PI))
		throw invalid_argument("GW: The angle of polarisaation does not lie between 0 and 2*PI !");
	AnglPol = AnglPol_n;
}



// **********************
// **  Others methods  **
// **********************
/*! Nothing */
void GW::init()
{
	
}


/*! Virtual unused method. Returned value is constant : 1. */
double GW::hp(double t)
{
	return(1.0);
}


/*! Virtual unused method. Returned value is constant : 0. */
double GW::hc(double t)
{
	return(0.0);
}

/*! \f[ DirProp=\left( \begin{array}{l}
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


void GW::DispTempVal(double t, ostream * OutDisp)
{
	
}

// end of LISACODE-GW.cpp
