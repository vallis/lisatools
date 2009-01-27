/*
 *  LISACODE-GWSto.cpp
 *  stochastique
 *
 *  Created by Auger Gerard (APC) on 22/10/07.
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI).
 *
 */

#include "LISACODE-GWSto.h"




// *****************
// *  Constructor  *
// *****************

GWSto::GWSto():GW(0,0,0)
{

}


GWSto::GWSto(	double Beta_n,
			 double Lambda_n,
			 double tStep_n,
			 double Slope_n,
			 double Fknee_n , 
			 double Fmin_n,
			 double Fac_Hp_n,
			 double Fac_Hc_n):
GW(Beta_n, Lambda_n, 0.0)
{
	
	tStep      = tStep_n;
	Fknee      = Fknee_n;
	Fmin       = Fmin_n;
	Fac_Hp     = Fac_Hp_n;
	Fac_Hc     = Fac_Hc_n;
	Slope      = Slope_n;
	
	
	/* Warning tfist and tLast are fixed with two considerations
	 - The interpolation is fixed at 7 OrderLa = 7.
	 - And tmax is fixed at 40 secondes.
	 */
	OrderLa = 3;
	double tmax = 10000.;
	
	tFirst = tStep*ceil(2+OrderLa/2.0);
	tLast = -1.0*(tmax+tStep*ceil(2+OrderLa/2.0));
	
	tDurAdd    = 40*tStep;
	Nb_Ageing  = 10000;
	
	init();	
}	




GWSto::GWSto( 	double Beta_n,
			 double Lambda_n,
			 double tStep_n,
			 double tDurAdd_n,
			 double tFirst_n,
			 double tLast_n,
			 double tder_n,
			 double Slope_n, 
			 double Tsample_n, 
			 double Fknee_n , 
			 double Fmin_n,
			 int Nb_Ageing_n,
			 double Fac_Hp_n,
			 double Fac_Hc_n):
GW(Beta_n, Lambda_n, 0.0)
{
	
	tStep     = tStep_n;
	tDurAdd   = tDurAdd_n;
	tFirst    = tFirst_n;
	tLast     = tLast_n;          // Warning tFirst and tLast need to be a multiple of tStep.
	tder_hp   = tder_n;
	tder_hc   = tder_hp;
	Slope = Slope_n;
	Tsample = Tsample_n;
	Fknee = Fknee_n;
	Fmin  = Fmin_n;
	Nb_Ageing = Nb_Ageing_n;
	Fac_Hp = Fac_Hp_n;
	Fac_Hc = Fac_Hc_n;
	
	// Appel du bruit
	
	Tsample   = tStep;
	tDurAdd   = 10*tStep;
	
	init();
}


// **********************
// **  Access methods  **
// **********************
void GWSto::setParam(int iP, double Param_n){
	switch (iP) {
		case 0:
			Beta = Param_n;
			break;
		case 1:
			Lambda = Param_n;
			break;
		case 2:
			Fknee = Param_n;
			break;
		case 3:
			Fmin = Param_n;
			break;
		case 4:
			Fac_Hp = Param_n;
			break;
		case 5:
			Fac_Hc = Param_n;
			break;
		case 6:
			Slope = Param_n;
			break;
		default:
			break;
	}
}

double GWSto::getParam(int iP){
	switch (iP) {
		case 0:
			return (Beta);
			break;
		case 1:
			return (Lambda);
			break;
		case 2:
			return (Slope);
			break;
		case 3:
			return (Fknee);
			break;
		case 4:
			return (Fmin);
			break;
		case 5:
			return (Fac_Hp);
			break;
		case 6:
			return (Fac_Hc);
			break;
		default:
			return (0.0);
			break;
	}
}



// **********************
// **  Others methods  **
// **********************

void GWSto::init()
{
	/*
	cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;
	cout << "- tStep     = " << tStep << endl;
	cout << "- tDurAdd   = " << tDurAdd << endl;
	cout << "- tFirst    = " << tFirst << endl;
	cout << "- tLast     = " << tLast << endl;          // Warning tFirst and tLast need to be a multiple of tStep.
	cout << "- tder_hp   = " << tder_hp << endl;
	cout << "- tder_hc   = " << tder_hc << endl;
	cout << "- Slope     = " << Slope << endl;
	cout << "- Tsample   = " << Tsample << endl;
	cout << "- Fknee     = " << Fknee << endl;
	cout << "- Fmin      = " << Fmin << endl;
	cout << "- Nb_Ageing = " << Nb_Ageing << endl;
	cout << "- Fac_Hp    = " << Fac_Hp << endl;
	cout << "- Fac_Hc    = " << Fac_Hc << endl;
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	 */
	
	// Appel du bruit
	
	Tsample   = tStep;
	tDurAdd   = 10*tStep;
	
	
	
	
	//  tder_hp and tder_hc value just for the init of GetHp and Hc
	tder_hp    = -2000.;
	tder_hc    = -2000.;
	
	
	//cout << "GWSTO : Nb_ageing = " << Nb_Ageing << endl ;
	NFhp =  new NoiseOof(tStep, tDurAdd, tFirst, tLast, Fmin, Fknee , Slope, Nb_Ageing);
	NFhc =  new NoiseOof(tStep, tDurAdd, tFirst, tLast, Fmin, Fknee , Slope, Nb_Ageing);
	//NFhp =  new NoiseWhite(tStep, tDurAdd, tFirst, tLast,1.);
	//NFhc =  new NoiseWhite(tStep, tDurAdd, tFirst, tLast,1.);
	
	//	Normalization factor  = sqrt(1/Fknee^3)	
	
	int power;
	power = (int)(-1.*Slope);
	Fac_norm =1./(pow(Fknee,power));
	Fac_norm = pow(Fac_norm,0.5);
	// NFhp =  new NoiseWhite(tStep, tDurAdd, tFirst, tLast);
	//NFhc =  new NoiseWhite(tStep, tDurAdd, tFirst, tLast);
	//Fac_norm =1 ;    
	//cout<<" First Norm ="<< Fac_norm <<" Fmax ="<<Fknee<<endl;
	
	//tDelMax = 0.0;
}

double GWSto::hp(double t)
{
	double tAddp(0.0);
	//cout << " + " << t ;
	if (t>tder_hp)
	{
		//cout << " --> addnoise for " << (int)(ceil((t-tder_hp)/tDurAdd)) << " : " << tder_hp << " :: " << tDurAdd << " > " << (t-tder_hp)/tDurAdd ;
		for(int i=0; i< (int)(ceil((t-tder_hp)/tDurAdd));i++)
		{
			NFhp->addNoise();
			tAddp += tDurAdd;
		}
		tder_hp += tAddp;
		//nAddhp++;
		//cout << "-->" <<  nAddhp << endl;
	}
	//cout << " ===> hp = " << NFhp->getNoise(t-tder_hp) << endl; 
	
	//if(tder_hp-t>tDelMax){
	//	tDelMax = tder_hp-t;
	//	cout << tDelMax << endl;
	//}
	return NFhp->getNoise((t-tder_hp),OrderLa)*Fac_norm*Fac_Hp;
}


double GWSto::hc(double t)
{
	
	double tAddc(0.0);
	//cout << " + " << t ;
	if (t>tder_hc)
	{
		//cout << " --> addnoise for " << (int)(ceil((t-tder_hp)/tDurAdd)) << " : " << tder_hp << " :: " << tDurAdd << " > " << (t-tder_hp)/tDurAdd ;
		for(int i=0; i< (int)(ceil((t-tder_hc)/tDurAdd));i++)
		{
			NFhc->addNoise();
			tAddc += tDurAdd;
		}
		tder_hc += tAddc;
		//nAddhp++;
		//cout << "-->" <<  nAddhp << endl;
	}
	//cout << " ===> hp = " << NFhp->getNoise(tder_hp-t) << endl; 
	
	return NFhc->getNoise((t-tder_hc),OrderLa)*Fac_norm*Fac_Hc;
	
	
}



void GWSto::DispTempVal(double t, ostream * OutDisp)
{
	
}

// end of LISACODE-GWSto.cpp
