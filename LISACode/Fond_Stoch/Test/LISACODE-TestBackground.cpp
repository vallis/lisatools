// $Id: LISACODE-TestBackground.cpp,v 1.2 2007/02/19 10:17:04 ruiloba Exp $
/*
 *  LISACODE-TestBackground.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *
 *  Created on 01/06/06 by Antoine PETITEAU (APC)
 *  Last modification on 01/06/06 by Antoine PETITEAU (APC)
 *
 */



#include <stdexcept>
#include <iostream>
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-Background.h"
#include "LISACODE-BackgroundGalactic.h"
#include "LISACODE-GeometryAnalytic.h"

using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   ****************************************";
		cout << endl << "   *                                      *";
		cout << endl << "   *          Test for simulation         *";
		cout << endl << "   *          -------------------         *";
		cout << endl << "   *  Confusion Whites Dwarfs Background  *";
		cout << endl << "   *  ----------------------------------  *";
		cout << endl << "   *          (LISACode v "<<LCVersion<<")           *";
		cout << endl << "   *                                      *";
		cout << endl << "   ****************************************" << endl << endl; 
		
		
		
		// **************************
		// * Debut des declarations *
		// **************************
		cout << "Creation of parameters ..." << endl;
		
		// Declaration of variables which control the simulation
		double tStep(5.0);
		double t(364.0*24.0*3600.0);
		double tmax(t+5.0*24.0*3600.0);
		double ttmpAff(0.0), StepdDisplay(24.0*3600.0);
		
		// Geometrie
		GeometryAnalytic LISAPos(0.0, 0.0, L0_m_default, 2, 1, tStep);
		
		// Phasemeters' File for CWDB
		char * FileName;
		double Factor(1000.0);
		FileName = "SigPhaMetSC.txt";
		// Creation of background
		BackgroundGalactic CWDB(&LISAPos, FileName, Factor);
		
		// Output file
		ofstream OutputFile;
		
		cout << "  Creation --> OK" << endl << endl;
			
			
		
			
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		cout << " t0   = " << t << endl;
		cout << " tmax = " << tmax << endl;
		OutputFile.open("OutputTestBackground.txt");
		OutputFile << "#Time s1 s2 s3 sp1 sp2 sp3" << endl;
		
		cout << "  Initialisations --> OK" << endl << endl;
			
			
			
			
		// *************
		// * Execution *
		// *************
			
		cout << "Running in progress (->"<< tmax << "s) ..." << endl;
		OutputFile.precision(15);
		do{
			t += tStep;
			
			//Display and record
			ttmpAff += tStep;
			if(ttmpAff >= StepdDisplay){
				cout << t/(24.0*3600.0) << " days" << endl;
				ttmpAff = 0.0;
			}
				
			// Record phasemeter's signal
			OutputFile << t << " " << CWDB.deltanu(1,0,t) << " " << CWDB.deltanu(2,0,t) << " ";
			OutputFile << CWDB.deltanu(3,0,t) << " " << CWDB.deltanu(1,1,t) << " ";
			OutputFile << CWDB.deltanu(2,1,t) << " " << CWDB.deltanu(3,1,t) << endl;
				
		}while(t<=tmax);
			
		cout << "Final time : " << t << " s" << endl << endl;	
		
		OutputFile.close();
					
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestGW: error: " << e.what()<<endl;
		cerr << "TestGW: abort!" << endl;
		exit(1);
	}
	return(0);
}



// end of TestGW.cpp

