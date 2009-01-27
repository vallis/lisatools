/*
 *  LISACODE-TestGWCusp.cpp
 *  V 1.4.2
 *  LISACode
 *
 *  Created on 26/08/08 by Antoine PETITEAU (AEI)
 *  Last modification on 26/08/08 by Antoine PETITEAU (AEI)
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GWCusp.h"
using namespace std;

const char Version[] = " version 1.0.0 ";

int main (int argc, char * const argv[])
{
	try {
		
		// ***** Help *****
		if((argc>1)&&(strcmp(argv[1],"--help")==0)){
			cout << " ---- HELP of TestGWCusp ----" << endl;
			cout << endl << "\tExecution : " << endl;
			cout << "\t\t (./)TestGWCusp [Options] OutputFileName" << endl ;
			cout << endl << "\tArguments :" << endl;
			cout << "\t\t * OutputFileName    : Output file which will contain t, h+, hx."  << endl ;
			//cout << endl << "\tOptions :" << endl;
			//cout << "\t\t * -? : ???."  << endl ;
			cout << endl << " --------" << endl << endl;
			return 0;
		}
		// ***** Version *****
		if((argc>1)&&(strcmp(argv[1],"--version")==0)){
			cout << " TestGWCusp - version " << Version << endl;
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
			strcpy(OutFileName,"/Users/petiteau/Applications/src/GWSrc/CosmicCusp/build/Debug/TestCusp.txt");
		}
		
		
		
		
		dt = 1.0;
		//tOffset = -900.0;
		tOffset = -0.5;
		tObs = 2097152.0;
		tDisp = 86400.0;
		
		tMax = tObs - tOffset;
		
		
		GWCusp MyGW(0.7598,		// Ecliptic latitude (beta in radians)
					4.7623,		// Ecliptic longitude (lambda in radians)
					2.31,		// Polarization  (psi in radians)          
					1.0e-10,	// Amplitude  (Hz^{1/3})
					1.0e6,		// Central Time (in seconds)
					0.1,		// MaximumFrequency (Hz)
					dt,			// Time step (in seconds)
					tObs,		// Observation time (in seconds)
					900.0		// Tpad (in seconds)
					);
		
		
		t = tOffset;
		tlD = t;
		OutFile.open(OutFileName);
		OutFile.precision(13);
		
		double c2psi(cos(2.0*MyGW.getParam(2)));
		double s2psi(sin(2.0*MyGW.getParam(2)));
		
		do{
			OutFile << t << " " << MyGW.hp(t)*c2psi << " " << -MyGW.hp(t)*s2psi << endl;
			if(tlD> tDisp){
				tlD = 0.0;
				cout << t << " " << MyGW.hp(t)*c2psi << " " << -MyGW.hp(t)*s2psi << endl;
			}
			tlD +=dt;
			t += dt;
		}while(t <= tMax);
		cout << t << " " << MyGW.hp(t)*c2psi << " " << -MyGW.hp(t)*s2psi << endl;
		
		OutFile.close();
		
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestGWCusp: error: " << e.what()<<endl;
		cerr << "TestGWCusp: abort!" << endl;
		exit(1);
	}
	return(0);
}



// end of TestGWCusp.cpp