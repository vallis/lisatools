// $Id: LISACODE-TDIApply.cpp,v 1.2 2007/02/19 10:07:16 ruiloba Exp $
/*
 *  LISACODE-TDIApply.cpp
 *  LISACode
 *  V 1.4  
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
#include "LISACODE-TDITools.h"
#include "LISACODE-TDI.h"
#include "LISACODE-ConfigSim.h"

using namespace std;



int main (int argc, char * const argv[])
{
	try {
		cout << endl << "   ************************* ";
		cout << endl << "   *                       * ";
		cout << endl << "   *       LISACode        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *      Apply TDI        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *   (" << LISACodeVersion << ")  * ";
		cout << endl << "   *                       * ";
		cout << endl << "   ************************* " << endl << endl; 
		
		
		
		// ***************************************
		// * Lecture du fichier de configuration *
		// ***************************************
		char * ConfigFileName("ConfigTDI");
		if(argc>1)
			ConfigFileName = argv[1];
		
		// *********** HELP *************
		if(strcmp(ConfigFileName,"--help")==0){
			cout << " ----- HELP -----" << endl;
			cout << endl << "\tExecution :" << endl;
			cout << "\t\t(./)TDIApply ConfigFileName " << endl;
			cout << endl << "\tArguments :" << endl;
			cout << endl << "\t\t * ConfigFileName (required)       : Configuration file name. Same file than LISACode configuration file." << endl;
			cout << endl << "\tNOTE :" << endl;
			cout << endl << "\t\t Add the moment, only for ASCII file !" << endl;
			cout <<  "\t\t There are more details in the user's manual." << endl;
			cout << endl << " ----------------" << endl;
			return 0;
			
		}
		// *********** Version *************
		if(strcmp(ConfigFileName,"--version")==0){
			cout << " ----- VERSION -----" << endl;
			cout << " TDIApply : executable of LISACode package - version " << LISACodeVersion << endl;
			cout << " ----------------" << endl;
			return 0;
		}
		
		// *********** Configuration file *************
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
		//double TDIGood(1.0e-20);
		//Ecart entre les retards reels et les retards appliques daans TDI (en s)
		//double DeltaTDIDelay(Config.gettDeltaTDIDelay());
		// Ecart entre l'instant de lecture des donnees et l'instant d'application de TDI
		double tTDIShift(2.0*MAX(0.0,Config.tMemNecInterpTDI()-Config.tMinDelay()));
		// Time of storage for TDI delays and Eta
		double tMemPDPM(tTDIShift + Config.tMemNecInterpTDI() + Config.tMaxDelay());
		double tMemTDI(tTDIShift + Config.tMemNecInterpTDI() + Config.getNbMaxDelays()*Config.tMaxDelay());
		
		cout << "    Parameters --> OK !" << endl << endl;
		
		//Declaration des memoires utilises
		cout << "  - Memorys..." << endl;
		vector<Memory *> RecordPDPM;
		for(int iSC=1; iSC<=3; iSC++)
			RecordPDPM.push_back(new MemoryReadDisk(tMemPDPM, tStepMes, Config.getFileNameSig(iSC)));
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
		TDI_InterData Eta(DelayTDI, & RecordPDPM, tMemTDI, tTDIShift/2.0, Config.getNoNoise(), Config.getTDIInterp(), Config.getTDIInterpUtilVal());
		// Acceleration module of TDI
		TDITools TDIQuickMod(DelayTDI, Config.getTDIDelayApprox());
		// Creation of generators
		vector<TDI> TDIGens;
		int NbGenTDI (Config.NbGenTDI());
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " ...";
			TDIGens.push_back(TDI(DelayTDI, & Eta, & RecordTDI, Config.getFileEncodingTDI(), iGen, Config.getGenTDIPacks(iGen), & TDIQuickMod));
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " --> OK" << endl;
		}		
		// Ecart entre l'instant de lecture des donnees et l'instant d'application de TDI
		cout << "    TDI --> OK !" << endl << endl;
		
		cout << "Creation --> OK" << endl << endl;
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		
		ifstream FileHead;
		char Buf[256] ;
		// Copy of configuration file in header
		FileHead.open(ConfigFileName);
		if (!FileHead){
			throw invalid_argument("Main : Can not open the configuration file ! ");
		}
		RecordTDI << "############## " << LISACodeVersion <<  " ##############" << endl;
		RecordTDI << "############## Configuration : " << ConfigFileName <<  " ##############" << endl;
		while(!FileHead.eof()) {
			FileHead.getline(Buf, 256);
			RecordTDI << "# " << Buf << endl;
		};
		RecordTDI << "############## End of configuration ##############" << endl;
		FileHead.close();
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
		cout << endl << "Runnning in progress : receive datas (-> " << tMemTDI+tTDIShift << " s)..." << endl;
		t = -1.0*(tMemTDI+tTDIShift);
		do{
			//Display time
			//cout << "* t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				//cout << t << " s" << endl;
				printf("%.0lf s     #0%03.0f %% \n", t, 100*t/tMax);
				fflush(stdout);
				ttmpAff = 0.0;
			}
			
			for(int iSC=0; iSC<RecordPDPM.size(); iSC++)
				RecordPDPM[iSC]->RecordAccData(tStepMes, t);
			DelayTDI->RecordAccData(tStepMes, t);
			
			
			
			t += tStepMes;
			tSinceFirstReception += tStepMes;
			
			
			ttmpAff += tStepMes;
			
			
			
		//}while(t<=tMemTDI+tTDIShift);
		}while(t<tTDIShift);
		
		
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
			//cout << "** t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				//cout << t << " s" << endl;
				printf("%.0lf s     #0%03.0f %% \n", t, 100*t/tMax);
				fflush(stdout);
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
			RecordTDI.precision(10);
			RecordTDI << t-tTDIShift;
			if(TDIQuickMod.getRapidOption())
				TDIQuickMod.RefreshDelay(tTDIShift);
			for(int iGen=0; iGen<NbGenTDI; iGen++){
				ResultTDI[iGen] = TDIGens[iGen].RecordAndReturnResult(tTDIShift);
				AbsMeanTDI[iGen] += fabs(ResultTDI[iGen]); 
			}
			RecordTDI << endl;
			
			t += tStepMes;
			//cout << "**************" << endl;
			//cout << t << " s " << endl;
			//cout << "**************" << endl;
			
			//Display time
			ttmpAff += tStepMes;
			
		}while(t<=tMax+tTDIShift);
		
		
		// Read TDI result
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			AbsMeanTDI[iGen] /= NbTDIApp*1.0;
			cout << " " << Config.getNameGenTDI(iGen) << " = " << AbsMeanTDI[iGen];
			/*cout << " -- > mag = " << log10(1.0e-13/fabs(AbsMeanTDI[iGen]));
			if(fabs(AbsMeanTDI[iGen]) < TDIGood)
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " successful !";
			else
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " not good !"; */
			cout << endl;
		}
		
		//RecordEta.close();
		
		cout << "Final time : " << t << " s" << endl;
		cout << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TDIApply: error: " << e.what()<<endl;
		cerr << "TDIApply: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-TDIApply.cpp

