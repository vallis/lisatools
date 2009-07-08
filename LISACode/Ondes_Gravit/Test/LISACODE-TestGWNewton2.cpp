// $Id: LISACODE-TestGWNewton2.cpp,v 1.1 2007/02/19 10:00:28 ruiloba Exp $
/*
 *  LISACODE-TestGWNewton2.cpp
 *  V 1.4.1
 *  LISACode
 *
 *  Created on 16/11/06 by Antoine PETITEAU (APC)
 *  Last modification on 22/05/07 by Antoine PETITEAU (APC)
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWFile.h"
#include "LISACODE-GWBinary.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-GWNewton2.h"
using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *************************************";
		cout << endl << "   *                                   *";
		cout << endl << "   *   Test une onde gravitationnelle  *";
		cout << endl << "   *   ------------------------------  *";
		cout << endl << "   *     issue d'un systeme binaire    *";
		cout << endl << "   *     --------------------------    *";
		cout << endl << "   *         en calcul 2.5 PN          *";
		cout << endl << "   *         ----------------          *";
		cout << endl << "   *         (LISACode v "<<LCVersion<<")         *";
		cout << endl << "   *                                   *";
		cout << endl << "   **************************************" << endl << endl; 
		
		
		/*  Donnees
			- masse1 masse du premier
			- masse2 masse du second
			- inclinaison angle d'inclinaison
			- distance distance en parsec
			- phase phase a l'origine
			- t_coalescence temps avant la coalescence
			- min-temps temps de depart du calcul (min-temps = 0 ==> calcul a tcoal)
			- max_temps temps de fin 
			- echantillonage
			*/
		
		double BetaGW(MathUtils::deg2rad(20.0)), LambdaGW(MathUtils::deg2rad(240.0));
		double AnglPolGW(MathUtils::deg2rad(0.0));
		double hplus,hx,freq,pha,temps,masse1,masse2,t_coalescence,inclinaison,phase,distance;
		double taud0,omega0,gw;
		int  type,i;
		double min_temps, max_temps,echant;
		
		masse1        = 1e5;
		masse2        = 1e5;
		t_coalescence = 60000;
		inclinaison   = M_PI/2.0; 
		phase         = 0.;
		distance      = 1.0e6;
		min_temps     = 0;
		max_temps     = 650000;
		echant        = 100;
		
		// Entree
		
		cout<<" Ordre 1 (type=1), Ordre 2 (type=2) : ";
		cin>>type;
		cout<<" Masse du premier : ";
		cin>>masse1;
		cout<<" Masse du second  : ";
		cin>>masse2;
		cout<<" Temps de coalescence : ";
		cin>>t_coalescence;
		cout<<" Inclinaison du plan (radians) : ";
		cin>>inclinaison;
		cout<<" Phase a l'origine : ";
		cin>>phase;
		cout<<" Distance (KiloParsec) : ";
		cin>>distance;
		cout<<" Start : ";
		cin>>min_temps; 
		cout<<" Stop  : ";
		cin>>max_temps;
		cout<<" echantillonage : ";
		cin>>echant;
		
		if(type>1)    //   Parametre omega0, taud0 et gw inutile en ordre 1
		{
			cout<<" omega0 : ";
			cin>>omega0;
			cout<<" taud0  : ";
			cin>>taud0;
			cout<<" gw     : ";
			cin>>gw;
		}
		else
		{
			omega0 = 0.;
			taud0  = 0.;
			gw     = 1.;
		}
		
		ofstream MonFichier;   // ouverture fichier truc
		MonFichier.open("ordre1_5an_105-1.txt");
		
		MonFichier << "# Simulation from " << min_temps << "s to " << max_temps << "s by step " << echant << "s." << endl;
		MonFichier << "# PostNewtonian GW : type " << type << " , M1 " << masse1 << " , M2 " <<masse2 << " , tcoal " << t_coalescence << " , i " << inclinaison << " , phi0 " << phase << " , r " << distance << " , omega0 " << omega0 << " , taud0 " << taud0 << " , gw " << gw << endl;
		MonFichier << "#Time hp hc frequency phase" << endl; 
		GWNewton2 MonOnde(BetaGW, LambdaGW, AnglPolGW, type, masse1, masse2, t_coalescence, inclinaison, phase, distance, omega0, taud0, gw);
		//GWNewton2 MonOnde(0.64, 5., 5.93, 2, 1e6, 1e6, 600000, 1.57, 0, 1e6, 10., 10., 1.);
		
		// Appel de la fonction
		i = 0;
		do{
			i++;
			temps = min_temps + i*echant ; 
			hplus= MonOnde.hp(temps);
			hx   = MonOnde.hc(temps);
			freq = MonOnde.fe(temps);
			pha  = MonOnde.phase(temps);
			MonFichier.precision(15);
			MonFichier << temps << " " << hplus << " "<< hx << "   " << freq << " " << pha << endl;   // Ecriture dans fichier
																			 //  cout<<" temps :"<<temps<<" hplus :"<<hplus<<" hx :"<<hx<<" fre :"<<freq;
		}while(temps<=max_temps);
		MonFichier.close();
		
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestGWNewton2: error: " << e.what()<<endl;
		cerr << "TestGWNewton2: abort!" << endl;
		exit(1);
	}
	return(0);
}



// end of TestGW.cpp