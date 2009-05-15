// $Id: LISACODE-TDIApply.cpp,v 1.2 2007/02/19 10:07:16 ruiloba Exp $
/*
 *  LISACODE-TDIApply.cpp
 *  LISACode
 *  V 1.4  
 *
 *
 * Description :
 * ----------
 * This is the main file for TDIApply application. 
 * TDIApply apply TDI generators on read data and delays in files.
 *
 *
 *  Created on 02/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 04/07/08 by Antoine PETITEAU (AEI)
 *
 */

#include <stdexcept>
#include <iostream>
#include <fstream.h>
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
		
		// *********** HELP *************
		if(((argc>1)&&(strcmp(argv[1],"--help")==0))||((argc>1)&&(strcmp(argv[1],"-h")==0))){
			cout << " ----- HELP -----" << endl;
			cout << endl << "\tExecution :" << endl;
			cout << "\t\t(./)TDIApply [Options] ConfigFileName " << endl;
			cout << endl << "\tArguments :" << endl;
			cout << endl << "\t\t * ConfigFileName (required) : Configuration file name. " << endl;
			cout << endl << "\tOptions :" << endl;
			cout << "\t\t * -h : Write header in binary files."  << endl ;
			cout << endl << "\tNOTE :" << endl;
			cout <<  "\t\t There are more details in the user's manual." << endl;
			cout << endl << " ----------------" << endl;
			return 0;
			
		}
		// *********** Version *************
		if(strcmp(argv[1],"--version")==0){
			cout << " ----- VERSION -----" << endl;
			cout << " TDIApply : executable of LISACode package - version " << LCVersion << " at " << DateOfLastUpdate << endl;
			cout << " ----------------" << endl;
			return 0;
		}
		
		cout << endl << "   ************************* ";
		cout << endl << "   *                       * ";
		cout << endl << "   *       LISACode        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *      Apply TDI        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *   (LISACode v "<<LCVersion<<")   * ";
		cout << endl << "   *                       * ";
		cout << endl << "   ************************* " << endl << endl; 
		
		
		
		// ***************************************
		// * Lecture du fichier de configuration *
		// ***************************************
		char * ConfigFileName("ConfigTDI");
		int nOption(0);
		bool BinHeader(false);
		
		
		// *********** Options *************
		for(int iarg=1; iarg<argc; iarg++){
			if((argc>1)&&(strcmp(argv[iarg],"-h")==0)){
				BinHeader = true;
				cout << "Options : Header in biary files" << endl;
				nOption++;
			}
		}
		
		if(argc>1+nOption)
			ConfigFileName = argv[1+nOption];
		
		
		// *********** Configuration file *************
		cout << endl << "Read configuration (" << ConfigFileName << ")..." << endl;
		ConfigSim Config(ConfigFileName);
		cout << "Read configuration (" << ConfigFileName << ") --> OK !" << endl;
		cout << endl;
		
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
		// * Offset between real delays and delays used in TDI (in seconds)
		double DeltaTDIDelay(Config.gettDeltaTDIDelay());
		double tTDIShift(Config.gettTDIShift());
		double tMemPDPM(Config.gettMemPhasemeters());
		double tMemTDI(Config.gettMemTDI());
		
		// For time display 
		time_t tstart, tcur;
		float EstimTStop;
		int hh, mm;
		float ss;
		int NDatPh1(0), NDatPh2(0);
		double ttmp;
		
		cout << "    Parameters --> OK !" << endl << endl;
		
		// *** Declaration of used memories
		cout << "  - Memorys..." << endl;
		vector<Memory *> RecordPDPM;
		for(int iSC=1; iSC<=3; iSC++){
			if(Config.UseInternalPhasemeter())
				RecordPDPM.push_back(new MemoryReadDisk(tMemPDPM, tStepMes, Config.getFileNameSig(iSC), Config.getFileEncodingSig(iSC), 5, tMax+tMemPDPM+tMemTDI+2));
			else
				RecordPDPM.push_back(new MemoryReadDisk(tMemPDPM, tStepMes, Config.getFileNameSig(iSC), Config.getFileEncodingSig(iSC), 3, tMax+tMemPDPM+tMemTDI+2));
		}
		ofstream RecordTDI;
		cout << "     Record TDI        : DISK (" << Config.getFileNameTDI() << ")"; 
		if(Config.getFileEncodingTDI() > 0){
			RecordTDI.open(Config.getFileNameTDI(),ios::binary);
			cout << " --> BINARY" << endl;
		}else{
			RecordTDI.open(Config.getFileNameTDI());
			cout << " --> ASCII" << endl;
		}
		Memory * DelayTDI;
		DelayTDI = new MemoryReadDisk(tMemTDI, tStepMes, Config.getFileNameDelays(), Config.getFileEncodingDelays(), 7, tMax+tMemPDPM+tMemTDI+tTDIShift);
		cout << "End of memory creation" << endl; fflush(stdout);
		
		// * Create data series
		for(int iSC=1; iSC<4; iSC++){
			RecordPDPM[iSC-1]->AddSerieData(0, "s" , 0, iSC);
			RecordPDPM[iSC-1]->AddSerieData(1, "s" , 1, iSC);
			if(Config.UseInternalPhasemeter()){
				RecordPDPM[iSC-1]->AddSerieData(2, "tau" , 0, iSC);
				RecordPDPM[iSC-1]->AddSerieData(3, "tau" , 1, iSC);
			}
		}
		cout << "End of creation of data series" << endl; fflush(stdout);
		for (int IndirectDir=0 ; IndirectDir<2; IndirectDir++){
			for(int iSC=1; iSC<4; iSC++){
				DelayTDI->AddSerieData(iSC+3*IndirectDir-1, "D" , IndirectDir, iSC);
			}
		}
		
		cout << "    Memorys  --> OK !" << endl;
		
		// *** Declaration TDI generators
		cout << "  - TDI... " << endl;
		// ** Creation of Eta signals
		TDI_InterData Eta(DelayTDI, & RecordPDPM, tMemTDI, tTDIShift/2.0, Config.getNoNoise(), Config.getTDIInterp(), Config.getTDIInterpUtilVal());
		// ** Acceleration module of TDI
		TDITools TDIQuickMod(DelayTDI, Config.getTDIDelayApprox());
		// ** Creation of generators
		vector<TDI> TDIGens;
		int NbGenTDI (Config.NbGenTDI());
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " ...";
			TDIGens.push_back(TDI(DelayTDI, & Eta, & RecordTDI, Config.getFileEncodingTDI(), iGen, Config.getGenTDIPacks(iGen), Config.getGenTDIPacksFact(iGen), & TDIQuickMod));
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " --> OK" << endl;
		}		
		// Ecart entre l'instant de lecture des donnees et l'instant d'application de TDI
		cout << "    TDI --> OK !" << endl << endl;
		
		cout << "Creation --> OK" << endl << endl;
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		
		if(Config.getFileEncodingTDI() == 0){
			ifstream FileHead;
			char Buf[256] ;
			// Copy of configuration file in header
			FileHead.open(ConfigFileName);
			if (!FileHead){
				throw invalid_argument("Main : Can not open the configuration file ! ");
			}
			RecordTDI << "############## LISACode v " << LCVersion << "-" << DateOfLastUpdate << " at " << MathUtils::TimeISO8601() << " ##############" << endl;
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
		}else{
			if(BinHeader){
				RecordTDI << "#TITLE t";
				for(int i=0; i<NbGenTDI; i++)
					RecordTDI << "," << Config.getNameGenTDI(i);
				RecordTDI << endl; 
				RecordTDI << "#RECORD " << NbGenTDI+1<< " " << (int)ceil((tMax)/tStepMes+1) <<  endl;
				RecordTDI << "#TIME 0.0 " << tStepMes << " " << tMax+tTDIShift << endl;
			}
		}
		
		//RecordEta.open("EtaSignal.txt");
		//RecordEta << "Time Eta1 Eta2 Eta3 Etap1 Etap2 Etap3 Eta3-Etap2" << endl;
		
		cout << "  Initialisations --> OK" << endl << endl;
		
		
		cout << "Time of storage for TDI delays and Eta = " << tMemTDI << endl;
		cout << "Time of storage for raw data = " << tTDIShift+Config.tMaxDelay()+Config.tMemNecInterpTDI() << endl;
		cout << "Shift time for TDI = " << tTDIShift << endl;
		
		// *************
		// * Execution *
		// *************
		
		// Phase 1 : 
		// ---------
		//  Les faisceaux sont arrives sur les photodiodes. On fonctionne maintenant en flux
		//  continu. Les donnees recus sur les photodiodes sont temporairement stockes en vue 
		//  d'une utilisation dans TDI. Le nombre de donnees qu'il est necessaire d'enregistrer
		//  avant de lancer TDI est choisi par rapport au retard maximal present dans les generateurs.
		cout << endl << "Runnning in progress : receive datas (-> " << tMemPDPM << " s)..." << endl;
		t = Config.gettStartPhasemeters();
		//cout << t << " " << tMemTDI << " " << tTDIShift << endl;
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
			
			NDatPh1++;
			
			//}while(t<=tMemTDI+tTDIShift);
		}while(t<tTDIShift-tMemTDI);
		
		// Phase 2 : 
		// ---------
		// Il y a assez de donnees pour calculer les Eta mais pas encore pour appliquer TDI.
		cout << endl << "Runnning in progress : enoughe data for pre-computing of TDI (-> " << tMemTDI << " s)..." << endl;
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
			
			Eta.ComputeEta();
			
			t += tStepMes;
			tSinceFirstReception += tStepMes;
			
			
			ttmpAff += tStepMes;
			
			NDatPh1++;
			
			//}while(t<=tMemTDI+tTDIShift);
		}while(t<tTDIShift);
		
		
		// Phase 3 : 
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
		time(&tstart);
		do{
			//cout << "** t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				time(&tcur);
				//cout << t << " s" << endl;
				EstimTStop = (tcur-tstart)*(tMax-t)/t;
				hh = (int)(floor(EstimTStop/3600.0));
				mm = (int)((EstimTStop - 3600.0*hh)/60);
				ss = EstimTStop - 60*(mm+60*hh);
				//cout << t << " s" << endl;
				printf("%.0lf s    (remaining time : %02d:%02d:%02.0f) #0%03.0f %% \n", t, hh, mm, ss, 100*t/tMax);
				fflush(stdout);
				ttmpAff = 0.0;
			}		 
			
			for(int iSC=0; iSC<RecordPDPM.size(); iSC++){
				//cout << "SC" << iSC; fflush(stdout);
				RecordPDPM[iSC]->RecordAccData(tStepMes, t);
			}
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
			ttmp = t-tTDIShift;
			if(Config.getFileEncodingTDI() == 0) // If TDIFile is an ASCII file, write time in ASCII
				RecordTDI << ttmp;
			else // Else write time in Binary
				RecordTDI.write((char*) &ttmp, sizeof(double));
			if(TDIQuickMod.getRapidOption())
				TDIQuickMod.RefreshDelay(tTDIShift);
			for(int iGen=0; iGen<NbGenTDI; iGen++){
				//cout << tTDIShift << endl;
				ResultTDI[iGen] = TDIGens[iGen].RecordAndReturnResult(tTDIShift);
				AbsMeanTDI[iGen] += fabs(ResultTDI[iGen]); 
			}
			if(Config.getFileEncodingTDI() == 0) // If TDIFile is an ASCII file
				RecordTDI << endl;
			
			t += tStepMes;
			//cout << "**************" << endl;
			//cout << t << " s " << endl;
			//cout << "**************" << endl;
			
			//Display time
			ttmpAff += tStepMes;
			
			NDatPh2++;
			
		}while(t<=tMax+tTDIShift);
		
		// Read TDI result
		cout.precision(15);
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
		
		
		cout << "Final time : " << t << " s" << endl << endl;
		
		
		//cout << "Header for binary file TDI :" << endl;
		//cout << "#TITLE TDIDataTitle" << endl; 
		//cout << "#RECORD " << NbGenTDI+1<< " " << NDatPh2 << endl;
		//cout << "#TIME 0.0 " << tStepMes << " " << t-tStepMes << endl;
		
		for(int iSC=1; iSC<=3; iSC++)
			delete RecordPDPM[iSC-1];
		delete DelayTDI;
		RecordTDI.close();
		
		cout << endl << "End" << endl;
		
		
		
	}
	
	
	catch(exception & e ) {
		cerr << "TDIApply: error: " << e.what()<<endl;
		cerr << "TDIApply: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-TDIApply.cpp

