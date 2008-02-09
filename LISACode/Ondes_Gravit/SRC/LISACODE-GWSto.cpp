/*
 *  LISACODE-GWSto.cpp
 *  stochastique
 *
 *  Created by Auger Gerard on 22/10/07.
 *  Copyright 2007 __APC__. All rights reserved.
 *
 */

#include "LISACODE-GWSto.h"




/* Constructor */

GWSto::GWSto():GW(0,0,0)
{
}
/* Cree une onde avec un bruit en 1/f^Slope.
  Beta_n         : IN Beta and Lambda of the source.
  Lambda_n		 :
  tStep_n        : In Sample
  Fknee_n        : In Minimum value where the shape = 1/f^3.
  Fmin_n         : In Maximum value where the shape = 1/f^3.
  Fac_Hp_n       : In Multiplicatif parameter on the amplitude of Hp.
  Fac_Hc_n       :  In Multiplicatif parameter on the amplitude of Hc.
 */


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
	
//  tder_hp and tder_hc value just for the init of GetHp and Hc
	tder_hp    = -2000.;
	tder_hc    = -2000.;	
	
	
//Call Noise	
	cout << "GWSTO : Nb_ageing = " << Nb_Ageing << endl ;
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
	cout<<" First Norm ="<<Fac_norm<<" Fmax ="<<Fknee<<endl;

	//tDelMax = 0.0;
	
}	

/* Cree une onde avec un bruit en f^Slope.
  Beta_n         : IN Beta and Lambda of the source.
  Lambda_n		 :
  tStep_n        : In Sample.
  tDurAdd_n      : In Number of events created with AddNoise.
  tFirst_n       : In Parameter for AddNoise.
  tLast_n        : In  Parameter for AddNoise.
  tder_n         : In  Parameter for AddNoise.
  Slope_n        : In Slope of f^Slope_n.
  Tsample_n      : In In general = tStep_n.
  Fknee_n        : In Minimum value where the shape = f^Slope.
  Fmin_n         : In Maximum value where the shape = f^Slope.
  Nb_Ageing_n    : Number of events for sabilization.
  Fac_Hp_n       : In Multiplicatif parameter on the amplitude of Hp.
  Fac_Hc_n       : In Multiplicatif parameter on the amplitude of Hc.
 */


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

	NFhp =  new NoiseOof(tStep, tDurAdd, tFirst, tLast, Fmin, Fknee , Slope, Nb_Ageing);
	NFhc =  new NoiseOof(tStep, tDurAdd, tFirst, tLast, Fmin, Fknee , Slope, Nb_Ageing);
	


//	Normalization factor  = sqrt(1/Fknee^3)	



	int power;
	power = (int)(-1.*Slope);
	Fac_norm =1./(pow(Fknee,power));	
	Fac_norm = pow(Fac_norm,0.5);
//	cout<<" Second Norm ="<<Fac_norm<<" Fmax ="<<Fknee<<endl;
	
}

/* Methodes d'acces */ 


/* Autres methodes */

/*! \brief Gives h_plus polarisation component depending on \a t input time.
 *

 */
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

/*! \brief Gives h_cross polarisation component depending on \a t input time.
 *
 */
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


// end of LISACODE-GWSto.cpp
