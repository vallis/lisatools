// $Id: LISACODE-TestGWFastSpinBBH.cpp,v 1.1 2007/02/19 10:00:28 ruiloba Exp $
/*
 *  LISACODE-TestGWFastSpinBBH.cpp
 *  V 1.4.2
 *  LISACode
 *
 *  Created on 21/08/08 by Antoine PETITEAU (AEI)
 *  Last modification on 21/08/08 by Antoine PETITEAU (AEI)
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GWFastSpinBBH.h"
using namespace std;

const char Version[] = " version 1.1.0 ";

int main (int argc, char * const argv[])
{
	try {
		
		// ***** Help *****
		if((argc>1)&&(strcmp(argv[1],"--help")==0)){
			cout << " ---- HELP of TestMF ----" << endl;
			cout << endl << "\tExecution : " << endl;
			cout << "\t\t (./)TestGWFastSpinBBH [Options] OutputFileName" << endl ;
			cout << endl << "\tArguments :" << endl;
			cout << "\t\t * OutputFileName    : Output file which will contain t, h+, hx."  << endl ;
			//cout << endl << "\tOptions :" << endl;
			//cout << "\t\t * -? : ???."  << endl ;
			cout << endl << " --------" << endl << endl;
			return 0;
		}
		// ***** Version *****
		if((argc>1)&&(strcmp(argv[1],"--version")==0)){
			cout << " TestGWFastSpinBBH - version " << Version << endl;
			return 0;
		}
		
		
		cout << endl << "   *************************************";
		cout << endl << "   *                                   *";
		cout << endl << "   *   Test calculation of GW emitted  *";
		cout << endl << "   *   ------------------------------  *";
		cout << endl << "   *   by spinning black holes binary  *";
		cout << endl << "   *   ------------------------------  *";
		cout << endl << "   *         ("<<LCVersion<<")         *";
		cout << endl << "   *                                   *";
		cout << endl << "   **************************************" << endl << endl; 
		
		
		int nOption(0);
		
		double t, dt, tOffset, tMax, tObs, tDisp, tlD;
		
		char OutFileName[256];
		ofstream OutFile;
		
		// ***** Read arguments *****
		if(argc-nOption>1){
			strcpy(OutFileName,argv[1+nOption]);
		}else{
			strcpy(OutFileName,"TestFastSpinBBH.txt");
		}
		
		
		
		
		dt = 15.0;
		//tOffset = -900.0;
		tOffset = -1500.0;
		tObs = 62914545.0;
		//tObs = 122880.0;
		tDisp = 604800.0;
		//tDisp = 1e30;
		
		tMax = tObs - tOffset;
		
		
		/*GWFastSpinBBH MyGW(0.759892750778,			// Ecliptic latitude (beta)
					   4.76236796807,			// Ecliptic longitude (lambda)
					   2906387.81661,			// mass1  (Msolar)          
					   1056812.14434,			// mass2  (Msolar)
					   19395657.1969,			// time of coalescence (s)
					   1000000.0,				// Distance (kpc)
					   0.76622880068,			// Spin1		
					   0.357815817167,			// Spin2
					   acos(-0.158856838338),	// Polar angle of Spin1,
					   acos(-0.482166499414),	// Polar angle of Spin2,    
					   3.21243381724,			// Azimuthal angle of Spin1
					   2.5442762228,			// Azimuthal angle of Spin2
					   3.1710410372,			// Phase at coalescence
					   2.02230588043,			// Initial polar angle L
					   4.74885787067,			// Initial azimuthal angle L
					   2.0,						// AmplPNorder
					   tOffset,					// Time offset
					   tObs,					// Duration of observation
					   7.0,						// Taper applied
					   150.0,					// Taper Steepness
					   6.0						// Rmin
					   );
		GWFastSpinBBH MyGW(0.657644432309,		// Ecliptic latitude (beta)
						   4.98921990792,		// Ecliptic longitude (lambda)
						   1465580.75942,		// mass1  (Msolar)          
						   868710.795073,		// mass2  (Msolar)
						   31525128.7938,		// time of coalescence (s)
						   1000000.0,			// Distance (kpc)
						   0.0770493004546,		// Spin1		
						   0.268523395273,		// Spin2
						   2.79680864369,		// Polar angle of Spin1,
						   2.27928544629,		// Polar angle of Spin2,    
						   0.0140107251252,		// Azimuthal angle of Spin1
						   4.17130033043,		// Azimuthal angle of Spin2
						   1.16933157672,		// Phase at coalescence
						   2.82873004221,		// Initial polar angle L
						   5.74015227561,		// Initial azimuthal angle L
						   0.0,					// AmplPNorder
						   tOffset,				// Time offset
						   tObs,				// Duration of observation
						   7.0,					// Taper applied
						   150.0,				// Taper Steepness
						   6.0					// Rmin
						   );
		 */
		GWFastSpinBBH MyGW(-0.90706300796,		// Ecliptic latitude (beta)
						   2.85511464611,		// Ecliptic longitude (lambda)
						   2661328.26284,		// mass1  (Msolar)          
						   1314050.98983,		// mass2  (Msolar)
						   6991981.17503,		// time of coalescence (s)
						   3044822.92821,			// Distance (kpc)
						   0.158093270585,		// Spin1		
						   0.590137842134,		// Spin2
						   2.69715140063,		// Polar angle of Spin1,
						   0.352192466745,		// Polar angle of Spin2,    
						   0.0853155091808,		// Azimuthal angle of Spin1
						   2.81415074689,		// Azimuthal angle of Spin2
						   4.88963284396,		// Phase at coalescence
						   0.541822592245,		// Initial polar angle L
						   1.23477696778,		// Initial azimuthal angle L
						   0.0,					// AmplPNorder
						   tOffset,				// Time offset
						   tObs,				// Duration of observation
						   7.0,					// Taper applied
						   150.0,				// Taper Steepness
						   6.0					// Rmin
						   );
		
		t = tOffset;
		tlD = t;
		OutFile.open(OutFileName);
		OutFile.precision(13);
		do{
			OutFile << t << " " << MyGW.hp(t) << " " << MyGW.hc(t) << endl;
			if(tlD> tDisp){
				tlD = 0.0;
				cout << t << " " << MyGW.hp(t) << " " << MyGW.hc(t) << endl;
			}
			tlD +=dt;
			t += dt;
		}while(t <= tMax);
		cout << t << " " << MyGW.hp(t) << " " << MyGW.hc(t) << endl;
		
		OutFile.close();
		
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestGWFastSpinBBH: error: " << e.what()<<endl;
		cerr << "TestGWFastSpinBBH: abort!" << endl;
		exit(1);
	}
	return(0);
}



// end of TestGWFastSpinBBH.cpp