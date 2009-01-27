// $Id: LISACODE-GWBinary.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWBinary.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 01/06/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */


#include "LISACODE-GWBinary.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \f[ \beta=0.917311-\frac{\pi}{2}  \textrm{ rad}\f]
 * \f[ \lambda=2.97378-\pi  \textrm{ rad}\f]
 * \f[ AnglPol= 2.46531 \textrm{ rad}\f]
 * \f[ M_1=\frac{M_{Sun}}{2} \textrm{ kg} \f]
 * \f[ M_2=0.033 \cdot M_{Sun} \textrm{ kg, where } M_{Sun} = 1.9889 \cdot 10^{30} \textrm{ kg} \f]
 * \f[ f_{orb}=\frac{1}{1028.76} \textrm{ Hz}\f]
 * \f[ inc=1.53921  \textrm{ rad}\f]
 * \f[ \phi_0=0  \textrm{ rad}\f]
 * \f[ r= \frac{kpc_m}{10} \textrm{ m, where } kpc_m = 3.0856675807 \cdot 10^{19} \textrm{ m}\f]
 * \arg	#init method is called to fill polarized amplitudes attributes #Ap and #Ac.
 * \arg	#CalculDirProp method is called to fill #DirProp attribute
 */
GWBinary::GWBinary()
: GW()
{
	// Default source : AMCVn
	
	//Beta = Theta - 0.5*math.pi
	//Lambda = Phi - math.pi
	Beta = 0.917311 - M_PI/2.0;
	Lambda = 2.97378 - M_PI;
	AnglPol	= 2.46531 ;
	
	M1 = 0.5*MS_SI;
	M2 = 0.033*MS_SI;
	forb = 1./1028.76;
	inc = 1.53921;
	phi0 = 0.0;
	r = 0.1*kpc_m;
	
	NParam = 8;
	
	init();
	
	CalculDirProp();
}


/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * Inputs are checked : \a bM1, \a bM2, \a bforb and \a br must be positive or null
 *
 * \f[ \beta=\beta_n  \textrm{ rad}\f]
 * \f[ \lambda=\lambda_n  \textrm{ rad}\f]
 * \f[ AnglPol=AnglPol_n \textrm{ rad}\f]
 * \f[ M_1=bM_1 \cdot M_{Sun} \textrm{ kg} \f]
 * \f[ M_2=bM_2 \cdot M_{Sun} \textrm{ kg, where } M_{Sun} = 1.9889 \cdot 10^{30} \textrm{ kg} \f]
 * \f[ f_{orb}=bf_{orb} \textrm{ Hz}\f]
 * \f[ inc=binc  \textrm{ rad}\f]
 * \f[ \phi_0=b\phi_0  \textrm{ rad}\f]
 * \f[ r=br \cdot kpc_m \textrm{ m, where } kpc_m = 3.0856675807 \cdot 10^{19} \textrm{ m}\f]
 * \arg	#init method is called to fill polarized amplitudes attributes #Ap and #Ac.
 * \arg	#CalculDirProp method is called to fill #DirProp attribute
 */
GWBinary::GWBinary (double Beta_n,
					double Lambda_n,
					double AnglPol_n,
					double bM1,
					double bM2,
					double bforb,
					double binc,
					double bphi0,
					double br) 
: GW(Beta_n, Lambda_n, AnglPol_n)
{
	if ((bM1 < 0.)||(bM2 < 0.))
		throw invalid_argument("error in GWBinary: masses must be positive");
	if (bforb < 0.)
		
		throw invalid_argument("error in GWBinary: orbital frequency must be positive");
	if (br < 0.)
		throw invalid_argument("error in GWBinary: distance must be positive");
	
	M1 = bM1*MS_SI;
	M2 = bM2*MS_SI;
	forb = bforb;
	inc = binc;
	phi0 = bphi0;
	r = br*kpc_m;
	
	NParam = 8;
	
	init();
}


/*! \brief Destructor */
GWBinary::~GWBinary ()
{
	
}


// ********************
// *  Access methods  *
// ********************
void GWBinary::setParam(int iP, double Param_n){
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
			M1 = Param_n;
			break;
		case 4:
			M2 = Param_n;
			break;
		case 5:
			r = Param_n;
			break;
		case 6:
			phi0 = Param_n;
			break;
		case 7:
			forb = Param_n;
			break;
		case 8:
			inc = Param_n;
			break;
		default:
			break;
	}
}

double GWBinary::getParam(int iP){
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
			return (M1);
			break;
		case 4:
			return (M2);
			break;
		case 5:
			return (r);
			break;
		case 6:
			return (phi0);
			break;
		case 7:
			return (forb);
			break;
		case 8:
			return (inc);
			break;
		default:
			return ( 0.0 );
			break;
	}
}


void GWBinary::setM1(double M1_n)
{
	if (M1_n < 0.)
		throw invalid_argument("error in GWBinary: Masse must be positive");
	M1 = M1_n;
}

void GWBinary::setM2(double M2_n)
{
	if (M2_n < 0.)
		throw invalid_argument("error in GWBinary: Masse must be positive");
	M2 = M2_n;
}

void GWBinary::setForb(double forb_n)
{
	if (forb_n < 0.)
		throw invalid_argument("error in GWBinary: Orbital frequency must be positive");
	forb = forb_n;	
}

void GWBinary::setInc(double inc_n)
{
	if ((inc_n<0.0)||(inc_n>2*M_PI))
		throw invalid_argument("GW: The angle of inclination does not lie between 0 and 2*PI !");
	inc = inc_n;
}

void GWBinary::setPhi0(double phi0_n)
{
	if ((phi0_n<0.0)||(phi0_n>2*M_PI))
		throw invalid_argument("GW: The initial phase does not lie between 0 and 2*PI !");
	phi0 = phi0_n;
}

void GWBinary::setDistance(double r_n)
{
	if (r_n < 0.)
		throw invalid_argument("error in GWBinary: distance must be positive");
	r = r_n;
}



// ********************
// *  Others methods  *
// ********************

/*!  \return Gravitational wave polarized amplitudes : 
* \f[ A_p=A \cdot (1+cos^2(inc)) \f]
* \f[ A_c=2 \cdot A \cdot cos(inc) \f]

*  where binary components orbital separation is :
* \f[ R=\big(\frac{G \cdot (M_1+M_2)}{(2 \cdot \pi \cdot f_{orb})^2} \big)^{\frac{1}{3}}  \textrm{ m}\f]
*  and gravitational wave intrinsic amplitude is: 
* \f[ A=\frac{2 \cdot G^2 \cdot M_1 \cdot M_2}{C^4 \cdot r \cdot R} \f]
*/
void GWBinary::init()
{
	double M, R, A;
	
	// Mass in kg
	M=M1+M2;
	
	// Orbital separation of the binary components (meters)
	R = pow((G_SI*M/pow((2.0*M_PI*forb),2.)),(1./3.));
	
	// Intrinsic amplitude of the gravitational wave
	double cTwo(c_SI*c_SI);
	A = 2.0*G_SI*G_SI/(cTwo*cTwo)* M1*M2/(r*R);
	
	//Polarized amplitudes of the gravitational wave
	double cinc=cos(inc);
	Ap = A*(1.0+(cinc*cinc));
	Ac = -2.0*A*cinc;
	
}


/*!  \f[ \textrm{Returned value}= A_p \cdot cos (4 \cdot \pi \cdot f_{orb} \cdot t + \phi_0) \f]
 */
double GWBinary::hp(double t)
{
	double phase;
	phase = 4.0*M_PI*forb*t+phi0;
	return(Ap*cos(phase));
}


/*! \f[ \textrm{Returned value}= A_c \cdot sin (4 \cdot \pi \cdot f_{orb} \cdot t + \phi_0) \f]
 */
double GWBinary::hc(double t)
{
	double phase;
	phase = 4.0*M_PI*forb*t+phi0;
	return(Ac*sin(phase));
}


void GWBinary::DispTempVal(double t, ostream * OutDisp)
{
	
}




/*! \brief Gives (h_plus,h_cross)=(hp,hc) polarisation components as a Couple depending on \a t input time.
 * \return
* \f[ \textrm{Returned value} =  \left( \begin{array}{l}
* A_p \cdot cos(4 \cdot \pi \cdot f_{orb} \cdot t + \phi_0) \\
* A_c \cdot sin(4 \cdot \pi \cdot f_{orb} \cdot t + \phi_0)
* \end{array} \right) \f]
*/
/*Couple GWBinary::hbin(double t)
{
	double phase, cphase, sphase;
	Couple hbin;
	
	phase=4.0*M_PI*forb*t+phi0;
	cphase=cos(phase);
	sphase=sin(phase);
	
	hbin.x = Ap*cphase; //hplus
	hbin.y = Ac*sphase; //hcross
	
	return hbin;
}*/

// end of LISACODE-GWBinary.cpp
