// $Id: LISACODE-TestGW.cpp,v 1.2 2007/02/19 10:00:28 ruiloba Exp $
/*
 *  TestGW.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 13/05/05 by Antoine PETITEAU (APC)
 *  Last modification on 20/02/06 by Antoine PETITEAU (APC)
 *
 */

#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWFile.h"
#include "LISACODE-GWBinary.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-GWSto.h"
using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *************************************";
		cout << endl << "   *                                   *";
		cout << endl << "   *   Test une onde gravitationnelle  *";
		cout << endl << "   *   ------------------------------  *";
		cout << endl << "   *         ("<<LCVersion<<")         *";
		cout << endl << "   *                                   *";
		cout << endl << "   **************************************" << endl << endl; 
		
		
		
		// **************************
		// * Debut des declarations *
		// **************************
		cout << "Creation of parameters ..." << endl;
		// Declaration of variables which control the simulation
		double tStep(50.0), tmax(366*24*3600.0);//tmax(pow(2,25)+tStep);
			double StepdDisplay(24*3600.0);
			// Declaration of temporary variables
			double t(0.0), ttmpAff(0.0);
			// Gravitational waves parameters
			double FreqGW(1.0e-3), AmplGWhp(1.0e-21), AmplGWhc(0.0e-21), AnglPolGW(MathUtils::deg2rad(0.0));
			double BetaGW(MathUtils::deg2rad(20.0)), LambdaGW(MathUtils::deg2rad(240.0)); 
			// Name of file where the result are stored
			char * FileName;
			FileName = "GWTest.txt";
			
			cout << endl << "Creation of items ..." << endl;
			// Creation of gravitational wave
			//GWPeriGate m_GW(BetaGW, LambdaGW, AnglPolGW, FreqGW, AmplGWhp, AmplGWhc);
			//GWBinary m_GW; //AMCVn
			//GWSto m_GW(BetaGW, LambdaGW, tStep, -3.0,  1.0/(2.0*tStep), 1.0/tmax, 1.0, 1.0);
			GWSto m_GW(BetaGW, LambdaGW, tStep, 100.0*tStep, 100.0*tStep, -100.0*tStep,  -1000.0,  tStep, -3.0,  1.0/(2.0*tStep), 1.0/tmax, 100000, 1.0, 1.0);
			
			//Declaration of used memory
			ofstream Record_GW;
			Record_GW.open(FileName); 
			
			
			cout << "  Creation --> OK" << endl << endl;
			
			
			
			
			// *******************
			// * Initialisations *
			// *******************
			cout << "Initialisations ..." << endl;
			Record_GW << "#Time hp hx" << endl;
			
			cout << "  Initialisations --> OK" << endl << endl;
			
			
			// **************************************
			// * Affichage des parametres de l'onde *
			// **************************************
			
			cout << "Gravitational wave :" << endl;
			cout << "--------------------" << endl;
			//cout << "  + Frequency = " << m_GW.getFreq() << " Hz" << endl;
			//cout << "  + Amplitude of component +   = " << m_GW.getAmplhp() << endl;
			//cout << "  + Amplitude of component x   = " << m_GW.getAmplhc() << endl;
			cout << "  + Angle of polarisation (proj{x},i) = " << m_GW.getAnglPol() << " rad" << endl;
			cout << "  + Direction : - Lambda =  " << m_GW.getLambda() << " rad" << endl;
			cout << "                - Beta   =  " << m_GW.getBeta() << " rad" << endl;
			
			vector<double> tmpDir;
			tmpDir = m_GW.getDirProp();
			tmpDir[0] = -0.86657;
			tmpDir[1] = -1.62802e-16;
			tmpDir[2] = 0.49905;
			
			cout << " Direction unit vector : - x =  " << tmpDir[0] << endl;
			cout << "                         - y =  " << tmpDir[1] << endl;
			cout << "                         - z =  " << tmpDir[2] << endl;
			m_GW.setDirProp(tmpDir);
			cout << " Recompute angle : - Lambda = " << m_GW.getLambda()  << " rad" << endl; 
			cout << "                   - Beta   = " << m_GW.getBeta()  << " rad" << endl; 
			cout << endl;
			
			
			// *************
			// * Execution *
			// *************
			
			cout << "Running in progress (->"<< tmax << "s) ..." << endl;
			do{
				t += tStep;
				
				//Display and record
				ttmpAff += tStep;
				if(ttmpAff >= StepdDisplay){
					cout << t/(24.0*3600.0) << " days" << endl;
					ttmpAff = 0.0;
				}
				
				// Record gravitationnal wave
				Record_GW << t << " " << m_GW.hp(t) << " " << m_GW.hc(t) << endl;
				//cout << t << " " << m_GW.hp(t) << " " << m_GW.hc(t) << endl;
				
			}while(t<=tmax);
			
			cout << "Final time : " << t << " s" << endl << endl;	
			
			Record_GW.close();
			
			
			return(0);
			
			// Read stored values for a new gravitationnal wave
			cout << "Reading of gravitational wave in progress ... " << endl;
			GWFile GWRead(BetaGW, LambdaGW, AnglPolGW, FileName);
			cout << " Reading --> OK" << endl << endl;
			
			// Get value for many times
			ofstream OutGW;
			OutGW.open("GWRead.txt");
			OutGW << "Time hp hx" << endl;
			t = tStep;
			tStep = tStep/2.0;
			cout << "Running in progress (->"<< tmax << "s) ..." << endl;
			do{
				t += tStep;
				
				//Display and record
				ttmpAff += tStep;
				if(ttmpAff >= StepdDisplay){
					cout << t/(24.0*3600.0) << " days" << endl;
					ttmpAff = 0.0;
				}
				
				// Record gravitationnal wave
				OutGW << t << " " << GWRead.hp(t) << " " << GWRead.hc(t) << endl;
				
			}while(t<=tmax);
			
			
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

