// $Id: LISACODE-TestPhasemeter.cpp,v 1.2 2007/02/19 10:24:46 ruiloba Exp $
/*
 *  LISACODE-TestPhasemeter.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 09/01/06 by  A.Petiteau
 *  Last modification on 30/01/06 by A.Petiteau
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GW.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWFile.h"
#include "LISACODE-GWBinary.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-Noise.h"
#include "LISACODE-NoiseWhite.h"
#include "LISACODE-NoiseFilter.h"
#include "LISACODE-NoiseFile.h"
#include "LISACODE-NoiseFShape.h"
#include "LISACODE-NoiseOof.h"
#include "LISACODE-NoiseTwoFilter.h"
#include "LISACODE-Filter.h"
#include "LISACODE-USOClock.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-Memory.h"
#include "LISACODE-MemoryReadDisk.h"
#include "LISACODE-MemoryWriteDisk.h"
#include "LISACODE-PhoDetPhaMet.h"
using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *********************************";
		cout << endl << "   *                               *";
		cout << endl << "   *   Test the LISA phasemeter    *"; 
		cout << endl << "   *   ------------------------    *";
		cout << endl << "   *      (LISACode v "<<LCVersion<<")        *";
		cout << endl << "   *                               *";
		cout << endl << "   *********************************" << endl << endl; 
		
		
		
		cout << "Creation of parameters ..." << endl;
		// Declaration of variables which control the simulation
		//double tStepPhy(0.1), tStepMes(1.0), tmax(100000.0);
		//double tMemNoiseFirst(5.0), tMemNoiseLast(-20.0);
		double tStepPhy(15.0), tStepMes(15.0), tmax(1000000.0);
		double tMemNoiseFirst(60.0), tMemNoiseLast(-120.0);
		// Declaration of temporary variables
		double t(0.0), ttmp(0.0);
		// Gravitationnal wave parameters
		double FreqGW(1.0e-2), AmplGWhp(1.0e-21), AmplGWhc(0.0e-21), AnglPolGW(MathUtils::deg2rad(0.0));
		double BetaGW(MathUtils::deg2rad(-90.0)), LambdaGW(MathUtils::deg2rad(180.0));
		bool FilterON(true);
		vector<double> FilterParam(4);
		
		FilterParam[0] = 140.0;
		FilterParam[1] = 0.1;
		FilterParam[2] = 0.1;
		FilterParam[3] = 0.3;
		
		// Initialisation du generateur aleatoire
		/*srand(123456);
		cout << (double)rand()/RAND_MAX << endl;
		cout << (double)rand()/RAND_MAX << endl;
		cout << (double)rand()/RAND_MAX << endl;
		cout << (double)rand()/RAND_MAX << endl;
		cout << (double)rand()/RAND_MAX << endl;*/
		
		
		cout << endl << "Creation of items ..." << endl;
		// Creation of the gravitationnals waves
		cout << "  -> GW ..." << endl;
		vector<GW *> m_GWs;
		m_GWs.push_back(new GWMono(BetaGW, LambdaGW, AnglPolGW, FreqGW, AmplGWhp, AmplGWhc)); 
		//m_GWs.push_back(new GWFile(BetaGW, LambdaGW, AnglPolGW, "SinGW.txt"));
		m_GWs.push_back(new GWBinary); //AMCVn
		
		// Creation of the geometry
		cout << "  -> Geometry ..." << endl;
		Geometry SCPos;
		
		// Creation of the transfer fonction
		cout << "  -> Transfer Fonction ..." << endl;
		TrFctGW SigGW(&m_GWs, &SCPos);
		
		// Creation of USO
		cout << "  -> USO ..." << endl;
		USOClock m_USO;
		
		// Creation of Background
		cout << "  -> Background ..." << endl;
		Background * CWDB ;
		CWDB = NULL;
		
		// Creation of Noise
		cout << "  -> Noises ..." << endl;
		vector<Noise *> NoisePointers;
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		/*
		 for(int i=0; i<6; i++){
			 NoisePointers.push_back(NULL);
			 //NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, 1.0e-13));
		 }
		 */
		for(int i=0; i<12; i++){
			NoisePointers.push_back(NULL);
		}
		
		// Declaration of used memory
		cout << "  -> Memory ..." << endl;
		MemoryWriteDisk RecordData(30.0, tStepMes, "OutputTestPhasemeter.txt");
		
		// Declaration of phasemeter
		cout << "  -> Phasemeter ..." << endl;
		PhoDetPhaMet m_PDPM(S, 0, 1, &SCPos, &NoisePointers, &m_USO, &RecordData, &SigGW, CWDB, tStepPhy, tStepMes, FilterON, FilterParam);
		
		cout << "Creation --> OK !" << endl;
		
		//RecordData.MakeTitles();
		
		
		// *************
		// * Execution *
		// *************
		
		cout << "Phasemeter's stabilization ...";
		double tStab(m_PDPM.gettStab());
		while(ttmp<=tStab){
			for(int i=0; i<int(NoisePointers.size()); i++){
				if(NoisePointers[i] != NULL){
					NoisePointers[i]->addNoise();
				}
			}
			//cout << "Pointer size = " << NoisePointers.size() << endl;
			//cout << "***** ttmp = " << ttmp << endl;
			m_PDPM.IntegrateSignal(0.0);
			
			ttmp += tStepMes;
		}
		cout << " --> OK !" << endl << endl;
		
		
		cout << "Running in progress (->"<< tmax << "s) ..." << endl;
		do{
			t += tStepMes;
			for(int i=0; i<int(NoisePointers.size()); i++){
				if(NoisePointers[i] != NULL){
					NoisePointers[i]->addNoise();
				}
			}
			//cout << "***** t = " << t << endl;
			m_PDPM.IntegrateSignal(t);
			RecordData.RecordAccData(tStepMes, m_USO.gTime(t, tStepMes));
			
		}while(t<=tmax);
		
		cout << "Final time : " << t << " s" << endl;	
		
		RecordData.CloseFile();
		
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestPhasemeter: error: " << e.what()<<endl;
		cerr << "TestPhasemeter: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-TestPhasemeter.cpp
