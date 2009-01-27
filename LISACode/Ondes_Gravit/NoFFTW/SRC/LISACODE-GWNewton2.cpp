// $Id: LISACODE-GWNewton2.cpp,v 1.6 2007/04/05 09:33:18 ruiloba Exp $
/*
 *  LISACODE-GWNewton2.cpp
 *  V 1.4.1
 *  LISACode
 *
 *
 *  Created on 16/11/06 by Antoine PETITEAU and Gerard AUGER (APC)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */


#include "LISACODE-GWNewton2.h"

// *****************
// *  Constructor  *
// *****************

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \f[ \beta=0.917311-\frac{\pi}{2}  \textrm{ rad}\f]
 * \f[ \lambda=2.97378-\pi  \textrm{ rad}\f]
 * \f[ AnglPol= 2.46531 \textrm{ rad}\f]
 * \f[ type = 1\f]
 * \f[ m_1=10^5 \cdot {M_{Sun}} \textrm{ kg} \f]
 * \f[ m_2=10^5 \cdot M_{Sun} \textrm{ kg, where } M_{Sun} = 1.9889 \cdot 10^{30} \textrm{ kg} \f]
 * \f[ rdist= 10^6 \cdot kpc_m \textrm{ m, where } kpc_m = 3.0856675807 \cdot 10^{19} \textrm{ m}\f]
 * \f[ \phi_{coal}=0  \textrm{ rad}\f]
 * \f[ \tau_{d0}=0 \f]
 * \f[ \omega_{0}=0 \f]
 * \f[ gw=1 \f]
 * \f[ inc=\frac{\pi}{2}  \textrm{ rad}\f]
 * \f[ t_{coal}=60000  \textrm{ s}\f]
  * #init()
 */
GWNewton2::GWNewton2()
: GW()
{
	// Default source : AMCVn
	
	//Beta = Theta - 0.5*math.pi
	//Lambda = Phi - math.pi
	Beta = 0.917311 - M_PI/2.0;
	Lambda = 2.97378 - M_PI;
	AnglPol	= 2.46531 ;
	
	type   =  1;
	m1     =  1.0e5*MS_SI;
	m2     =  1.0e5*MS_SI;
	rdist  =  1.0e6*kpc_m;
	phcoal =  0.0;
	taud0  =  0.0;
	omega0 =  0.0;
	gw     =  1.0;
	inc    =  M_PI/2.0;
	tcoal  =  60000.0;
	
	init();
}

// Constructor for order 1 et 2
/*! \brief Constructs an instance and initializes it with default values and inputs
 *
 * \arg #Beta= Beta_n (using GW constructor)
 * \arg #Lambda = Lambda_n (using GW constructor)
 * \arg #AnglPol = AnglPol_n (using GW constructor)
 * \arg	#type   =  ttype
 * \f[	m1     =  mm1 \cdot M_{Sun}  \textrm{ kg, where } M_{Sun} = 1.9889 \cdot 10^{30} \textrm{ kg} \f]
 * \f[	m2     =  mm2 \cdot M_{Sun}  \textrm{ kg, where } M_{Sun} = 1.9889 \cdot 10^{30} \textrm{ kg} \f]
 * \f[ rdist= \cdot kpc_m \textrm{ m, where } kpc_m = 3.0856675807 \cdot 10^{19} \textrm{ m}\f]
 * \arg	#phcoal =  pphcoal
 * \arg	#taud0  =  ttaud0
 * \arg	#omega0 =  oomega0
 * \arg	#gw     =  ggw
 * \arg	#inc    =  iinc
 * \arg	#tcoal  =  ttcoal
 * #init()
 
*/
GWNewton2::GWNewton2(double Beta_n,
					 double Lambda_n,
					 double AnglPol_n,
					 int ttype,
					 double mm1,
					 double mm2,
					 double ttcoal,
					 double iinc, 
					 double pphcoal,
					 double rrdist, 
					 double ttaud0,
					 double oomega0, 
					 double ggw)  
: GW(Beta_n, Lambda_n, AnglPol_n)
{
	type   =  ttype;
	m1     =  mm1*MS_SI;
	m2     =  mm2*MS_SI;
	rdist  =  rrdist*kpc_m;
	phcoal =  pphcoal;
	taud0  =  ttaud0;
	omega0 =  oomega0;
	gw     =  ggw;
	inc    =  iinc;
	tcoal  =  ttcoal;
	
	init();
}


/*! \brief Destructor */
GWNewton2::~GWNewton2 ()
{
	
}


// **********************
// **  Access methods  **
// **********************
void GWNewton2::setParam(int iP, double Param_n){
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
			m1 = Param_n;
			break;
		case 4:
			m2 = Param_n;
			break;
		case 5:
			tcoal = Param_n;
			break;
		case 6:
			inc = Param_n;
			break;
		case 7:
			phcoal = Param_n;
			break;
		case 8:
			rdist = Param_n;
			break;
		default:
			break;
	}
}

double GWNewton2::getParam(int iP){
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
		case 3:
			return(m1);
			break;
		case 4:
			return(m2);
			break;
		case 5:
			return(tcoal);
			break;
		case 6:
			return(inc);
			break;
		case 7:
			return(phcoal);
			break;
		case 8:
			return(rdist);
			break;
		default:
			return ( 0.0 );
			break;
	}
}


/*!  Input is checked: \f[ M1_n \ge 0 \f] */
void GWNewton2::setM1(double M1_n)
{
	if (M1_n < 0.)
		throw invalid_argument("error in GWNewton2: Masse must be positive");
	m1 = M1_n;
}

/*!  Input is checked: \f[ M2_n \ge 0 \f]  */
void GWNewton2::setM2(double M2_n)
{
	if (M2_n < 0.)
		throw invalid_argument("error in GWNewton2: Masse must be positive");
	m2 = M2_n;
}

/*! \brief Sets #tcoal attribute.
 *
 * Input is checked: \f[ tcoal_n \ge 0 \f]
 */
void GWNewton2::setTcoal(double tcoal_n)
{
	if (tcoal_n < 0.)
		throw invalid_argument("error in GWNewton2: Orbital frequency must be positive");
	tcoal = tcoal_n;	
}

/*! \brief Qets #inc attribute.
 *
 * Input is checked: \f[ 0 \le inc_n \le 1 \cdot \pi  \f]
 */
void GWNewton2::setInc(double inc_n)
{
	if ((inc_n<0.0)||(inc_n>2*M_PI))
		throw invalid_argument("GW: The angle of inclination does not lie between 0 and 2*PI !");
	inc = inc_n;
}

/*! \brief Sets #phcoal attribute.
 *
 * Input is checked: \f[ 0 \le phcoal_n \le 1 \cdot \pi  \f]
 */
void GWNewton2::setPhCoal(double phcoal_n)
{
	if ((phcoal_n<0.0)||(phcoal_n>2*M_PI))
		throw invalid_argument("GW: The initial phase does not lie between 0 and 2*PI !");
	phcoal = phcoal_n;
}

/*! \brief Sets #rdist attribute.
 *
 * Input is checked: \f[ rdist_n \ge 0 \f]
 */
void GWNewton2::setDistance(double rdist_n)
{
	if (rdist_n < 0.)
		throw invalid_argument("error in GWNewton2: distance must be positive");
	rdist = rdist_n;
}



// **********************
// **  Others methods  **
// **********************
/*!
 * \f[ m_{tot}   =  m1 + m2 \f]
 * \f[ deltam   =  m1 - m2 \f]
 * \f[	\mu     =  \frac{m_1*m_2}{m_1 + m_2} \f]
 * \f[	\nu     =   \frac{\mu}{m_{tot}} \f]
 * \f[	ci     =  cos(inc) \f]
 * \f[	si     =  sin(inc) \f]
 * \f[	c_{mass}  =  \mu^{\frac{3}{5}} \cdot m_{tot}^{\frac{2}{5}} \f]	
 * \f[	time_{encour} = -1000 \f]
 *
 * \arg If (type=2)
 * \f[	\theta = -\frac{11831}{9240} \f]
 * \f[	\lambda = \frac{3}{7} \cdot (\theta - \frac{1039}{4620}) \f] 
 * \f[	a1 = \frac{55 \cdot \nu}{96} + \frac{3715}{8064} \f]
 * \f[	a2 = - \frac{3 \cdot \pi}{4} \f]	
 * \f[	a3 = \frac{9275495}{14450688} + \frac{284875}{258048} \cdot \nu + \frac{1855}{2048}) \cdot \nu^2 \f]
 * \f[	a4 = (-\frac{38645}{172032} + \frac{65}{2048} \cdot \nu) \cdot \pi \f]	
 * \f[	a5 = \frac{831032450749357}{57682522275840} - \frac{53}{40} \cdot \pi^2- \frac{107}{56} \cdot CE_{RG} + \frac{107}{448}
 *  \textrm{ where } CE_{RG} \textrm{ is Euler constant} \f]
 * \f[	a6 = (-\frac{123292747421}{4161798144} + \frac{2255}{2048} \cdot \pi^2 +
 * \frac{385}{48} \cdot \lambda - \frac{55}{16} \cdot \theta) \cdot \nu 
 * + \frac{154565}{1835008} \cdot \nu^2 - \frac{1179625}{1769472} \cdot \nu^3 \f]	
 * \f[	a7 = (\frac{188516689}{173408256} + \frac{488825}{516096} \cdot \nu - \frac{141769}{516096} \cdot \nu^2) \cdot \pi \f]
 * \f[	b1 = \frac{C^3}{8 \cdot G \cdot mtot} \f]
 * \f[	b2 = \frac{743}{2688} + \frac{11}{32} \cdot \nu \f]
 * \f[	b3 = -\frac{3 \cdot \pi}{10} \f]	
 * \f[	b4 = \frac{1855099}{14450688} + \frac{56975}{258048} \cdot \nu + \frac{371}{2048} \cdot \nu^2 \f]	
 * \f[	a11 = \frac{2 \cdot G \cdot mtot \cdot \nu}{rdist \cdot C^2} \f]
 * \f[	a22 = \frac{G \cdot mtot}{C^3} \f]	
 * \f[	b11 = -(1 + ci^2) \f]
 * \f[	c1 = - \frac{si}{8} \cdot \frac{deltam}{mtot} \f]	
 * \f[	c2 = 5 + ci^2 \f]	
 * \f[	c3 = 9 \cdot (1 + ci^2) \f]
 * \f[	d1 = \frac{1}{6} \cdot (19 + 9 \cdot ci^2 - 2 \cdot ci^4 - \nu \cdot (19 - 11 \cdot ci^2 - 6 \cdot ci^4)) \f]	
 * \f[	d2 = \frac{4}{3} \cdot si^2 \cdot (1 + ci^2) \cdot (1 - 3 \cdot \nu) \f]	
 * \f[	e1 = \frac{si}{192} \cdot \frac{deltam}{mtot} \f]
 * \f[	e2 = (57 + 60 \cdot ci^2 - ci^4) - 2 \cdot \nu \cdot (49 - 12 \cdot ci^2 - ci^4) \f]	
 * \f[	e3 = \frac{27}{2} \cdot ((73 + 40 \cdot ci^2 - 9 \cdot ci^4)
 - 2 \cdot \nu \cdot (25 - 8 \cdot ci^2 - 9 \cdot ci^4)) \f]      
 * \f[	e4 = \frac{625}{2} \cdot (1 - 2 \cdot \nu) \cdot si^2 \cdot (1 + ci^2) \f]	
 * \f[	e5 = 2 \cdot \pi \cdot (1 + ci^2) \f]	
 * \f[	f1 = \frac{1}{120} \cdot (22 + 396 \cdot ci^2 + 145 \cdot ci^4
 - 5 \cdot ci^6 + \frac{5}{3} \cdot \nu \cdot (706 - 216 \cdot ci^2 -	
 251 \cdot ci^4 + 15 \cdot ci^6) - 5 \cdot \nu^2 \cdot (98 - 108 \cdot ci^2 + 7 \cdot ci^4 + 5 \cdot ci^6)) \f] 
 * \f[	f3 = \frac{2}{15} \cdot si^2 \cdot ((59 + 35 \cdot ci^2 - 8 \cdot ci^4) - 
 * \frac{5}{3} \cdot \nu \cdot (131 + 59 \cdot ci^2 - 24 \cdot ci^4) + 
 * 5 \cdot \nu^2 \cdot (21 - 3 \cdot ci^2 - 8 \cdot ci^4)) \f]	
 * \f[	f5 = \frac{81}{40} \cdot (1 - 5 \cdot \nu + 5 \cdot \nu^2) \cdot si^4 \cdot (1 + ci^2) \f]	
 * \f[	f6 = \frac{si}{40} \cdot \frac{deltam}{mtot} \f]	
 * \f[	f7 = 11 + 7 \cdot ci^2 + 10 \cdot (5 + ci^2) \cdot log(2) \f]	
 * \f[	f8 = 5 \cdot \pi \cdot (5 + ci^2) \f]	
 * \f[	f9 = 27 \cdot (7 - 10 \cdot log(\frac{3}{2})) \cdot (1 + ci^2) \f]
 * \f[	f10 = 135 \cdot \pi \cdot (1 + ci^2) \f]	
 * \f[	a1x = -2 \cdot ci \f]	
 * \f[	b1x = - \frac{3}{4} \cdot si \cdot ci \cdot \frac{deltam}{mtot} \f]	
 * \f[	c1x = \frac{ci}{3} \cdot ((17 - 4 \cdot ci^2) - \cdot \nu \cdot (13 - 12 \cdot ci^2)) \f]	
 * \f[	c2x = -\frac{8}{3} \cdot (1 - 3 \cdot \nu) \cdot ci \cdot si^2 \f]	
 * \f[	d1x = \frac{si \cdot ci}{96} \cdot \frac{deltam}{mtot} \f]	
 * \f[	d2x = 63 - 5 \cdot ci^2 - 2 \cdot \nu \cdot (23 - 5 \cdot ci^2) \f]	
 * \f[	d3x = -\frac{27}{2} \cdot (67 - 15 \cdot ci^2 - 2 \cdot \nu \cdot (19 - 15 \cdot ci^2)) \f]	
 * \f[	d4x = \frac{625}{2} \cdot (1 - 2 \cdot \nu) \cdot si^2 \f]	
 * \f[	d5x = -4 \cdot \pi \cdot ci \f]	
 * \f[	e1x = \frac{ci}{60} \cdot (68 + 226 \cdot ci^2 
 - 15 \cdot ci^4 + \frac{5}{3} \cdot \nu \cdot (572 - 490 \cdot ci^2 + 45 \cdot ci^4) 
 - 5 \cdot \nu^2 \cdot (56 - 70 \cdot ci^2 + 15 \cdot ci^4)) \f]	
 * \f[	e2x = \frac{4}{15} \cdot ci \cdot si^2 \cdot
 * (55 - 12 \cdot ci^2 - \frac{5}{3} \cdot \nu \cdot (119 - 36 \cdot ci^2) +  5 \cdot \nu^2 \cdot (17 - 12 \cdot ci^2)) \f]	
 * \f[	e3x = -\frac{81}{20} \cdot (1 - 5 \cdot \nu + 5 \cdot \nu^2) \cdot ci \cdot si^4 \f]	
 * \f[	e4x = -\frac{3}{20} \cdot si \cdot ci \cdot \frac{deltam}{mtot} \f]	
 * \f[	e5x = 3 + 10 \cdot log(2) \f]	
 * \f[	e6x = 5 \cdot \pi \f]	
 * \f[	e7x = -9 \cdot (7 - 10 \cdot log(\frac{3}{2})) \f]	
 * \f[	e8x = -45 \cdot \pi \f]	
 * \f[	g1 = \frac{ \cdot G \cdot mtot}{C^3} \f]
 */
void GWNewton2::init()
{
	mtot   =  m1 + m2;
	deltam =  m1 - m2;
	mu     =  m1*m2/(m1 + m2);
	nu     =  mu/mtot;
	ci     =  cos(inc);
	si     =  sin(inc);
	cmass  =  (pow(mu,3./5.))*(pow(mtot,2./5.));
	
	time_encour = -1000.0;
	//   commun(0.0); 
	
	//cout << "GWNewton2 :" << endl;
	//cout << "  type = " << type << " , m1 = " << m1 << " , m2 = " << m2 << " , tcoal = " << tcoal << " , inc = " << inc << " , phcoal = " << phcoal << " , dist = " << rdist << " , omega0 = " << omega0 << " , taud = " << taud0 << " , gw = " << gw << endl;
	
	if(type>1)
	{
		teta = -(11831./9240.);
		lambda = (3.*(teta - 1039./4620.))/7.; 
		a1 = (55.*nu)/96. + 3715./8064.;
		a2 = -((3.*M_PI)/4.); 
		a3 = 9275495./14450688. + (284875./258048.)*nu + (1855./2048.)*pow(nu,2);
		a4 = (-(38645./172032.) + (65./2048.)*nu)*M_PI; 
		a5 = 831032450749357./57682522275840. - (53./40.)*pow(M_PI,2)-
		(107./56.)*CE_RG + 107./448.;
		a6 = (-(123292747421./4161798144.) + (2255./2048.)*pow(M_PI,2) +
			  (385./48.)*lambda - (55./16.)*teta)*nu + 
		(154565./1835008.)*pow(nu,2) - (1179625./1769472.)*pow(nu,3); 
		a7 = (188516689./173408256. + (488825./516096.)*nu - (141769./516096.)*
			  pow(nu,2))*M_PI;
		b1 = pow(c_SI,3)/(8.*G_SI*mtot);
		b2 = 743./2688. + (11./32)*nu;
		b3 = -((3.*M_PI)/10.); 
		b4 = 1855099./14450688. + (56975./258048.)*nu + (371./2048.)*pow(nu,2); 
		a11 = (2.*G_SI*mtot*nu)/(rdist*pow(c_SI,2));
		a22 = (G_SI*mtot)/pow(c_SI,3); 
		b11 = -(1 + pow(ci,2)); 
		c1 = (-(si/8.))*(deltam/mtot); 
		c2 = 5. + pow(ci,2); 
		c3 = 9.*(1 + pow(ci,2));
		d1 = 1/6.*(19. + 9.*pow(ci,2) - 2*pow(ci,4) - nu*(19. - 11.*pow(ci,2) - 
														  6.*pow(ci,4))); 
		d2 = (4/3.)*pow(si,2)*(1. + pow(ci,2))*(1. - 3.*nu); 
		e1 = (si/192.)*(deltam/mtot);
		e2 = (57. + 60.*pow(ci,2) - pow(ci,4)) - 2.*nu*(49. - 12.*pow(ci,2) - 
														pow(ci,4)); 
		e3 = (27./2)*((73. + 40.*pow(ci,2) - 9.*pow(ci,4)) - 2.*nu*
					  (25. - 8.*pow(ci,2) - 9.*pow(ci,4))); 
		e4 = (625./2)*(1. - 2.*nu)*pow(si,2)*(1. + pow(ci,2)); 
		e5 = 2*M_PI*(1 + pow(ci,2)); 
		f1 = (1./120)*(22. + 396.*pow(ci,2) + 145.*pow(ci,4) - 5.*pow(ci,6) + 
					   (5/3.)*nu*(706. - 216.*pow(ci,2) - 
								  251.*pow(ci,4) + 15.*pow(ci,6)) - 5.*pow(nu,2)*(98. - 108*pow(ci,2) + 
																				  7.*pow(ci,4) + 5.*pow(ci,6))); 
		f3 = (2./15)*pow(si,2)*((59. + 35*pow(ci,2) - 8.*pow(ci,4)) - (5/3.)*nu*
								(131. + 59*pow(ci,2) - 24*pow(ci,4)) + 
								5.*pow(nu,2)*(21. - 3*pow(ci,2) - 8.*pow(ci,4))); 
		f5 = (81/40.)*(1. - 5.*nu + 5.*pow(nu,2))*pow(si,4)*(1. + pow(ci,2)); 
		f6 = (si/40.)*(deltam/mtot); 
		f7 = 11. + 7.*pow(ci,2) + 10.*(5. + pow(ci,2))*log(2.); 
		f8 = 5.*M_PI*(5. + pow(ci,2)); 
		f9 = 27.*(7. - 10.*log(3/2.))*(1. + pow(ci,2));
		f10 = 135.*M_PI*(1. + pow(ci,2)); 
		a1x = -2.*ci; 
		b1x = (-3./4)*si*ci*(deltam/mtot); 
		c1x = (ci/3.)*((17 - 4*pow(ci,2)) - nu*(13 - 12*pow(ci,2))); 
		c2x = (-(8./3.))*(1 - 3*nu)*ci*pow(si,2); 
		d1x = ((si*ci)/96.)*(deltam/mtot); 
		d2x = 63. - 5.*pow(ci,2) - 2*nu*(23 - 5*pow(ci,2)); 
		d3x = (-(27./2.))*(67 - 15*pow(ci,2) - 2*nu*(19. - 15.*pow(ci,2))); 
		d4x = (625./2.)*(1 - 2.*nu)*pow(si,2); 
		d5x = -4*M_PI*ci; 
		e1x = (ci/60.)*(68. + 226.*pow(ci,2) - 15*pow(ci,4) + (5./3.)*nu*
						(572 - 490*pow(ci,2) + 45*pow(ci,4)) - 
						5*pow(nu,2)*(56. - 70.*pow(ci,2) + 15*pow(ci,4))); 
		e2x = (4./15.)*ci*pow(si,2)*(55 - 12*pow(ci,2) - (5./3.)*nu*
									 (119 - 36*pow(ci,2)) + 
									 5*pow(nu,2)*(17 - 12*pow(ci,2))); 
		e3x = (-(81./20.))*(1 - 5*nu + 5*pow(nu,2))*ci*pow(si,4); 
		e4x = (-(3./20.))*si*ci*(deltam/mtot); 
		e5x = 3. + 10.*log(2.); 
		e6x = 5*M_PI; 
		e7x = -9*(7 - 10*log(3/2.)); 
		e8x = -45.*M_PI; 
		g1 = (G_SI*mtot)/pow(c_SI,3);
		
	}
}


/*! \brief sets #phi, #omega, #hint and #time_encour attributes depending on \a temps input time.
 *
 *  If  \a temps \f$\ne \f$ #time_encour , time dependant attributes must be updated and #time_encour is set to \a temps input :
 * \arg if type=1
 * \f[ \phi   = \phi_{coal}-\left({C^3*\frac{t_{coal} - temps}{5 \cdot G \cdot cmass}}\right)^{\frac{5}{8}} \f]
 * \f[ \omega = \frac{C^3}{8 \cdot G \cdot cmass} \cdot    *{\left({\frac{C^3\left(t_{coal} - temps\right)}{5 \cdot G \cdot cmass}}\right)}^{-\frac{3}{8}} \f]
 * \f[ hint  = -\frac{2 \cdot G \cdot mtot \cdot \nu}{C^2 \cdot rdist} \cdot {\left({\frac{G \cdot mtot \cdot \omega}{C^3}}\right)}^{\frac{2}{3}} \f]
 * \arg if type=2
 * \f[ \tau_d  = \nu \cdot C^{3} \cdot \frac{t_{coal} - temps}{5 \cdot G \cdot mtot} \f]
 * \f[ \phi   = {-1 \over \nu} \left( \tau_d^\frac{5}{8} + a1 \cdot \tau_d^\frac{3}{8} + a2 \cdot \tau_d^\frac{1}{4} + a3 \cdot \tau_d^\frac{1}{8} 
 + a4 \cdot log(\frac{\tau_d}{\tau_{d0}}) + \left( a5 \cdot log(\frac{\tau_d}{256}) + a6 \right) \cdot \tau_d^{-\frac{1}{8}} + a7 \cdot \tau_d^{-\frac{1}{4}} \right) \f]
 * \f[ \omega = b1 \cdot ( \tau_d^{-\frac{3}{8}}
 + b2 \cdot \tau_d^{-\frac{5}{8}}
 + b3 \cdot \tau_d^{-\frac{4}{8}}
 + b4 \cdot \tau_d^{-\frac{7}{8}} ) \f]
 * \f[ \psi   = \phi - \frac{2 \cdot G \cdot mtot \cdot \omega}{C^3} \cdot log(\frac{\omega}{\omega_0}) \f]
 */
void GWNewton2::commun(double temps)
{
	//cout << "temps = " << temps << "  , time_encour = " << time_encour << " ,  fabs(temps-time_encour) = " << fabs(temps-time_encour) << " ,  fabs(temps-time_encour)>PRECISION = " << (fabs(temps-time_encour)>PRECISION) << endl;
	if(fabs(temps-time_encour)>PRECISION)
    {
		if(type==1)     // Calcul pour ordre 1
		{
			phi   = phcoal-pow((pow(c_SI,3)*(tcoal - temps)/(5.*G_SI*cmass)),5./8.);
			omega = pow(c_SI,3)/(8.*G_SI*cmass)*pow((pow(c_SI,3)*(tcoal - temps)/(5.*G_SI*cmass)),-3./8.);
			hint  = (-2.*G_SI*mtot*nu)/(c_SI*c_SI*rdist)*pow((G_SI*mtot*omega/pow(c_SI,3)),2./3.);
			time_encour=temps;
		}
		else          // Calcul pour ordre 2.5
		{
			
			taud  = nu*pow(c_SI,3)*((tcoal - temps)/(5*G_SI*mtot));
			phi   = -((a7/pow(taud,0.25) + a3*pow(taud,0.125) + a2*pow(taud,0.25) + a1*pow(taud,0.375) + 
					   pow(taud,0.625) + (a6 + a5*log(taud/256.))/pow(taud,0.125) + a4*log(taud/taud0))
					  /nu);
			cout.precision(15);
			//cout << "temps = " << temps << endl;
			//cout << "taud = " << taud << endl;
			//cout << "taud0 = " << taud0 << endl;
			omega = b1*(pow(taud,(-3./8.)) + b2/pow(taud,(5./8.)) + b3/pow(taud,(4./8.)) + b4/pow(taud,(7./8.))); 
			psi   = phi - ((2.*G_SI*mtot*omega)/pow(c_SI,3))*log(omega/omega0);
			//      cout<<"  taud  :"<<taud<<"  Phi  :"<<phi<<" omega  :"<<omega<<"  psi  :"<<psi;
			time_encour=temps;
		} 
		
		
	}
}

/*! \brief Gives hp component depending on \a temps input time (s).
 *
 * #commun method is called to update time depending attributes\n
 * if temps > tcoal -100, hp=0, else :
 * \arg if type=1
 * \f[  hp = hint \cdot (1+ci^2) \cos(2 \cdot \phi) \f]
 * \arg if type=2
 * \f[  hp = a11 \cdot (a22 \cdot \omega)^\frac{2}{3} \cdot \left(b11 \cdot \cos(2 \cdot \psi) \right. \f]
 * \f[ + c1 \cdot {\left(\frac{g1 \cdot \omega}{gw}\right)}^\frac{1}{3} \cdot \left(c2 \cdot \cos(\psi) - c3 \cdot \cos(3 \cdot \psi)\right) \f]
 * \f[ + {\left(\frac{g1 \cdot \omega}{gw}\right)}^\frac{2}{3} \cdot \left(d1 \cdot \cos(2. \cdot \psi) - d2 \cdot \cos(4 \cdot \psi)\right) \f]
 * \f[  + \frac{g1 \cdot \omega}{gw} \cdot \left(e1 \left( e2 \cdot cos(\cdot \psi)- e3 \cdot cos(3 \cdot \psi) + e4 \cdot  \cos(5 \cdot \psi) \right) - e5 \cdot \cos(2 \cdot \psi)\right)  \f] 
 * \f[ + {\left(\frac{g1 \cdot \omega}{gw}\right)}^\frac{2}{3} \cdot \left( f1 \cdot \cos(2 \cdot \psi) + f3 \cdot \cos(4 \cdot \psi) - f5 \cdot \cos(6 \cdot \psi) \right. \f]
 * \f[ \left. \left. + f6 \cdot \left( f7 \sin(\psi) + f8 \cdot \cos(\psi) - f9 \cdot \sin(3 \cdot \psi) + f10 \cdot \cos(3 \cdot \psi) \right) \right) \right) \f]
 */
double GWNewton2::hp(double temps)
{
	double hp;
	
	// Calcul  hplus
	commun(temps);
	
	if(temps>tcoal-100){
		hp = 0.0;
	}else{
		if(type<2)
		{
			hp = hint*(1 + ci*ci)*cos(2.*phi);
		}  
		else
		{
			cout.precision(15);
			//cout << "taud = " << taud << endl;
			//cout << "phi = " << phi << endl;
			//cout << "omega = " << omega << endl;
			//cout << "psi = " << psi << endl;
			//cout << "hp0 = " << b11*cos(2.*psi) << endl;
			//cout << "hp12 = " << pow((g1*omega)/gw,(1./3.))*c1*(c2*cos(psi)-c3*cos(3.*psi)) << endl;
			//cout << "hp1 = " << pow((g1*omega)/gw,(2./3.))*(d1*cos(2.*psi)-d2*cos(4*psi)) << endl;
			//cout << "hp32 = " << (g1*omega)/gw*(e1*(e2*cos(psi)-e3*cos(3.*psi)+e4*cos(5.*psi))-
			//									e5*cos(2.*psi)) << endl;
			//cout << "hp2 = " << pow((g1*omega)/gw,(4./3.))*(f1*cos(2.*psi) + f3*cos(4.*psi)-
			//												f5*cos(6.*psi)+f6*(f7*sin(psi)-
			//																   f8*cos(psi)-f9*sin(3.*psi)+f10*cos(3.*psi))) << endl;
			//cout << "fact = " << a11*pow(a22*omega,(2./3.)) << endl;
			
			hp   = a11*pow(a22*omega,(2./3.))*(b11*cos(2.*psi)+
											   
											   pow((g1*omega)/gw,(1./3.))*c1*(c2*cos(psi)-c3*cos(3.*psi))+
											   
											   pow((g1*omega)/gw,(2./3.))*(d1*cos(2.*psi)-d2*cos(4*psi))+
											   
											   (g1*omega)/gw*(e1*(e2*cos(psi)-e3*cos(3.*psi)+e4*cos(5.*psi))-
															  e5*cos(2.*psi))+
											   
											   pow((g1*omega)/gw,(4./3.))*(f1*cos(2.*psi) + f3*cos(4.*psi)-
																		   f5*cos(6.*psi)+f6*(f7*sin(psi)-
																							  f8*cos(psi)-f9*sin(3.*psi)+f10*cos(3.*psi))));
			//cout << " hp = " << hp << endl;
		}
		
	}
	////cout << "   ** t = " << temps << " : hp = " << hp <<endl;
	return (hp);
}



/*! \brief Gives hc component depending on temps input time (s).
 *
 * #commun method is called to update time depending attributes\n
 * if temps > tcoal -100, hc=0, else :
 * \arg if type=1
 * \f[  hc = hint \cdot 2 \cdot ci \cdot \sin(2 \cdot \phi) \f]
 * \arg if type=2
 * \f[ hc=a11 \cdot {\left( a22 \cdot \omega \right) }^\frac{2}{3} \cdot \f]
 * \f[\left( a1x \cdot \sin(2 \cdot \psi) \right. \f]
 * \f[ + {\left( \frac{g1 \cdot \omega}{gw} \right) }^{\frac{1}{3}}    \left( b1x \cdot \left(\sin(\psi) - 3 \cdot \sin(3 \cdot \psi) \right) \right) \f]
 * \f[ + {\left(\frac{g1 \cdot \omega}{gw}\right)}^\frac{2}{3} \cdot \left( c1x \cdot \sin(2 \cdot \psi)  + c2x \cdot \sin(4 \cdot \psi) \right) \f]
 * \f[ + \frac{g1 \cdot \omega}{gw} \left( d1x \cdot \left( d2x \cdot \sin(\psi) + d3x \cdot \sin(3 \cdot \psi) + d4x \cdot \sin(5 \cdot \psi) \right) + d5x \cdot \sin(2 \cdot \psi)\right)  \f]
 * \f[ + {\left( \frac{g1 \cdot \omega}{gw} \right) }^{\frac{4}{3}} \cdot \left( e1x \sin(2 \cdot \psi) + e2x \sin(4 \cdot \psi) + e3x \sin(6 \cdot \psi) \right.  \f]
 * \f[ \left. \left. + e4x \cdot \left( \left( e5x \cdot \cos(\psi) + e6x \cdot \sin(\psi) + e7x \right) \cdots \sin(3 \cdot \psi) - e8x \sin(2 \cdot \psi) \right) \right) \right) \f]
 */
double GWNewton2::hc(double temps)
{
	double hc;
	
	// Calcul  hx
	commun(temps);
	if(temps>tcoal-100){
		hc = 0.0;
	}else{
		if(type<2)
		{
			hc = hint*(2*ci)*sin(2.*phi);
		} 
		else  
		{
			
			hc  =  a11*pow(a22*omega,(2./3.))*(a1x*sin(2.*psi)+
											   
											   pow((g1*omega)/gw,(1./3.))*(b1x*(sin(psi)- 3.*sin(3.*psi)))+
											   
											   pow((g1*omega)/gw,(2./3.))*(c1x*sin(2.*psi)+ c2x*sin(4.*psi))+
											   
											   (g1*omega)/gw*(d1x*(d2x*sin(psi)+d3x*sin(3.*psi)+d4x*sin(5.*psi))
															  +d5x*sin(2.*psi))+
											   
											   pow((g1*omega)/gw,(4./3.))*(e1x*sin(2.*psi)+e2x*sin(4.*psi)+e3x*
																		   sin(6.*psi)+e4x*((e5x*cos(psi)+e6x*sin(psi)+e7x)*cos(3.*psi)-
																							e8x*sin(3.*psi))));
			
			
			
		}
	}
	////cout << "   ** t = " << temps << " : hc = " << hc <<endl;
	return (hc);
}


void GWNewton2::DispTempVal(double t, ostream * OutDisp)
{
	
}



/*Couple GWNewton2::hbin(double t)
{
	Couple hbin;
	
	commun(t);
	
	hbin.x = hp(t);
	hbin.y = hc(t);

	return hbin;
}
*/
 
/*! \brief Returns frequency depending on \a temps input time.
 *
 * #commun method is called to update time depending attributes\n
 * If \a temps > #tcoal -100, fe=0, else :
 * \f[ fe=\frac{\omega}{\pi} \f]
 */
double GWNewton2::fe(double temps)
{
    double fe;
    
	//   Calcul de la frequence
    commun(temps);
	if(temps>tcoal-100){
		fe = 0.0;
	}else{
	
		fe=omega/M_PI;
    }
	return(fe);
}


/*! \brief Returns phase depending on \a temps input time.
*
* #commun method is called to update time depending attributes\n
* If \a temps > #tcoal -100, fe=0, else :
* \f[ fe=\frac{\omega}{\pi} \f]
*/
double GWNewton2::phase(double temps)
{
    double phase;
    
	//   Calcul de la frequence
    commun(temps);
	if(temps>tcoal-100){
		phase = 0.0;
	}else{
		
		phase=2.*psi;
    }
	return(phase);
}

// end of LISACODE-GWNewton2.cpp
