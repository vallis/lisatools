/*
 *  LISACode.cpp
 *  V 1.4
 *  LISACode
 *
 *
 * Description :
 * ----------
 * Ce fichier principal contient le main de lancement du programme. Il gere egalement l'interface avec l'utilisateur
 * du simulateur.
 *
 *
 *  Created on 14/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 06/06/06 by Antoine PETITEAU (APC)
 *
 */
/*!\mainpage Developers' Manual of LISACode 
This document provides a description of the LISACode software for future developpers. Some information about the code execution (\ref params) are also provided to users. 
This manual is divided in three sections:
- \ref intro 
- \ref codeDesc 
- \ref params

*/

/*!\page intro Introduction

LISACode[<a href="Bibliography.html#LISACode">Petiteau &nbsp; 2008</a>] is a LISA mission simulator. It is highly structured and programmed in C++. The simulator has the purpose to bridge the gap between the basic principles of LISA and a sophisticated end-to-end engineering level simulator.  This software package, which runs on most computer platforms, can be downloaded from the Lisa-France web site (http://www.apc.univ-paris7.fr/LISA-France/analyse.phtml).

\section techDesc LISACode technical description

LISACode simulates the LISA gravitational wave (GW) detector (see http://www.esa.int/esaSC/SEMEJRR1VED_index_0.html and [<a href="Bibliography.html#LISAPPA">Bender &nbsp; 1998</a>]). It does not aim at simulating the LISA detector in detail but rather it uses the response function of its main components, particularly because they will affect the noise level of the detector response. It also includes an implementation of the TDI (Time Delay Interferometry, [<a href="Bibliography.html#TDIRevue">Tinto &nbsp; 2004</a>] [<a href="Bibliography.html#TDIVinet">Dhurandhar &nbsp; 2002</a>]
) technique which allows to suppress the noise introduced by lasers frequency instability. 

The main inputs and outputs of LISACode are time-dependent sequences. Input sequences describe the GW strain and output sequences describe the phasemeters response or their treatment via various TDI combination.

A number of elementary GW signals can be defined, but the main aim of the code is to be used in conjunction with more sophisticated GW simulators via intermediate data files.

*/



/*!\page codeDesc A description of the Code
\section org Code organisation 
LISACode is written in C++ and has a very modular structure. The main structure of LISACode is shown in the figure below. This structure maps the main components of the LISA detector as well as its physical inputs (see details in [<a href="Bibliography.html#LISACode">Petiteau &nbsp; 2008</a>][<a href="Bibliography.html#LISACodeLISASymp6">Petiteau &nbsp; 2006</a>]).
Its main components are: 
\arg a variety of GW inputs, 
\arg a detailed description of the orbits [<a href="Bibliography.html#OrbitLISA">Dhurandhar &nbsp; 2004</a>] of the three satellites (including the breathing and rotation
modes of the LISA triangle), 
\arg the different noise sources (lasers, DFS and the
interferometric measurements),
\arg the phasemetre measurements and 
\arg the Ultra Stable Oscillator (USO) clock performances.

\image html New_Structure_Anglais.gif
\image latex New_Structure_Anglais.eps

The next figure shows the organisation of the libraries used by LISACode. The green boxes represent objects, the red boxes the libraries or modules (see  Modules) and the pink boxes the executables.

\image html droppedImage-2_-_petit.jpg
\image latex droppedImage-2_-_petit.eps

\section phyConst Physical constants
Constants used by the LISA simulator are described in next files:

\ref LISACODE-LISAConstants.h : Physical constants of LISA instrument.\n
\ref LISACODE-PhysicConstants.h : Physical constants, reference values and unit conversions.


\section detIO Details of LISACode simulator input/outputs

The first input is the GW itself. It can be defined internally through some simplified models which produce signals of constant frequency. The GW may also be input via a time sequence as well as produced by more sophisticated simulator codes. An example of this is given below. \n

The orbits of LISA are generated internally by the simulator. They correspond to realistic orbits that contain both the breathing and rotation modes of Lisa as a function of its rotation around the sun (see [<a href="Bibliography.html#OrbitLISA">Dhurandhar &nbsp; 2004</a>]). The parameters of these orbits can be adjusted to modify the average distance between the satellites (nominally \f$5 10^9 m\f$) or be defined in such a way  to keep LISA at a fixed given location. The initial position on the orbits can also be defined in inputs. \n
An important element of the LISA response and hence of the code are the different nature of noise inputs. This include the optical noise due to shot noise and related factors as described in table 4.1 of [<a href="Bibliography.html#TDIRevue">Tinto&nbsp;2004</a>].The inertial mass and the laser noises can also be defined at input. Normally, these noises are defined as bandwidth limited white noise but different shapes of noise can be used.\n
Using the orbits, the response of LISA to the GW will be calculated and a relative frequency fluctuation  will be input to the Phasemeter Module. These will be combined with the different noise contribution to produce the primary phasemeter output signal which then will be processed through a Butterworth filtering module. In standard operation, the primary signal will be produced at a 10 Hz signal and outputted, after filtering, at a 1 Hz rate.

These signals can be saved on disk files and/or processed by a TDI module using a variety of TDI combinations that are defined in input.

The above description of the code provides only a brief summary of its capabilities. The \ref params  will provide a (non-exhaustive) list of the parameters that can be used to control the input, the processing and the output of the code and will therefore give a more complete idea of its possibilities.

*/

/*!\page params LISACode parameters
As the use of the different parameters may be complicated, the reader may again refer to section IV of http://www.apc.univ-paris7.fr/LISA-France/analyse.phtml where examples of various input and output are given.

The following figures give the different reference frames that are used in the LISACode.
\image html  figure_psi-last.gif
\image latex figure_psi-last.eps width=10cm 
In an input file, you may include your own comments. These must be preceded (column 1 and 2) by the # symbol followed by one space. The file should preferably end with the Keyword "END".

Times are in seconds, lengths in meters, angles in degrees, frequencies in Hz, ...

There is a variety of output files. Some of them are related to the phasemeter outputs. One of them is related to the output of the various TDI combinations. These may be defined in the configuration file and hence the output file will reflect this choice. In the examples given in http://www.apc.univ-paris7.fr/LISA-France/analyse.phtml the output file consists of 9 columns. The first column is the time, the second, third, fourth and fifth columns give the alpha, beta, gamma and zeta TDI combination [<a href="Bibliography.html#TDIRevue">Tinto&nbsp;2004</a>]. The sixth column gives the first generation Michelson combination (X1s1) related to satellite s1 (that is to say using the arms between s1 and s2 and s1 and s3). The seventh, eighth and ninth columns give the 2nd generation Michelson combination for satellite 1,2 and 3.



*/
 

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "randlib.h"
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Tools.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-GW.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-GWSto.h"
#include "LISACODE-Memory.h"
#include "LISACODE-MemoryWriteDisk.h"
#include "LISACODE-MemoryReadDisk.h"
#include "LISACODE-TDI_InterData.h"
#include "LISACODE-TDITools.h"
#include "LISACODE-TDI.h"
#include "LISACODE-LISA.h"
#include "LISACODE-ConfigSim.h"

using namespace std;
/*!\defgroup main Main (directory Main) 
 \{
*/
/*! \brief LISA simulator.
 * \arg Initialization.\n
 * Random generator is initialized.\n
 * Config is a #ConfigSim instance created with data read from "ConfigRefBase" file.\n
 * RecordPDPM is a #Memory vector where spacecraft signals wil be recorded.\n
 * LISACode is a LISA instance created with Config and RecordPDPM.\n
 * Eta signals are created.\n
 * TDI generators are created using approximative delay computation specified in Config.\n
 * \arg Data processing first step : \f$ \textrm{time }t=0,\dots,tMemTDI+tTDIShift  \textrm{ with tStepMes timsetep}\f$.\n
 * Signals are stored.\n
 * LISA::MakeOneStepOfTime method is called.\n
 * Delays are recorded.\n
 * Positions are recorded.\n
 * \arg Data processing second step : when there are enough data, TDI is computed and results are stored in file, 
 * \f$ \textrm{while time }t \le tmax  \textrm{ with tStepMes timsetep}\f$.\n
 * TDI is computed using TDI_InterData::ComputeEta method.\n
 * Delays are recorded.\n
 * Positions are recorded.\n
 */

int main (int argc, char * const argv[])
{
	try {
		
		// *********** HELP *************
		if(((argc>1)&&(strcmp(argv[1],"--help")==0))||((argc>1)&&(strcmp(argv[1],"-h")==0))){
			cout << " ----- HELP -----" << endl;
			cout << endl << "\tExecution :" << endl;
			cout << "\t\t(./)LISACode [Options] ConfigFileName RandomSeed" << endl;
			cout << endl << "\tArguments :" << endl;
			cout << endl << "\t\t * ConfigFileName (required) : Configuration file name. " << endl;
			cout << endl << "\t\t * RandomSeed (not required) : Seed of random generator. If it's" << endl;
			cout << "\t\t\t  not specified the random seed is the time machine." << endl;
			cout << endl << "\tOptions :" << endl;
			cout << "\t\t * -H : Write header in binary files."  << endl ;
			cout << endl << "\tNOTE :" << endl;
			cout <<  "\t\t There are more details in the user's manual." << endl;
			cout << endl << " ----------------" << endl;
			return 0;
			
		}
		
		// *********** Version *************
		if(((argc>1)&&(strcmp(argv[1],"--version")==0))&&((argc>1)&&(strcmp(argv[1],"-v")==0))){
			cout << " ----- VERSION -----" << endl;
			cout << " LISACode : main executable of LISACode package - version " << LCVersion << " at " << DateOfLastUpdate << endl;
			cout << " ----------------" << endl;
			return 0;
		}
		
		Tools MT;
		
		cout << endl << "   ************************* ";
		cout << endl << "   *                       * ";
		cout << endl << "   *       LISACode        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *   Simulator of LISA   * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *  (LISACode v "<<LCVersion<<")   * ";
		cout << endl << "   *                       * ";
		cout << endl << "   ************************* " << endl << endl; 
		
		// ************************************
		// * Read configuration file and help *
		// ************************************
		char * ConfigFileName("ConfigRefBase");
		//char * ConfigFileName("ConfigSMBH3_NN");
		//char * ConfigFileName("QuickConfig_LAG4.xml");
		//char * ConfigFileName("QuickConfig_LAG4_AfterAppDelay.xml");
		//char * ConfigFileName("ConfigTraining-111a_NN_Nov3");
		//char * ConfigFileName("challenge2.2-frequency-training.xml");
		int nOption(0);
		bool BinHeader(false);
		
		
		
		// *********** Options *************
		for(int iarg=1; iarg<argc; iarg++){
			if((argc>1)&&(strcmp(argv[iarg],"-H")==0)){
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
		
		
		
		// *********** Initialisation du generateur aleatoire *************
		time_t tc;
		long is1, is2;
		// Define tc
		if(argc>2+nOption){
			tc = atoi(argv[2+nOption]);
			cout << "Fixed random seed : " << tc << endl; 
		}else{
			if(Config.getGlobalRandomSeed() != -1){
				tc = Config.getGlobalRandomSeed();
				cout << "Configuration file random seed : " << tc << endl;
			}else{
				//srand((unsigned)time(NULL));
				tc = time(NULL); // Temps : mot de 32 bits
				cout << "Free random seed : " << tc << endl;
			}
		}
		
		
		is1 = is2 = 0; // Tous les bits a 0
		/* On fait :
			tc  =bit= b[n-1]b[n-2]...b1b0
			is1 =bit= b0b2...b[n/2-1]
			is2 =bit= b1b3...b[n/2]
			*/
		for (int i=0;i<16;i++) {
			is1 |= (tc&1)<<(16-i); // On prend le bit de poids le plus faible que l'on met a la correspondance sur le bit de poid le plus fort en 16 bits !
			tc>>=1; // Decale de 1 bit = divise par 2
			is2 |= (tc&1)<<(16-i); // idem
			tc>>=1; // Redecalage de 1 bit = divise par 2
		}
		setall(is1,is2);
		getsd(&is1,&is2);
		//cout << "is1 = " << is1 << " et is2 = " << is2 << endl;
		
		
		
		// ****************
		// * Declarations *
		// ****************
		cout << endl << "Creation ..." << endl;
		
		cout << "  - Parameters...";
		//Declaration des variables qui parametrisent la Simulation
		//double tStepPhy(Config.gettStepPhy());
		double tStepMes(Config.gettStepMes());
		double tMax(Config.gettMax());
		double tDur(Config.gettDur());
		double tOffset(Config.gettOffset());
		double DisplayStep(Config.gettDisplay());
		// Temporary variables
		double t(0.0), ttmpAff(0.0), tSinceFirstReception(0.0);
		// * Critere sur la moyenne d'un generateur TDI pour qu'il soit declare efficace
		//double TDIGood(1.0e-19);
		// * Offset between real delays and delays used in TDI (in seconds)
		double DeltaTDIDelay(Config.gettDeltaTDIDelay());
		double tTDIShift(Config.gettTDIShift());
		double tMemPDPM(Config.gettMemPhasemeters());
		double tMemTDI(Config.gettMemTDI());
		Vect tmp;
		// ** For time display 
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
			if(strcmp(Config.getFileNameSig(iSC),"None")==0){
				cout << "     Record Signal SC" << iSC << " : RAM" << endl;
				RecordPDPM.push_back(new Memory(tMemPDPM, tStepMes));
				// TO BE CONTROL : Voir si le TimeStore (premier parametre est correct car ici on a
				// tTDIShift+Config.tMaxDelay()+Config.tMemNecInterpTDI() qui correspond à une valeur tres
				// grande (plusieurs retards TDI) ; or la mise en memoire pour les generateurs TDI est faite 
				// dans TDI_InterData Eta qui lui va aller chercher dans la memoire RecordPDPM des valeurs retardees
				// au maximum d'une longueur de bras
			}else{
				cout << "     Record Signal SC" << iSC << " : RAM & DISK (" << Config.getFileNameSig(iSC) << ")" << endl;
				RecordPDPM.push_back(new MemoryWriteDisk(tMemPDPM, tStepMes, Config.getFileNameSig(iSC), Config.getFileEncodingSig(iSC), BinHeader,-1.0*(tMemTDI+tTDIShift),tMax+tTDIShift));
			}
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
			
		//ofstream RecordNoise, RecordEta; // Enregistrement intermediqire des Bruit et Eta 
		//vector<double> ListNoises; // Liste qui stocke les bruits pour leur enregistrement
								   //Declaration des retards
		Memory * DelayTDI;
		if(strcmp(Config.getFileNameDelays(),"None")==0){
			cout << "     Record Delays     : RAM" << endl;
			DelayTDI = new Memory(tMemTDI, tStepMes);
		}else{
			cout << "     Record Delays     : RAM & DISK (" << Config.getFileNameDelays() << ")" << endl;
			DelayTDI = new MemoryWriteDisk(tMemTDI, tStepMes, Config.getFileNameDelays(), Config.getFileEncodingDelays(), BinHeader,-1.0*(tMemTDI+tTDIShift),tMax+tTDIShift);
		}
		for (int IndirectDir=0 ; IndirectDir<2; IndirectDir++){
			for(int iSC=1; iSC<4; iSC++){
				DelayTDI->AddSerieData(iSC+3*IndirectDir-1, "D" , IndirectDir, iSC);
			}
		}
		// ** Declaration of delays and sky position
		Memory * SCPos;
		if(strcmp(Config.getFileNamePositions(),"None")==0){
			cout << "     Record Positions  : RAM" << endl;
			SCPos = new Memory(tMemTDI, tStepMes);
		}else{
			cout << "     Record Positions  : RAM & DISK(" << Config.getFileNamePositions() << ")" << endl;
			SCPos = new MemoryWriteDisk(tMemTDI, tStepMes, Config.getFileNamePositions(), Config.getFileEncodingPositions(), BinHeader,-1.0*(tMemTDI+tTDIShift),tMax+tTDIShift);
		}
		for(int iSC=1; iSC<4; iSC++){
			SCPos->AddSerieData(iSC-1, "px" , 0, iSC);
			SCPos->AddSerieData(iSC+3-1, "py" , 0, iSC);
			SCPos->AddSerieData(iSC+6-1, "pz" , 0, iSC);
		}
		cout << " --> OK !" << endl << endl;
		
		// *** Declaration of LISA
		cout << "  - LISA..." << endl;
		LISA LISACode(& Config, & RecordPDPM);
		cout << "    LISA --> OK !" << endl << endl;
		
		// *** Declaration TDI generators
		cout << "  - TDI... " << endl;
		// ** Creation of Eta signals
		TDI_InterData Eta(DelayTDI, & RecordPDPM, tMemTDI, tTDIShift/2.0, Config.getNoNoise(), Config.getTDIInterp(), Config.getTDIInterpVal());
		// ** Acceleration module of TDI
		TDITools TDIQuickMod(DelayTDI, Config.getTDIDelayApprox());
		// ** Creation of generators
		vector<TDI> TDIGens;
		int NbGenTDI (Config.NbGenTDI());
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " ...";
			TDIGens.push_back(TDI(&MT, DelayTDI, & Eta, & RecordTDI, Config.getFileEncodingTDI(), iGen, Config.getGenTDIPacks(iGen), Config.getGenTDIPacksFact(iGen), & TDIQuickMod));
			cout << "    Creation of " << Config.getNameGenTDI(iGen) << " --> OK" << endl;
		}
		cout << "    TDI --> OK !" << endl << endl;
		
		cout << "Creation --> OK" << endl << endl;
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		
		for(int iSC=1; iSC<=3; iSC++)
			RecordPDPM[iSC-1]->MakeTitles(ConfigFileName);
		// ** Write TDI file Header
		if(Config.getFileEncodingTDI() == 0){
			ifstream FileHead;
			char Buf[1064] ;
			// * Copy of configuration file in header
			FileHead.open(ConfigFileName);
			if (!FileHead){
				throw invalid_argument("Main : Can not open the configuration file ! ");
			}
			RecordTDI << "############## LISACode v " << LCVersion << "-" << DateOfLastUpdate << " at " << MathUtils::TimeISO8601() << " ##############" << endl;
			RecordTDI << "############## Configuration : " << ConfigFileName <<  " ##############" << endl;
			while(!FileHead.eof()) {
				FileHead.getline(Buf, 1064);
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
				RecordTDI << "#RECORD " << NbGenTDI+1<< " " << (int)ceil((tMax)/tStepMes) <<  endl;
				RecordTDI << "#TIME 0.0 " << tStepMes << " " << tMax+tTDIShift << endl;
			}
		}
		
		DelayTDI->MakeTitles(ConfigFileName);
		SCPos->MakeTitles(ConfigFileName);
		//RecordNoise.open("NoiseSignal.txt");
		//RecordNoise << "Temps";
		//RecordNoise << " p1 p2 p3 pp1 pp2 pp3";
		//RecordNoise << " DeBO1 DeBO2 DeBO3 DeBOp1 DeBOp2 DeBOp3";
		//RecordNoise << " deMR1 deMR2 deMR3 deMRp1 deMRp2 deMRp3";
		//RecordNoise << endl;
		//RecordEta.open("EtaSignal.txt");
		//RecordEta << "#Time Eta1 Eta2 Eta3 Etap1 Etap2 Etap3 Eta3-Etap2" << endl;
		
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
		t = tOffset+Config.gettStartPhasemeters();
		//cout << t << " " << tMemTDI << " " << tTDIShift << endl;
		do{
			//Display time
			//cout << "* t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				//cout << t << " s" << endl;
				printf("%.0lf s     #0%03.0f %% \n", t, 100*(t-tOffset)/tDur);
				fflush(stdout);
				ttmpAff = 0.0;
			}
			
			// Run LISA
			LISACode.MakeOneStepOfTime(t);
			
			/*		
				ListNoises = LISACode.PresentMeanNoise(t);
			RecordNoise << t;
			for (int i=0; i<int(ListNoises.size()); i++)
				RecordNoise << " " << ListNoises[i];		
			RecordNoise << endl; 
			*/		 
			//Record delays
			for(int s=0; s<2; s++){
				for (int i=1; i<4; i++){
					DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+DeltaTDIDelay);
					//DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+((double)genunf(0.0, 1.0)-0.5)*DeltaTDIDelay);
				}
			}
			DelayTDI->RecordAccData(tStepMes, t);
			//Record positions
			for (int i=1; i<4; i++){
				tmp = LISACode.gPosSC(i, t);
				SCPos->ReceiveData(i-1, tmp.p[0]);
				SCPos->ReceiveData(i+3-1, tmp.p[1]);
				SCPos->ReceiveData(i+6-1, tmp.p[2]);
			}
			SCPos->RecordAccData(tStepMes, t);
			
			t += tStepMes;
			tSinceFirstReception += tStepMes;
			

			ttmpAff += tStepMes;
			
			NDatPh1++;
			
		//}while(t<=tMemTDI+tTDIShift);
		}while(t<tOffset+tTDIShift-tMemTDI);
		
		// Phase 2 : 
		// ---------
		// Il y a assez de donnees pour calculer les Eta mais pas encore pour appliquer TDI.
		cout << endl << "Runnning in progress : enoughe data for pre-computing of TDI (-> " << tMemTDI << " s)..." << endl;
		do{
			//Display time
			//cout << "* t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				//cout << t << " s" << endl;
				printf("%.0lf s     #0%03.0f %% \n", t, 100*(t-tOffset)/tDur);
				fflush(stdout);
				ttmpAff = 0.0;
			}
			
			// Run LISA
			LISACode.MakeOneStepOfTime(t);
			
			/*		
			 ListNoises = LISACode.PresentMeanNoise(t);
			 RecordNoise << t;
			 for (int i=0; i<int(ListNoises.size()); i++)
			 RecordNoise << " " << ListNoises[i];		
			 RecordNoise << endl; 
			 */		 
			
			//Record delays
			for(int s=0; s<2; s++){
				for (int i=1; i<4; i++){
					DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+DeltaTDIDelay);
					//DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+((double)genunf(0.0, 1.0)-0.5)*DeltaTDIDelay);
				}
			}
			DelayTDI->RecordAccData(tStepMes, t);
			//Record positions
			for (int i=1; i<4; i++){
				tmp = LISACode.gPosSC(i, t);
				SCPos->ReceiveData(i-1, tmp.p[0]);
				SCPos->ReceiveData(i+3-1, tmp.p[1]);
				SCPos->ReceiveData(i+6-1, tmp.p[2]);
			}
			SCPos->RecordAccData(tStepMes, t);
			
			Eta.ComputeEta();
			
			t += tStepMes;
			tSinceFirstReception += tStepMes;
			
			
			ttmpAff += tStepMes;
			
			NDatPh1++;
			
		}while(t<tOffset+tTDIShift);
		
		// Phase 3 : 
		// ---------
		//  On a maintenant assez de donnees pour appliquer TDI. On calcule donc les resultats
		//  des generateurs et on les enregistre dans un fichier.
		cout << endl << "Runnning in progress : Enough recorded data ..." << endl;
		cout << "   --> Compute TDI directly (->" << tMax << "s) ..." << endl;
		vector<double> ResultTDI;
		vector<double> AbsMeanTDI;
		ResultTDI.resize(Config.NbGenTDI());
		AbsMeanTDI.resize(Config.NbGenTDI());	 
		int NbTDIApp(0);
		time(&tstart);
		//cout << t << " s" << endl;
		do{
			//cout << "** t = " << t << endl;
			if(ttmpAff >= DisplayStep){
				time(&tcur);
				//cout << t << " s" << endl;
				EstimTStop = (tcur-tstart)*(tMax-t)/(t-tOffset);
				hh = (int)(floor(EstimTStop/3600.0));
				mm = (int)((EstimTStop - 3600.0*hh)/60);
				ss = EstimTStop - 60*(mm+60*hh);
				//cout << t << " s" << endl;
				printf("%.0lf s    (remaining time : %02d:%02d:%02.0f) #0%03.0f %% \n", t, hh, mm, ss, 100*(t-tOffset)/tDur);
				fflush(stdout);
				ttmpAff = 0.0;
			}		 
			
			// Run LISA
			LISACode.MakeOneStepOfTime(t);   
			
			/*		 //Record Noise
				ListNoises = LISACode.PresentMeanNoise(t);
			RecordNoise.precision(15);
			RecordNoise << t;
			for (int i=0; i<int(ListNoises.size()); i++)
				RecordNoise << " " << ListNoises[i];
			RecordNoise << endl; 
			*/		 
			// Record delays
			for(int s=0; s<2; s++){
				for (int i=1; i<4; i++){
					// Incertitude sur les retards fixe
					DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+DeltaTDIDelay);
					// Incertitude sur les retards tiree uniformement
					//DelayTDI->ReceiveData(i+3*s-1, LISACode.gDelayT(i, s, t)+((double)genunf(0.0, 1.0)-0.5)*DeltaTDIDelay);
				}
			}
			DelayTDI->RecordAccData(tStepMes, t);
			//Record positions
			for (int i=1; i<4; i++){
				tmp = LISACode.gPosSC(i, t);
				SCPos->ReceiveData(i-1, tmp.p[0]);
				SCPos->ReceiveData(i+3-1, tmp.p[1]);
				SCPos->ReceiveData(i+6-1, tmp.p[2]);
			}
			SCPos->RecordAccData(tStepMes, t);
			
			
			
			//Compute TDI
			NbTDIApp++;
			
			Eta.ComputeEta();
			/*// ** Record Eta signal
			if(Eta.getUsable()){
				RecordEta.precision(15);
				RecordEta << t-tTDIShift ;
				for(int i=1; i<7; i++)
					RecordEta << " " << Eta.gData(i, tTDIShift);
				RecordEta << " " << Eta.gData(3, tTDIShift)-Eta.gData(5, tTDIShift);
				RecordEta << endl;
			}
			 */
			
			
			//if(t>179.0)
			//	cout << "stop" << endl;
			
			// Compute and record TDI generators
			RecordTDI.precision(10);
			ttmp = t-tTDIShift;
			if(Config.getFileEncodingTDI() == 0) // If TDIFile is an ASCII file, write time in ASCII
				RecordTDI << ttmp;
			else // Else write time in Binary
				RecordTDI.write((char*) &ttmp, sizeof(double));
			if(TDIQuickMod.getRapidOption())
				TDIQuickMod.RefreshDelay(tTDIShift);
			
			
			////if((t>34602000)&&(t<34603000)){
			////	TDIGens[0].DEBUGWRITE = true;
			////	cout << " ======================>>>>>>>>>>>>>>>>>>>>>>>> t = " << t << endl ;
			////}else {
			////	TDIGens[0].DEBUGWRITE = false;
			////}
			
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
			
		}while(t<tMax+tTDIShift);
		
		
		// Read TDI result
		cout.precision(15);
		for(int iGen=0; iGen<NbGenTDI; iGen++){
			AbsMeanTDI[iGen] /= NbTDIApp*1.0;
			cout << " " << Config.getNameGenTDI(iGen) << " = " << AbsMeanTDI[iGen];
			//cout << "    (count Delay = " << TDIGens[iGen].getCountInterDelay();
			//cout << "  ,  count Eta = " << TDIGens[iGen].getCountInterEta() << " )";
			/*cout << " -- > mag = " << log10(1.0e-13/fabs(AbsMeanTDI[iGen]));
			if(fabs(AbsMeanTDI[iGen]) < TDIGood)
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " successful !";
			else
				cout << " --> TDI " << Config.getNameGenTDI(iGen) << " not good !"; */
			cout << endl;
		}
		
		
		//RecordNoise.close();
		//RecordEta.close();
		
		cout << "Final time : " << t << " s" << endl << endl;
		
		//cout << "Header for binary file SCi(NRec=5), Pos(NRec=10) or Delay(NRec=7) :" << endl;
		//cout << "#TITLE SimDataTitle" << endl; 
		//cout << "#RECORD NRec " << NDatPh1+NDatPh2 << " " << (int)ceil((tMax+tMemTDI+tTDIShift)/tStepMes+1) << endl;
		//cout << "#TIME " <<  -1.0*(tMemTDI+tTDIShift) << " " << tStepMes << " " << t-tStepMes << " " << tMax+tTDIShift << endl;
		//cout << endl;
		//cout << "Header for binary file TDI :" << endl;
		//cout << "#TITLE TDIDataTitle" << endl; 
		//cout << "#RECORD " << NbGenTDI+1<< " " << NDatPh2 << " " << (int)ceil((tMax)/tStepMes+1) <<  endl;
		//cout << "#TIME 0.0 " << tStepMes << " " << t-tStepMes << " " << tMax+tTDIShift << endl;
		
		for(int iSC=1; iSC<=3; iSC++)
			delete RecordPDPM[iSC-1];
		delete DelayTDI;
		delete SCPos;
		RecordTDI.close();
		
		cout << endl << "End" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "LISACode: error: " << e.what()<<endl;
		cerr << "LISACode: abort!" << endl;
		exit(1);
	}
	return(0);
}

/*! \}*/

// end of LISACode.cpp
