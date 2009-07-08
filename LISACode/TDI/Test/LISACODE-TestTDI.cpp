// $Id: LISACODE-TestTDI.cpp,v 1.2 2007/03/29 15:17:29 lalanne Exp $
/*
 *  LISACODE-TestTDI.cpp
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Ce fichier principal contient le main d'application de TDI sur des donnees enregistrees.
 *
 *
 *  Created on 02/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 02/11/05 by Antoine PETITEAU (APC)
 *
 */

#include <stdexcept>
#include <iostream>
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Memory.h"
#include "LISACODE-MemoryWriteDisk.h"
#include "LISACODE-MemoryReadDisk.h"
#include "LISACODE-TDI_InterData.h"
#include "LISACODE-TDI.h"
#include "LISACODE-ConfigSim.h"

using namespace std;



int main (int argc, char * const argv[])
{
	try {
		Tools MT;
		
		cout << endl << "   *********************";
		cout << endl << "   *                   *";
		cout << endl << "   *     Test TDI      *";
		cout << endl << "   *     --------      *";
		cout << endl << "   * (LISACode v "<<LCVersion<<") *";
		cout << endl << "   *                   *";
		cout << endl << "   *********************" << endl << endl; 
		
		
		
		// ***************************************
		// * Lecture du fichier de configuration *
		// ***************************************
		char * ConfigFileName("ConfigTDI");
		cout << endl << "Read configuration (" << ConfigFileName << ")..." << endl;
		ConfigSim Config(ConfigFileName);
		
		
		// ****************
		// * Declarations *
		// ****************
		cout << endl << "Creation ..." << endl;
		
		cout << "  - Parameters..." << endl;
		//Declaration des variables qui parametrisent la Simulation
		double tStepMes(Config.gettStepMes());
		double tMax(Config.gettMax());
		double DisplayStep(Config.gettDisplay());
		//Declaration des variables temporaires
		double t(0.0), ttmpAff(0.0), tSinceFirstReception(0.0);
		// Critere sur la moyenne d'un generateur TDI pour qu'il soit declare efficace
		double TDIGood(1.0e-20);
		//Ecart entre les retards reels et les retards appliques daans TDI (en s)
		//double DeltaTDIDelay(Config.gettDeltaTDIDelay());
		// Ecart entre l'instant de lecture des donnees et l'instant d'application de TDI
		double tTDIShift(2.0*MAX(0.0,Config.tMemNecInterpTDI()-Config.tMinDelay()));
		// Time of storage for TDI delays and Eta
		double tMemTDI(tTDIShift/2.0+Config.tMemNecInterpTDI()+Config.getNbMaxDelays()*Config.tMaxDelay());
		
		cout << "    Parameters --> OK !" << endl << endl;
		
		//Declaration des memoires utilises
		cout << "  - Memorys..." << endl;
		vector<Memory *> RecordPDPM;
		for(int iSC=1; iSC<=3; iSC++)
			RecordPDPM.push_back(new MemoryReadDisk(tTDIShift+Config.tMaxDelay()+Config.tMemNecInterpTDI(), tStepMes,
													 Config.getFileNameSig(iSC)));
		ofstream RecordTDI;
		RecordTDI.open(Config.getFileNameTDI());
		Memory * DelayTDI;
		DelayTDI = new MemoryReadDisk(tMemTDI, tStepMes, Config.getFileNameDelays());
		
		// Create data series
		for(int iSC=1; iSC<4; iSC++){
			RecordPDPM[iSC-1]->AddSerieData(0, "s" , 0, iSC);
			RecordPDPM[iSC-1]->AddSerieData(1, "s" , 1, iSC);
			RecordPDPM[iSC-1]->AddSerieData(2, "tau" , 0, iSC);
			RecordPDPM[iSC-1]->AddSerieData(3, "tau" , 1, iSC);
		}
		for (int IndirectDir=0 ; IndirectDir<2; IndirectDir++){
			for(int iSC=1; iSC<4; iSC++){
				DelayTDI->AddSerieData(iSC+3*IndirectDir-1, "D" , IndirectDir, iSC);
			}
		}
		tMax = RecordPDPM[1]->gettMax();
		
		cout << "    Memorys  --> OK !" << endl;
		
		//Declaration des generateurs TDI
		cout << "  - TDI... " << endl;
		// Creation of Eta signals
		TDI_InterData Eta(DelayTDI, & RecordPDPM, tMemTDI, tTDIShift/2.0, false, Config.getTDIInterp(), Config.getTDIInterpVal());
		// Acceleration module of TDI
		TDITools TDIQuickMod(DelayTDI, false);
		// Creation of generators
		vector<TDI> TDIGens;
		int NbGenTDI (Config.NbGenTDI());
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " ...";
			TDIGens.push_back(TDI(&MT, DelayTDI, & Eta, & RecordTDI, 0, iGen, Config.getGenTDIPacks(iGen), & TDIQuickMod));
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " --> OK" << endl;
		}
		// Ecart entre l'instant de lecture des donnees et l'instant d'application de TDI
		cout << "    TDI --> OK !" << endl << endl;
		
		cout << "Creation --> OK" << endl << endl;
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		
		RecordTDI << "#Time";
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			RecordTDI << " " << Config.getNameGenTDI(iGen);
		}
		RecordTDI << endl;
		
		//RecordEta.open("EtaSignal.txt");
		//RecordEta << "Time Eta1 Eta2 Eta3 Etap1 Etap2 Etap3 Eta3-Etap2" << endl;
		
		cout << "  Initialisations --> OK" << endl << endl;
		
		// *************
		// * Execution *
		// *************
				
		// Phase 1 : 
		// ---------
		//  Les faisceaux sont arrives sur les photodiodes. On fonctionne maintenant en flux
		//  continu. Les donnees recus sur les photodiodes sont temporairement stockes en vue 
		//  d'une utilisation dans TDI. Le nombre de donnees qu'il est necessaire d'enregistrer
		//  avant de lancer TDI est choisi par rapport au retard maximal present dans les generateurs.
		cout << endl << "Runnning in progress : receive datas and phasemeter stabilization (->" << tMemTDI << "s) ..." << endl;
		do{
			t += tStepMes;
			tSinceFirstReception += tStepMes;
			
			//Display time
			ttmpAff += tStepMes;
			if(ttmpAff >= DisplayStep){
				cout << t << " s" << endl;
				ttmpAff = 0.0;
			}
			
			for(int iSC=0; iSC<RecordPDPM.size(); iSC++)
				RecordPDPM[iSC]->RecordAccData(tStepMes, t);
			DelayTDI->RecordAccData(tStepMes, t);
			
		}while(t<tMemTDI+tTDIShift);
		
		
		// Phase 2 : 
		// ---------
		//  On a maintenant assez de donnees pour appliquer TDI. On calcule donc les resultats
		//  des generateurs et on les enregistre dans un fichier.
		cout << endl << "Runnning in progress : Enough recorded datas ..." << endl;
		cout << "   --> Compute TDI directly (->" << tMax << "s) ..." << endl;
		vector<double> ResultTDI;
		vector<double> AbsMeanTDI;
		ResultTDI.resize(Config.NbGenTDI());
		AbsMeanTDI.resize(Config.NbGenTDI());	 
		int NbTDIApp(0);
		do{
			t += tStepMes;
			//cout << "**************" << endl;
			//cout << t << " s " << endl;
			//cout << "**************" << endl;
			
			//Display time
			ttmpAff += tStepMes;
			if(ttmpAff >= DisplayStep){
				cout << t << " s" << endl;
				ttmpAff = 0.0;
			}		 
			
			for(int iSC=0; iSC<RecordPDPM.size(); iSC++)
				RecordPDPM[iSC]->RecordAccData(tStepMes, t);
			DelayTDI->RecordAccData(tStepMes, t);
			
			//Compute TDI
			NbTDIApp++;
			
			Eta.ComputeEta();
			//Record Eta signal
			//RecordEta.precision(15);
			//RecordEta << t ;
			//for(int i=1; i<7; i++)
			//	RecordEta << " " << Eta.gData(i, 0.0);
			//RecordEta << endl;
			
			/*for(int nt=-99; nt<=0; nt++){
				Eta.ComputeEta(10.0-0.01*nt);
			//Record Eta signal
			RecordEta.precision(15);
			RecordEta << t-(10.0-0.01*nt) ;
			for(int i=1; i<7; i++)
				RecordEta << " " << Eta.gData(i, 0.0);
			RecordEta << " " << (Eta.gData(3, 0.0)-Eta.gData(5, 0.0));
			RecordEta << endl;
			}
			*/
			
			// Compute and record TDI generators
			RecordTDI << t-tTDIShift;
			if(TDIQuickMod.getRapidOption())
				TDIQuickMod.RefreshDelay(tTDIShift);
			for(int iGen=0; iGen<NbGenTDI; iGen++){
				//cout << tTDIShift << endl;
				ResultTDI[iGen] = TDIGens[iGen].RecordAndReturnResult(tTDIShift);
				AbsMeanTDI[iGen] += fabs(ResultTDI[iGen]); 
			}
			RecordTDI << endl;
			
		}while(t<tMax);
		
		
		// Read TDI result
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			AbsMeanTDI[iGen] /= NbTDIApp*1.0;
			cout << " " << Config.getNameGenTDI(iGen) << " = " << AbsMeanTDI[iGen];
			cout << " -- > mag = " << log10(1.0e-13/fabs(AbsMeanTDI[iGen]));
			if(fabs(AbsMeanTDI[iGen]) < TDIGood)
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " successful !" << endl;
			else
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " not good !" << endl;
		}
		
		
		//RecordEta.close();
		
		cout << "Final time : " << t << " s" << endl;
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestTDI: error: " << e.what()<<endl;
		cerr << "TestTDI: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-TestTDI.cpp

