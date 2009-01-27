// $Id: LISACODE-GWFastSpinBBH.cpp,v 1.6 2007/04/20 12:28:41 ruiloba Exp $
/*
 *  LISACODE-GWFastSpinBBH.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 20/08/08 by  N. Cornish updated for LISACode by A.Petiteau (AEI)
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */


#include "LISACODE-GWFastSpinBBH.h"

// *****************
// *  Constructor  *
// *****************

GWFastSpinBBH::GWFastSpinBBH()
: GW()
{
	initNULL();
	
	// Default source
	
	setParam(0, 0.759892750778); // Beta
	setParam(1, 4.76236796807); // Lambda
	setParam(2, 2906387.81661); // m1
	setParam(3, 1056812.14434); // m2
	setParam(4, 19395657.1969); // tcoal
	setParam(5, 1000000.0); // distance
	setParam(6, 0.76622880068); // chi1
	setParam(7, 0.357815817167); // chi2
	setParam(8, acos(-0.158856838338)); // PolarAngleOfSpin1
	setParam(9,  acos(-0.482166499414)); // PolarAngleOfSpin2
	setParam(10, 3.21243381724); // AzimuthalAngleOfSpin1
	setParam(11, 2.5442762228); // AzimuthalAngleOfSpin2
	setParam(12, 3.1710410372); // phic
	setParam(13, 2.02230588043); // InitialPolarAngleL
	setParam(14, 4.74885787067); // InitialAzimuthalAngleL
	
	TaperApplied = 7.0;
	AmplPNorder = 0.0;
	Tobs = 62914560.0;
	Toffset = -900.0;
	TaperSteepness = 150.0;
	Rmin = 6.0;
	
	NParam = 15;
	
	initRK();
	AllocMemory();
	init();
	
	CalculDirProp();
}


GWFastSpinBBH::GWFastSpinBBH(double Toffset_n, double Tobs_n)
: GW()
{
	initNULL();
	
	// Default source
	
	setParam(0, 0.759892750778); // Beta
	setParam(1, 4.76236796807); // Lambda
	setParam(2, 2906387.81661); // m1
	setParam(3, 1056812.14434); // m2
	setParam(4, 19395657.1969); // tcoal
	setParam(5, 1000000.0); // distance
	setParam(6, 0.76622880068); // chi1
	setParam(7, 0.357815817167); // chi2
	setParam(8, acos(-0.158856838338)); // PolarAngleOfSpin1
	setParam(9,  acos(-0.482166499414)); // PolarAngleOfSpin2
	setParam(10, 3.21243381724); // AzimuthalAngleOfSpin1
	setParam(11, 2.5442762228); // AzimuthalAngleOfSpin2
	setParam(12, 3.1710410372); // phic
	setParam(13, 2.02230588043); // InitialPolarAngleL
	setParam(14, 4.74885787067); // InitialAzimuthalAngleL

	TaperApplied = 7.0;
	AmplPNorder = 0.0;
	Tobs = Tobs_n;
	Toffset = Toffset_n;
	TaperSteepness = 150.0;
	Rmin = 6.0;
	
	NParam = 15;
	
	initRK();
	AllocMemory();
	init();
	
	CalculDirProp();
}

GWFastSpinBBH::GWFastSpinBBH (double Beta_n,
					  double Lambda_n, 
					  double Mass1_n,                
					  double Mass2_n,                 
					  double CoalescenceTime_n,
					  double Distance_n,
					  double Spin1_n,
					  double Spin2_n,
					  double PolarAngleOfSpin1_n,
					  double PolarAngleOfSpin2_n,    
					  double AzimuthalAngleOfSpin1_n,
					  double AzimuthalAngleOfSpin2_n,
					  double PhaseAtCoalescence_n,
					  double InitialPolarAngleL_n,
					  double InitialAzimuthalAngleL_n,
					  double AmplPNorder_n,
					  double Toffset_n,
					  double Tobs_n,
					  double TaperApplied_n,
					  double TaperSteepness_n,
					  double Rmin_n) 
: GW(Beta_n, Lambda_n, 0.0)
{
	initNULL();
	
	setParam(2, Mass1_n); // m1
	setParam(3, Mass2_n); // m2
	setParam(4, CoalescenceTime_n); // tcoal
	setParam(5, Distance_n); // distance
	setParam(6, Spin1_n); // chi1
	setParam(7, Spin2_n); // chi2
	setParam(8, PolarAngleOfSpin1_n); // PolarAngleOfSpin1
	setParam(9, PolarAngleOfSpin2_n); // PolarAngleOfSpin2
	setParam(10, AzimuthalAngleOfSpin1_n); // AzimuthalAngleOfSpin1
	setParam(11, AzimuthalAngleOfSpin2_n); // AzimuthalAngleOfSpin2
	setParam(12, PhaseAtCoalescence_n); // phic
	setParam(13, InitialPolarAngleL_n); // InitialPolarAngleL
	setParam(14, InitialAzimuthalAngleL_n); // InitialAzimuthalAngleL
	
	
	TaperApplied = TaperApplied_n;
	setAmplPNorder(AmplPNorder_n);
	Tobs = Tobs_n;
	Toffset = Toffset_n;
	Rmin = Rmin_n;
	TaperSteepness = TaperSteepness_n;
	
	NParam = 15;
	
	initRK();
	AllocMemory();
	init();
}


/*! \brief Destructor
 */
GWFastSpinBBH::~GWFastSpinBBH ()
{
	if(timevec != NULL){
		
		free(timevec);
		free(mulvec);
		free(sphilvec);
		free(cphilvec);
		free(betavec);
		free(sigmavec);
		free(thomasvec);
		free(muly2);
		free(sphily2);
		free(cphily2);
		free(betay2);
		free(sigmay2);
		free(thomasy2);
		
		free(uspline);
		
		for(int iR = 0; iR < 9; iR++){
			free(krk[iR]);
		}
		free(krk);
	}
}


// **********************
// **  Access methods  **
// **********************

void GWFastSpinBBH::setParam(int iP, double Param_n){
	switch (iP) {
		case 0:
			Beta = Param_n;
			break;
		case 1:
			Lambda = Param_n;
			break;
		case 2:
			m1 = Param_n;
			Mtot = m1 + m2;
			dm = (m1-m2)/Mtot;
			mu = m1*m2/Mtot;
			eta = mu/Mtot;
			etain = 1.0/eta;
			Mchirp = pow(mu, 0.6)*pow(Mtot, 0.4);
			break;
		case 3:
			m2 = Param_n;
			Mtot = m1 + m2;
			dm = (m1-m2)/Mtot;
			mu = m1*m2/Mtot;
			eta = mu/Mtot;
			etain = 1.0/eta;
			Mchirp = pow(mu, 0.6)*pow(Mtot, 0.4);
			break;
		case 4:
			tc = Param_n;
			break;
		case 5:
			DL = Param_n;
			break;
		case 6:
			chi1 = Param_n;
			break;
		case 7:
			chi2 = Param_n;
			break;
		case 8:
			PolarAngleOfSpin1 = Param_n;
			break;
		case 9:
			PolarAngleOfSpin2 = Param_n;
			break;
		case 10:
			AzimuthalAngleOfSpin1 = Param_n;
			break;
		case 11:
			AzimuthalAngleOfSpin2 = Param_n;
			break;
		case 12:
			phic = Param_n;
			break;
		case 13:
			InitialPolarAngleL = Param_n;
			break;
		case 14:
			InitialAzimuthalAngleL = Param_n;
			break;
		case 15:
			Mchirp = Param_n;
			Mtot = Mchirp / pow(eta,0.6);
			mu = eta*Mtot;
			m1 = Mtot*(1.0+sqrt(1.0-4.0*eta))/2.0;
			m2 = Mtot*(1.0-sqrt(1.0-4.0*eta))/2.0;
			dm = (m1-m2)/Mtot;
			etain = 1.0/eta;
			break;
		case 16:
			mu = Param_n;
			Mtot = pow(Mchirp,2.5) / pow(mu,1.5);
			eta = mu/Mtot;
			m1 = Mtot*(1.0+sqrt(1.0-4.0*eta))/2.0;
			m2 = Mtot*(1.0-sqrt(1.0-4.0*eta))/2.0;
			dm = (m1-m2)/Mtot;
			etain = 1.0/eta;
			break;
		case 17:
			eta = Param_n;
			Mtot = Mchirp / pow(eta,0.6);
			mu = eta*Mtot;
			m1 = Mtot*(1.0+sqrt(1.0-4.0*eta))/2.0;
			m2 = Mtot*(1.0-sqrt(1.0-4.0*eta))/2.0;
			dm = (m1-m2)/Mtot;
			etain = 1.0/eta;
			break;
		default:
			break;
	}
}

double GWFastSpinBBH::getParam(int iP)
{
	switch (iP) {
		case 0:
			return (Beta);
			break;
		case 1:
			return (Lambda);
			break;
		case 2:
			return (m1);
			break;
		case 3:
			return (m2);
			break;
		case 4:
			return (tc);
			break;
		case 5:
			return (DL);
			break;
		case 6:
			return (chi1);
			break;
		case 7:
			return (chi2);
			break;
		case 8:
			return (PolarAngleOfSpin1);
			break;
		case 9:
			return (PolarAngleOfSpin2);
			break;
		case 10:
			return (AzimuthalAngleOfSpin1);
			break;
		case 11:
			return (AzimuthalAngleOfSpin2);
			break;
		case 12:
			return (phic);
			break;
		case 13:
			return (InitialPolarAngleL);
			break;
		case 14:
			return (InitialAzimuthalAngleL);
			break;
		case 15:
			return (Mchirp);
			break;
		case 16:
			return (mu);
			break;
		case 17:
			return (eta);
			break;
		default:
			return ( 0.0 );
			break;
	}
}

void GWFastSpinBBH::setMass1(double Mass1_n)
{
	if (Mass1_n < 0.)
		throw invalid_argument("error in GWFastSpinBBH::setM1 Masse must be positive");
	m1 = Mass1_n;
}


void GWFastSpinBBH::setMass2(double Mass2_n)
{
	if (Mass2_n < 0.)
		throw invalid_argument("error in GWFastSpinBBH::setM2 Masse must be positive");
	m2 = Mass2_n;
}


void GWFastSpinBBH::setDistance(double r_n)
{
	if (r_n < 0.)
		throw invalid_argument("error in GWFastSpinBBH::setDistance : Distance must be positive !");
	DL = r_n;
}



void GWFastSpinBBH::setAmplPNorder(double AmplPNorder_n)
{
	if (AmplPNorder_n < 0.)
		throw invalid_argument("error in GWFastSpinBBH::setAmplPNorder : Amplitude PN order must be positive !");
	
	if (AmplPNorder_n > 2.0)
		throw invalid_argument("error in GWFastSpinBBH::setAmplPNorder : At the moment, maximum amplitude PN order is 2.");
	AmplPNorder = AmplPNorder_n ;
}

// **********************
// **  Others methods  **
// **********************

void GWFastSpinBBH::initNULL()
{	
	krk = NULL;
	timevec = NULL;
	mulvec = NULL;
	sphilvec = NULL;
	cphilvec = NULL;
	betavec = NULL;
	sigmavec = NULL;
	thomasvec = NULL;
	muly2 = NULL;
	sphily2 = NULL;
	cphily2 = NULL;
	betay2 = NULL;
	sigmay2 = NULL;
	thomasy2 = NULL;
	uspline = NULL;
}


void GWFastSpinBBH::AllocMemory()
{
	if(timevec != NULL){
		
		free(timevec);
		free(mulvec);
		free(sphilvec);
		free(cphilvec);
		free(betavec);
		free(sigmavec);
		free(thomasvec);
		free(muly2);
		free(sphily2);
		free(cphily2);
		free(betay2);
		free(sigmay2);
		free(thomasy2);
		
		free(uspline);
		
		for(int iR = 0; iR < 9; iR++){
			free(krk[iR]);
		}
		free(krk);
		
		
	}
	
	krk = (double**) malloc(9*sizeof(double*));
	for(int iR = 0; iR < 9; iR++){
		krk[iR] = (double*) malloc(6*sizeof(double));
	}
	
	timevec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	mulvec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	sphilvec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	cphilvec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	betavec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	sigmavec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	thomasvec = (double*)malloc(RK_VECLENGTH*sizeof(double));
	
	muly2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	sphily2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	cphily2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	betay2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	sigmay2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	thomasy2 = (double*)malloc(RK_VECLENGTH*sizeof(double));
	
	uspline = (double*)malloc(RK_VECLENGTH*sizeof(double));
	
	RK_VECLENGTH_Old = RK_VECLENGTH;
}




void GWFastSpinBBH::initRK()
{
	// Cash-Karp Runge-Kutta parameters:
	A2 = (1./5.);
	A3 = (3./10.);
	A4 = (3./5.);
	A5 = 1.;
	A6 = (7./8.);
	
	B21 = (1./5.);
	B31 = (3./40.);
	B32 = (9./40.);
	B41 = (3./10.);
	B42 = (-9./10.);
	B43 = (6./5.);
	B51 = (-11./54.);
	B52 = (5./2.);
	B53 = (-70./27.);
	B54 = (35./27.);
	B61 = (1631./55296.);
	B62 = (175./512.);
	B63 = (575./13824.);
	B64 = (44275./110592.);
	B65 = (253./4096.);
	
	C1 = (37./378.);
	C2 = 0.;
	C3 = (250./621.);
	C4 = (125./594.);
	C5 = 0.;
	C6 = (512./1771.);
	
	D1 = (2825./27648.);
	D2 = 0.;
	D3 = (18575./48384.);
	D4 = (13525./55296.);
	D5 = (277./14336.);
	D6 = (1./4.);
	
	RK_H = 100000;
	RK_EPS = 0.00000001;
	
	// Length of RK storage vectors:
	RK_VECLENGTH = 20000;
}


void GWFastSpinBBH::init()
{	
	double N[3], LSvals[9], updatedvals[9], fourthordervals[9], updatedthomas;
	double derivvals[9];
	double thomasderiv;
	double LcrossN[3];
	double JcrossL[3];
	double Jtot[3];
	double thomasval, maxerr, oldh;
	double LdotS1, LdotS2, LdotN, S1dotS2;
	double costhetaL0, sinthetaL0, costhetaS10, sinthetaS10, costhetaS20, sinthetaS20;
	double phiL0, phiS10, phiS20;
	double fac;
	double beta, sigma;
	double tcurrent, tfinal, hcurrent;
	double f, r, x, fold;
	double Lmag, S1mag, S2mag;
	double Jmag, JdotL;
	int index;
	bool laststep;
	int NbVecLength(1);
	
	cout.precision(13);
	cout << "   --> Initialisation of FastBBH (compute precession for " << Tobs << " s starting at " << Toffset << " s ) ... ";
	
	
	xmax = 1.0/Rmin;
	
	costheta = sin(Beta);
	costhetaL0 = cos(InitialPolarAngleL);
	costhetaS10 = cos(PolarAngleOfSpin1);
	costhetaS20 = cos(PolarAngleOfSpin2);
	phi = Lambda;
	phiL0 = InitialAzimuthalAngleL;
	phiS10 = AzimuthalAngleOfSpin1;
	phiS20 = AzimuthalAngleOfSpin2;
	
	
	Amp = pow(Mchirp*TSUN,5./3.)/(DL*kpc_m/c_SI);
	
	fac = eta/(5.0*Mtot*TSUN); // Tau / (tc - t)   : refMLDC3[gr-qc_0806.2110]
	
	Phi0 = -2.*pow(fac,0.625);	// -2 fac^(5/8)
	Phi10 = -pow(fac,0.375)*(3715./4032.+55./48.*eta); // - (3715/4032 + 55/48 eta) fac^(3/8) 
	p15 = 0.375*pow(fac,0.25);	// (3/8) fac^(1/4)
	p150 = p15*(4.*M_PI);			// 4 pi (3/8) fac^(1/4)
	p20 = -pow(fac,0.125);		// - fac^(1/8)
	p200 = p20*(9275495./7225344.+284875./129024.*eta+1855./1024.*eta*eta); // - (9275495/7225344 + 284875/129024 eta + 1855/1024 eta^2) fac^(1/8)
	
	printf("\nm1 = %.12e   m2 = %.12e    tc = %.12e   DL =  %.12e   x1 = %.12e    x2 = %.12e \n",	m1, m2, tc, DL, chi1, chi2);
	printf("theta = %.12e   thetaL = %.12e  thS1 = %.12e  thS2  = %.12e \n", M_PI - Beta, InitialPolarAngleL, PolarAngleOfSpin1, PolarAngleOfSpin2);
	printf("phi = %.12e   phiL = %.12e  phiS1 = %.12e   phiS2 = %.12e  phic = %.12e \n", phi, phiL0, phiS10, phiS20, phic);
	
	// Sky position:
	
	sintheta = sqrt(1.0 - costheta*costheta);
	N[0] = sintheta*cos(phi);
	N[1] = sintheta*sin(phi);
	N[2] = costheta;
	
	
	// ** Set inital value of L, S1 and S2
	// LSvals: 0-2 are L, 3-5 S1, and 6-8 S2 (all unit vectors)
	
	sinthetaL0 = sqrt(1.0-(costhetaL0*costhetaL0));
	LSvals[0] = sinthetaL0*cos(phiL0);
	LSvals[1] = sinthetaL0*sin(phiL0);
	LSvals[2] = costhetaL0;
	
	sinthetaS10 =  sqrt(1.0-(costhetaS10*costhetaS10));
	LSvals[3] = sinthetaS10*cos(phiS10);
	LSvals[4] = sinthetaS10*sin(phiS10);
	LSvals[5] = costhetaS10;
	
	sinthetaS20 =  sqrt(1.0-(costhetaS20*costhetaS20));
	LSvals[6] = sinthetaS20*cos(phiS20);
	LSvals[7] = sinthetaS20*sin(phiS20);
	LSvals[8] = costhetaS20;
	
	// Compute scalar product L.S1 , L.S2 , S1.S2
	LdotS1 = calcLdotS1(LSvals);
	LdotS2 = calcLdotS2(LSvals);
	S1dotS2 = calcSdotS(LSvals);
	
	// Initial beta and sigma:
	
	beta = (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
	
	sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;
	
	// Set up range of integration.  The extra Toffset=900 s is more than enough for the light travel time across the LISA orbit.
	
	tcurrent = Toffset/TSUN;
	tmax = Tobs - Toffset;  // will decrease if xmax lies within integration range
	if(tc > tmax) 
		tmax = tc;
	tfinal = tmax/TSUN;
	
	f = Freq(0.0, Mtot, Mchirp, eta, beta, sigma, tc);
	
	// Initial orbital radius
	r = pow(Mtot,1./3.)/pow(M_PI*f*TSUN,2./3.);
	
	x = pow(M_PI*Mtot*f*TSUN,2./3.);
	
	Lmag = mu*Mtot*TSUN*TSUN/sqrt(x);
	S1mag = chi1*m1*m1*TSUN*TSUN;
	S2mag = chi2*m2*m2*TSUN*TSUN;
	
	Jtot[0] = Lmag*LSvals[0]+S1mag*LSvals[3] + S2mag*LSvals[6];
	Jtot[1] = Lmag*LSvals[1]+S1mag*LSvals[4] + S2mag*LSvals[7];
	Jtot[2] = Lmag*LSvals[2]+S1mag*LSvals[5] + S2mag*LSvals[8];
	
	Jmag = sqrt(calcLdotN(Jtot, Jtot));
	
	for(int i = 0; i < 3; i++)
		Jtot[i] /= Jmag;
	
	JdotL = calcLdotN(Jtot, LSvals);
	calcLcrossN(JcrossL, Jtot, LSvals);
	
	// printf("L %e S1 %e S2 %e J %e\n", Lmag, S1mag, S2mag, Jmag);
	
	calcderivvals(derivvals, LSvals, r, m1, m2, Mtot, mu, chi1, chi2);
	LdotN = calcLdotN(LSvals, N);
	calcLcrossN(LcrossN, LSvals, N);
	thomasderiv = -2.*LdotN/(1.-LdotN*LdotN)*(LcrossN[0]*derivvals[0]+LcrossN[1]*derivvals[1]+LcrossN[2]*derivvals[2]);
	
	x = calcLdotN(JcrossL, LcrossN);
	
	// printf("Nom %e \n", x); 
	
	x /= sqrt((1.0-JdotL*JdotL)*(1.0-LdotN*LdotN));
	
	// printf("sin(i) = %e,  sin(kappa) =  %e \n", sqrt(1.0-JdotL*JdotL), sqrt(1.0-LdotN*LdotN));
	
	gamma0 = acos(x); // Orbital phase rotation to match Kidder frame
	
	
	//-acos( (Lnz*Ldotn + cos(theta))/(sin(iota)*sqrt(1.-Ldotn*Ldotn)) );
	
	// printf("Orbital phase rotation to match Kidder frame = %f\n", gamma0);
	
	for(int i = 0; i < 9; i++) 
		LSvals[i] += Toffset*derivvals[i];  // back up the initial values to time Toffset
	thomasval = Toffset*thomasderiv;
	
	index = 0;  // index for the various storage vectors
	hcurrent = RK_H;  // ?? : 100000
	laststep = false;  // ?? : 2
	
	LdotS1 = calcLdotS1(LSvals);
	LdotS2 = calcLdotS2(LSvals);
	S1dotS2 = calcSdotS(LSvals);
	
	//  beta and sigma at time -Toffset
	
	beta = (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);     
	sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
	
	// Put in result vector the first value for time , L (mul, sin(phi), cos(phi)) , beta, sigma, thomas
	timevec[index] = tcurrent*TSUN;
	mulvec[index] = LSvals[2];
	sphilvec[index] = LSvals[1];
	cphilvec[index] = LSvals[0]; 
	betavec[index] = beta;
	sigmavec[index] = sigma;
	thomasvec[index] = thomasval; 
	
	fold = 0.0;
	
	// ** Start the integration:
	
	while(tcurrent < tfinal)
    {	
		
		f = Freq(tcurrent*TSUN, Mtot, Mchirp, eta, betavec[index], sigmavec[index], tc);
		x = pow(M_PI*Mtot*f*TSUN,2./3.);
		
		if ((f >= fold) && (x < xmax)) {  // Check to make sure we're chirping and PN valid
			
			if (tcurrent+hcurrent-tfinal > 0.) // tcurrent + hcurrent > tfinal
			{
				hcurrent = tfinal-tcurrent;
				laststep = true;
			}
			
			do
			{	    
				rkckstep(updatedvals, fourthordervals, &updatedthomas, hcurrent, LSvals, thomasval, tcurrent, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc);
				
				maxerr = 0.0;
				
				for(int i = 0; i <= 8; i++)
				{
					double errori = fabs((updatedvals[i]-fourthordervals[i])/RK_EPS);
					maxerr = (errori > maxerr) ? errori : maxerr;
				}
				
				oldh = hcurrent;
				
				if (maxerr > 1.) 
				{
					hcurrent *= 0.9*pow(maxerr,-0.25);
					laststep = false;
				}
				else if (maxerr <= 0.0006)
					hcurrent *= 4.;
				else
					hcurrent *= 0.9*pow(maxerr,-0.2);
			}
			while(maxerr > 1.);
			
			for (int i = 0; i <= 8; i++)
				LSvals[i] = updatedvals[i];
			
			thomasval = updatedthomas;
			
			if (laststep)
				tcurrent = tfinal;
			else
				tcurrent += oldh;
			
			index++;
			
			timevec[index] = tcurrent*TSUN;
			
			LdotS1 = calcLdotS1(LSvals);
			LdotS2 = calcLdotS2(LSvals);
			S1dotS2 = calcSdotS(LSvals);
			
			// ** Store data
			mulvec[index] = LSvals[2];
			sphilvec[index] = LSvals[1];
			cphilvec[index] = LSvals[0]; 
			
			betavec[index] =  (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
			
			sigmavec[index] = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
			
			thomasvec[index] = thomasval;
			
			// printf("%d %e %e %e %f %f %f %f\n", index, timevec[index], betavec[index], sigmavec[index], LdotS1, LdotS2, S1dotS2, thomasval);
			
			fold = f;
			
		}else {
			// If PN gone bad go to tfinal and keep everything the same.
			
			tmax = tcurrent*TSUN;  
			// At tmax, we've just barely gone beyond xmax
			
			tcurrent = tfinal;
			timevec[index] = tcurrent*TSUN;
			mulvec[index] = mulvec[index-1];
			sphilvec[index] = sphilvec[index-1];
			cphilvec[index] = cphilvec[index-1];
			betavec[index] = betavec[index-1];
			sigmavec[index] = sigmavec[index-1];
			
			thomasval = thomasvec[index-1];
			thomasvec[index] = thomasval;
			// printf("%d %e %e %e %f %f\n", index, timevec[index], betavec[index], sigmavec[index], thomasval);
			
		}
		
		if (index >= NbVecLength*RK_VECLENGTH) {
			throw invalid_argument("GWFastSpinBBH::init : Size of data vectors is too much small : change RK_VECLENGTH");
		}
    }
	// ** End of integration
	
	
	idxm = index;
	
	// Compute spline data
	spline(timevec, mulvec, index, 1.e30, 1.e30, muly2);
	spline(timevec, sphilvec, index, 1.e30, 1.e30, sphily2);
	spline(timevec, cphilvec, index, 1.e30, 1.e30, cphily2);
	spline(timevec, betavec, index, 1.e30, 1.e30, betay2);
	spline(timevec, sigmavec, index, 1.e30, 1.e30, sigmay2); 
	spline(timevec, thomasvec, index, 1.e30, 1.e30, thomasy2);
	
	// End precession. 
	
	/*
	FILE * TestOutFile;
	TestOutFile = fopen("TestOutGWFastSpinBBH.txt", "w");
	for(int i = 0; i<idxm; i++){
		fprintf(TestOutFile, "%d %e %e %e %e %e %e %e %e %e %e %e %e\n", i,timevec[i],mulvec[i],sphilvec[i],cphilvec[i],betavec[i],sigmavec[i],thomasvec[i],muly2[i],sphily2[i],cphily2[i],betay2[i],sigmay2[i],thomasy2[i]);
	}
	fclose(TestOutFile);
	*/
	 
	timeCur = -1.0e30;
	idxcur = 0;
	xold = 0.0;
	cout << " --> OK" << endl;
}


void GWFastSpinBBH::AmpShared(double t)
{
	double deltat;
	double A, B, C, D;
	double costhetaL, sinthetaL, cphil, sphil, phiL, beta, sigma, thomas;
	double xrt;
	double Phase, Phi15, Phi20, OPhase;
	double LdotN, taper;
	double ci2, ci4, ci6, si, si2, si4;
	double cPhi[7], sPhi[7]; 
	double TPI(2.0*M_PI);
	double ln2(0.693147180559945);   // ln 2
	double ln32(0.405465108108164);  // ln(3/2)
	bool ForwardStep(true);
	
	if(fabs(t-timeCur)>PRECISION){
		if(timeCur>t)
			ForwardStep = false;
		timeCur = t;
		
		// ** Compute of waveform
		
		if (t <= tmax) // Make sure we have data for the point.
		{
			// ** Interpolation
			
			while(t> timevec[idxcur+1]){
				idxcur++;
			}
			while(t< timevec[idxcur]){
				idxcur--;
				if(idxcur<0){
					cerr << "Required time is " << t << " s and first time of precession data is " << timevec[0] << " s." << endl;
					throw invalid_argument("GWFastSpinBBH::AmpShared : The required time is os small !");
				}
			}
			
			deltat = timevec[idxcur+1]-timevec[idxcur];
			
			A = (timevec[idxcur+1]-t)/deltat;
			B = 1.0-A;
			C = (pow(A,3.0)-A)*deltat*deltat/6.0;
			D = (pow(B,3.0)-B)*deltat*deltat/6.0;
			
			costhetaL = A*mulvec[idxcur] + B*mulvec[idxcur+1] + C*muly2[idxcur] + D*muly2[idxcur+1];
			sinthetaL = sqrt(1.-costhetaL*costhetaL);
			cphil = A*cphilvec[idxcur] + B*cphilvec[idxcur+1] + C*cphily2[idxcur] + D*cphily2[idxcur+1]; 
			sphil = A*sphilvec[idxcur] + B*sphilvec[idxcur+1] + C*sphily2[idxcur] + D*sphily2[idxcur+1];
			phiL = atan2(sphil,cphil);
			beta = A*betavec[idxcur] + B*betavec[idxcur+1] + C*betay2[idxcur] + D*betay2[idxcur+1];
			sigma = A*sigmavec[idxcur] + B*sigmavec[idxcur+1] + C*sigmay2[idxcur] + D*sigmay2[idxcur+1];
			thomas = -(A*thomasvec[idxcur] + B*thomasvec[idxcur+1] + C*thomasy2[idxcur] + D*thomasy2[idxcur+1]);
			
			sf = Freq(t, Mtot, Mchirp, eta, beta, sigma, tc);
			
			sx = pow(M_PI*Mtot*sf*TSUN,2./3.);
			
			//cout.precision(13);
			//cout << t << " " << timevec[idxcur] << " " << timevec[idxcur+1] << " ";
			//cout << costhetaL << " " << sinthetaL << " " << cphil << " " << sphil << " " << phiL  << " " << beta << " " << sigma << " " << thomas << " " << f << " " << x;
			
			
			if((sx < xmax) && ( (!ForwardStep) || ((ForwardStep)&&(sx > xold)) ) ) {
				// If forward step, control the system inspiral
				xold = sx;
				Phi15 = p150 - p15*beta;
				Phi20 = p200 - p20*(15./32.*sigma);
				
				Phase = 2.0*phic + etain*(Phi0*pow(tc-t,0.625) + Phi10*pow(tc-t,0.375) + Phi15*pow(tc-t,0.25) + Phi20*pow(tc-t,0.125)); 
				
				LdotN = costhetaL*costheta + sinthetaL*sintheta*cos(phiL - phi);
				
				taper = 0.5*(1.+tanh(TaperSteepness*(1./TaperApplied-sx)));
				
				Ax = 2.*Amp*pow(M_PI*sf,2./3.)*taper;
				
				ci = LdotN;
				ci2 = ci*ci;
				ci4 = ci2*ci2;
				ci6 = ci2*ci4;
				si2 = 1.0-ci2;
				si4 = si2*si2;
				si = sqrt(si2);
				xrt = sqrt(sx);
				
				OPhase = (Phase+thomas)/2.0 + gamma0;
				
				cPhi[1] = cos(OPhase);
				sPhi[1] = sin(OPhase);
				
				for(int j = 2; j <= 6; j++)
				{
					cPhi[j] = cPhi[j-1]*cPhi[1]-sPhi[j-1]*sPhi[1];
					sPhi[j] = sPhi[j-1]*cPhi[1]+cPhi[j-1]*sPhi[1];
				}
				
				/* dominant harmonic */
				shp = Ax*(1.0+ci2)*cPhi[2];
				shc = -2.0*Ax*ci*sPhi[2];
				
				
				if(AmplPNorder > 0.0)
				{
					/* 0.5 PN correction */
					shp += xrt*Ax*si*dm/8.0*((5.0+ci2)*cPhi[1]-9.0*(1.0+ci2)*cPhi[3]);
					shc += -xrt*Ax*3.0*si*ci*dm/4.0*(sPhi[1]-3.0*sPhi[3]); 
				}
				
				if(AmplPNorder > 0.5)
				{
					/* 1 PN correction */
					shp += -sx*Ax*(cPhi[2]*((19.0+9.0*ci2-2.0*ci4)-eta*(19.0-11.0*ci2-6.0*ci4))/6.0
								  -4.0*cPhi[4]*si2*(1.0+ci2)*(1.0-3.0*eta)/3.0);
					shc += sx*Ax*(sPhi[2]*ci*((17.0-4.0*ci2)-eta*(13.0-12.0*ci2))/3.0
								 -8.0*sPhi[4]*(1.0-3.0*eta)*ci*si2/3.0);
				}
				
				if(AmplPNorder > 1.0)
				{
					/* 1.5 PN correction */
					shp += -xrt*sx*Ax*((dm*si/192.0)*(cPhi[1]*((57.0+60.0*ci2-ci4)-2.0*eta*(49.0-12.0*ci2-ci4))
													 -27.0*cPhi[3]*((73.0+40.0*ci2-9.0*ci4)-2.0*eta*(25.0-8.0*ci2-9.0*ci4))/2.0
													 +625.0*cPhi[5]*(1.0-2.0*eta)*si2*(1.0+ci2)/2.0)
									  -TPI*(1.0+ci2)*cPhi[2]);
					shc += xrt*sx*Ax*((dm*si*ci/96.0)*(sPhi[1]*((63.0-5.0*ci2)-2.0*eta*(23.0-5.0*ci2))
													  -27.0*sPhi[3]*((67.0-15.0*ci2)-2.0*eta*(19.0-15.0*ci2))/2.0
													  +625.0*sPhi[5]*(1.0-2.0*eta)*si2/2.0)
									 -4.0*M_PI*ci*sPhi[2]);
				}
				
				if(AmplPNorder > 1.5)
				{
					/* 2 PN correction */
					shp += -sx*sx*Ax*(cPhi[2]*((22.0 + (396.0 * ci2) + (145.0 * ci4) - (5.0 * ci6))
											 +5.0*eta*(706.0 - (216.0 * ci2) - (251.0 * ci4) + (15.0 * ci6))/3.0
											 -5.0*eta*eta*(98.0 - (108.0 * ci2) + (7.0 * ci4) + (5.0 * ci6)))/120.0
									+cPhi[4]*2.0*si2*((59.0 + (35.0 * ci2) - (8.0 * ci4)) 
													  -5.0*eta*(131.0 + (59.0 * ci2) - (24.0 * ci4))/3.0
													  +5.0*eta*eta*(21.0 - (3.0 * ci2) - (8.0 * ci4)))/15.0
									-cPhi[6]*81.0*si4*(1.0+ci2)*(1.0-5.0*eta+5.0*eta*eta)/40.0
									+(dm*si/40.0)*(sPhi[1]*(11.0 + (7.0 * ci2) + (10.0 * (5.0+ci2)*ln2))
												   -cPhi[1]*5.0*M_PI*(5.0+ci2)
												   -sPhi[3]*27.0*(7.0-10.0*ln32)*(1.0+ci2)
												   +cPhi[3]*135.0*M_PI*(1.0+ci2)));
					shc += sx*sx*Ax*(sPhi[2]*ci*(68.0 + (226.0 * ci2) - (15.0 * ci4)
											   +5.0*eta*(572.0 - (490.0 * ci2) + (45.0 * ci4))/3.0
											   -5.0*eta*eta*(56.0 - (70.0 * ci2) + (15.0 * ci4)))/60.0
								   +sPhi[4]*4.0*ci*si2*((55.0-12.0*ci2)-5.0*eta*(119.0-36.0*ci2)/3.0
														+5.0*eta*eta*(17.0-12.0*ci2))/15.0
								   -sPhi[6]*81.0*ci*si4*(1.0-5.0*eta+5.0*eta*eta)/20.0
								   -(dm*3.0*si*ci/20.0)*(cPhi[1]*(3.0+10.0*ln2)+sPhi[1]*5.0*M_PI
														 -cPhi[3]*9.0*(7.0-10.0*ln32)-sPhi[3]*45.0*M_PI));        
				}
				
				
				psi = atan2(costheta*cos(phi-phiL)*sinthetaL-costhetaL*sintheta, sinthetaL*sin(phi-phiL));
				
				c2psi = cos(2.*psi);
				s2psi = sin(2.*psi);
				
				//cout.precision(13);
				//cout << t << " ";
				//cout << psi << " " << shp << " " << shc;
				//cout << endl;
			}
			
		}else{
			shp = 0.0;
			shc = 0.0;
		}
	}
}


double GWFastSpinBBH::hp(double t)
{
	AmpShared(t);
	return (-shp*c2psi - shc*s2psi) ;
}


double GWFastSpinBBH::hc(double t)
{
	AmpShared(t);
	return (shp*s2psi - shc*c2psi);
}


void GWFastSpinBBH::DispTempVal(double t, ostream * OutDisp)
{
	(*OutDisp) << " " << psi << " " << ci << " " << Ax << " " << sf << " " << sx;
}


/*Couple GWFastSpinBBH::hbin(double t)
{
	Couple hbin;
	AmpShared(t);
	hbin.x = -shp*c2psi - shc*s2psi;
	hbin.y = shp*s2psi - shc*c2psi;
	return hbin;
}
*/

double GWFastSpinBBH::Freq(double t, double Mtot, double Mchirp, double eta, double beta, double sigma, double tc)  
{
	double fac, f10, f15, f20, f;
	
	fac = eta*(tc-t)/(5.*Mtot*TSUN);
	
	f10 = 743./2688.+11./32.*eta;
	f15 = -3.*(4.*M_PI-beta)/40.;
	f20 = 1855099./14450688.+56975./258048.*eta+371./2048.*eta*eta-3./64.*sigma;
	
	f = (pow(fac,-3./8.) + f10*pow(fac,-5./8.) + f15*pow(fac,-3./4.) + f20*pow(fac,-7./8.))/8./M_PI/Mtot/TSUN;
	
	return(f);
}



void GWFastSpinBBH::rkckstep(double outputvals[], double fourthorderoutputvals[], double *outputthomas, double h, double currentvals[], double currentthomas, double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc)
{
	int i;
	
	double intvals[9];
	
	double kthomas[6];
	
	update(0, 0.0, h, currentvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) 
		intvals[i] = currentvals[i] + B21*krk[i][0];
	
	update(1, A2, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) intvals[i] = currentvals[i] + B31*krk[i][0] + B32*krk[i][1];
	
	update(2, A3, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) intvals[i] = currentvals[i] + B41*krk[i][0] + B42*krk[i][1] + B43*krk[i][2];
	
	update(3, A4, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) intvals[i] = currentvals[i] + B51*krk[i][0] + B52*krk[i][1] + B53*krk[i][2] + B54*krk[i][3];
	
	update(4, A5, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) intvals[i] = currentvals[i] + B61*krk[i][0] + B62*krk[i][1] + B63*krk[i][2] + B64*krk[i][3] + B65*krk[i][4];
	
	update(5, A6, h, intvals, t, m1, m2, Mtot, Mchirp, mu, eta, chi1, chi2, N, tc, krk, kthomas);
	
	for(i = 0; i < 9; i++) {
		outputvals[i] = currentvals[i] + C1*krk[i][0] + C2*krk[i][1] + C3*krk[i][2] + C4*krk[i][3] + C5*krk[i][4] + C6*krk[i][5];
		fourthorderoutputvals[i] = currentvals[i] + D1*krk[i][0] + D2*krk[i][1] + D3*krk[i][2] + D4*krk[i][3] + D5*krk[i][4] + D6*krk[i][5];
	}
	
	*outputthomas = currentthomas + C1*kthomas[0] + C2*kthomas[1] + C3*kthomas[2] + C4*kthomas[3] + C5*kthomas[4] + C6*kthomas[5]; 
}


void GWFastSpinBBH::update(int j, double A, double h, double intvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc, double **k, double kthomas[])
{
	
	double r;
	int i;
	double derivvals[9];
	double LcrossN[3];
	double LdotS1;
	double LdotS2;
	double S1dotS2;
	double LdotN;
	double thomasderiv;
	double beta;
	double sigma;
	
	LdotS1 = calcLdotS1(intvals);
	LdotS2 = calcLdotS2(intvals);
	S1dotS2 = calcSdotS(intvals);
	
	beta =  (LdotS1*chi1/12.)*(113.*(m1/Mtot)*(m1/Mtot) + 75.*eta) + (LdotS2*chi2/12.)*(113.*(m2/Mtot)*(m2/Mtot) + 75.*eta);
	sigma = eta*(721.*LdotS1*chi1*LdotS2*chi2 - 247.*S1dotS2*chi1*chi2)/48.;    
	
	r = pow(Mtot,1./3.)/pow(M_PI*Freq((t+A*h)*TSUN, Mtot, Mchirp, eta, beta, sigma, tc)*TSUN,2./3.);
	
	if (r == r) // catches NaNs
	{ // Compute derivs of L, S1 and S2
		calcderivvals(derivvals, intvals, r, m1, m2, Mtot, mu, chi1, chi2);
	}
	else 
	{ // If r is not a number, put derivs of L, S1 and S2 at 0
		for(i = 0; i < 9; i++) 
			derivvals[i] = 0.0;
	}
	
	LdotN = calcLdotN(intvals, N);
	
	calcLcrossN(LcrossN, intvals, N);
	
	thomasderiv = -2.*LdotN/(1.-LdotN*LdotN)*(LcrossN[0]*derivvals[0]+LcrossN[1]*derivvals[1]+LcrossN[2]*derivvals[2]);
    
	for(i = 0; i < 9; i++) 
		k[i][j] = h*derivvals[i];
	
	kthomas[j] = h*thomasderiv; 
}


// Calculates the L, S1 and S2 derivatives
void GWFastSpinBBH::calcderivvals(double derivvals[], double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	derivvals[3] = S1xdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[4] = S1ydot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[5] = S1zdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[6] = S2xdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[7] = S2ydot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[8] = S2zdot(inputs, r, m1, m2, Mtot, mu, chi1, chi2);
	derivvals[0] = (-chi1*pow(m1,2.)*derivvals[3]-chi2*pow(m2,2.)*derivvals[6])/(mu*sqrt(Mtot*r));
	derivvals[1] = (-chi1*pow(m1,2.)*derivvals[4]-chi2*pow(m2,2.)*derivvals[7])/(mu*sqrt(Mtot*r));
	derivvals[2] = (-chi1*pow(m1,2.)*derivvals[5]-chi2*pow(m2,2.)*derivvals[8])/(mu*sqrt(Mtot*r));
}

// Compute S1xdot : eq.(11b) ref[Apostolatos_PRD49_12] ?? why chi2*pow(m2,2.) ??
double GWFastSpinBBH::GWFastSpinBBH::S1xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[1]*inputs[5]-inputs[2]*inputs[4])+0.5*chi2*pow(m2,2.)*(inputs[7]*inputs[5]-inputs[8]*inputs[4]))/(pow(r,3.));
}

// Compute S1ydot : eq.(11b) ref[Apostolatos_PRD49_12] ?? why chi2*pow(m2,2.) ??
double GWFastSpinBBH::S1ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[2]*inputs[3]-inputs[0]*inputs[5])+0.5*chi2*pow(m2,2.)*(inputs[8]*inputs[3]-inputs[6]*inputs[5]))/(pow(r,3.));
}

// Compute S1zdot : eq.(11b) ref[Apostolatos_PRD49_12] ?? why chi2*pow(m2,2.) ??
double GWFastSpinBBH::S1zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m2/m1)*mu*sqrt(Mtot*r)-1.5*chi2*pow(m2,2.)*calcLdotS2(inputs))*(inputs[0]*inputs[4]-inputs[1]*inputs[3])+0.5*chi2*pow(m2,2.)*(inputs[6]*inputs[4]-inputs[7]*inputs[3]))/(pow(r,3.));
}

// Compute S2xdot  : eq.(11c) ref[Apostolatos_PRD49_12] ?? why chi1*pow(m1,2.) ??
double GWFastSpinBBH::S2xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)*calcLdotS1(inputs))*(inputs[1]*inputs[8]-inputs[2]*inputs[7])+0.5*chi1*pow(m1,2.)*(inputs[4]*inputs[8]-inputs[5]*inputs[7]))/(pow(r,3.));
}

// Compute S2ydot : eq.(11c) ref[Apostolatos_PRD49_12] ?? why chi1*pow(m1,2.) ??
double GWFastSpinBBH::S2ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)*calcLdotS1(inputs))*(inputs[2]*inputs[6]-inputs[0]*inputs[8])+0.5*chi1*pow(m1,2.)*(inputs[5]*inputs[6]-inputs[3]*inputs[8]))/(pow(r,3.));
}

// Compute S2zdot : eq.(11c) ref[Apostolatos_PRD49_12] ?? why chi1*pow(m1,2.) ??
double GWFastSpinBBH::S2zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2)
{
	return (((2.+1.5*m1/m2)*mu*sqrt(Mtot*r)-1.5*chi1*pow(m1,2.)* calcLdotS1(inputs) )*(inputs[0]*inputs[7]-inputs[1]*inputs[6])+0.5*chi1*pow(m1,2.)*(inputs[3]*inputs[7]-inputs[4]*inputs[6]))/(pow(r,3.));
}

double GWFastSpinBBH::calcLdotS1(double inputs[])
{
	return inputs[0]*inputs[3]+inputs[1]*inputs[4]+inputs[2]*inputs[5];
}

double GWFastSpinBBH::calcLdotS2(double inputs[])
{
	return inputs[0]*inputs[6]+inputs[1]*inputs[7]+inputs[2]*inputs[8];
}

double GWFastSpinBBH::calcSdotS(double inputs[])
{
	return inputs[3]*inputs[6]+inputs[4]*inputs[7]+inputs[5]*inputs[8];
}

double GWFastSpinBBH::calcLdotN(double inputs[], double nvec[])
{
	return inputs[0]*nvec[0] + inputs[1]*nvec[1] + inputs[2]*nvec[2];
}

void GWFastSpinBBH::calcLcrossN(double output[], double inputs[], double nvec[])
{
	output[0] = inputs[1]*nvec[2]-inputs[2]*nvec[1];
	output[1] = inputs[2]*nvec[0]-inputs[0]*nvec[2];
	output[2] = inputs[0]*nvec[1]-inputs[1]*nvec[0];
}

void GWFastSpinBBH::spline(double *x, double *y, int n, double yp1, double ypn, double *y2)
// Unlike NR version, assumes zero-offset arrays.  CHECK THAT THIS IS CORRECT.
{
	int i, k;
	double p, qn, sig, un;
	//double *uspline;
	
	//uspline = (double*)MT->AllocMemory((n-2)*sizeof(double));
	
	// Boundary conditions: Check which is best.  
	if (yp1 > 0.99e30)
		y2[0] = uspline[0] = 0.0;
	else {
		y2[0] = -0.5;
		uspline[0] = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
	}
	
	for(i = 1; i < n-1; i++) {
		sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p = sig*y2[i-1] + 2.0;
		y2[i] = (sig-1.0)/p;
		uspline[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		uspline[i] = (6.0*uspline[i]/(x[i+1]-x[i-1])-sig*uspline[i-1])/p;
	}
	
	// More boundary conditions.
	if (ypn > 0.99e30)
		qn = un = 0.0;
	else {
		qn = 0.5;
		un = (3.0/(x[n-1]-x[n-2]))*(ypn-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
	}
	
	y2[n-1] = (un-qn*uspline[n-2])/(qn*y2[n-2]+1.0);
	
	for (k = n-2; k >= 0; k--)
		y2[k] = y2[k]*y2[k+1]+uspline[k];
	
	// Add A. Petiteau
	y2[n] = 0.0;
}



// end of LISACODE-GWFastSpinBBH.cpp
