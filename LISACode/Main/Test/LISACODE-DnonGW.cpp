// $Id: LISACODE-DnonGW.cpp,v 1.2 2007/02/19 10:07:16 ruiloba Exp $
/*
 *  LISACODE-DnonGW.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 26/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 01/10/06 by A.Petiteau, T.Regimbau 
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Tools.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-GWMono.h"
#include "LISACODE-GWFile.h"
#include "LISACODE-GWBinary.h"
#include "LISACODE-GWPeriGate.h"
#include "LISACODE-GWSto.h"
#include "LISACODE-Geometry.h"
#include "LISACODE-TrFctGW.h"
#include "LISACODE-ConfigSim.h"
using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		// *********** HELP *************
		if(((argc>1)&&(strcmp(argv[1],"--help")==0))||((argc>1)&&(strcmp(argv[1],"-h")==0))){
			cout << " ----- HELP -----" << endl;
			cout << endl << "\tExecution :" << endl;
			cout << "\t\t(./)DnonGW ConfigFileName SignalFileName GWStrainFileName PositionFileName DelayFileName" << endl;
			cout << endl << "\tArguments :" << endl;
			cout << endl << "\t\t * ConfigFileName (required)       : Configuration file name. Same file than LISACode" << endl;
			cout <<         "\t\t    configuration but only measurement time step, maximal time (duration), gravitational" << endl;
			cout <<         "\t\t    waves and orbit parameters are required." << endl;
			cout << endl << "\t\t * SignalFileName (required)       : Output file for the gravitational strain on 6 phasemeters." << endl;
			cout << endl << "\t\t * GWStrainFileName (not required) : Output file for the gravitational strain for each GWs." << endl;
			cout << endl << "\t\t * PositionFileName (not required) : Output file for the spacecraft positions." << endl;
			cout << endl << "\t\t * DelayFileName (not required)    : Output file for the time delay between spacecrafts." << endl;
			cout << endl << "\tNOTE :" << endl;
			cout <<  "\t\t There are more details in the user's manual." << endl;
			cout << endl << " ----------------" << endl;
			return 0;
			
		}
		// *********** Version *************
		if(((argc>1)&&(strcmp(argv[1],"--version")==0))&&((argc>1)&&(strcmp(argv[1],"-v")==0))){
			cout << " ----- VERSION -----" << endl;
			cout << " DnonGW : executable of LISACode package - version " << LCVersion << " at " << DateOfLastUpdate << endl;
			cout << " ----------------" << endl;
			return 0;
		}
		
		Tools MT;
		
		cout << endl << "   **************************** ";
		cout << endl << "   *                          * ";
		cout << endl << "   *        LISACode          * ";
		cout << endl << "   *   --------------------   * ";
		cout << endl << "   *   Gravitational Strain   * ";
		cout << endl << "   *   --------------------   * ";
		cout << endl << "   *    (LISACode v "<<LCVersion<<")     * ";
		cout << endl << "   *                          * ";
		cout << endl << "   **************************** " << endl << endl; 
		
		// ***************************************
		// * Lecture du fichier de configuration *
		// ***************************************
		
		char * ConfigFileName("ConfigRefBase");
		char * SigFileName("DnGW_SigFctTrGW.txt");
		char * GWsFileName("None");
		char * PosFileName("None");
		char * DelayFileName("None");
		bool RecGWs(false), RecPos(false), RecDelay(false);
		bool BinarySig(false);
		char * extention;
		char * ptr;
		double * RecVal;
		
		
		if(argc>1)
			ConfigFileName = argv[1];
		
		
		// *********** Read file names *************
		if(argc>2)
			SigFileName = argv[2];
		
		if(argc>3)
			GWsFileName = argv[3];
		if((strcmp(GWsFileName,"None")!=0)&&(strcmp(GWsFileName,"0")!=0)&&(strcmp(GWsFileName,"No")!=0))
			RecGWs = true;
		
		if(argc>4)
			PosFileName = argv[4];
		if((strcmp(PosFileName,"None")!=0)&&(strcmp(PosFileName,"0")!=0)&&(strcmp(PosFileName,"No")!=0))
			RecPos = true;
		
		if(argc>5)
			DelayFileName = argv[5];
		if((strcmp(DelayFileName,"None")!=0)&&(strcmp(DelayFileName,"0")!=0)&&(strcmp(DelayFileName,"No")!=0))
			RecDelay = true;
		
		
		// *********** Configuration file *************
		cout << endl << "Read configuration (" << ConfigFileName << ")..." << endl;
		ConfigSim Config(ConfigFileName);
		
		// Read output file extention : if .bin , the output is a binary file
		extention=(char*)malloc(16*sizeof(char));
		ptr=strchr(SigFileName,'\0');
		strcpy(extention,"\0");
		if(ptr!=NULL) strncat(extention,ptr-4,16);
		cout << endl << "Output file " << SigFileName << "  : ";
		if(strncmp(extention,".bin",4)==0){
			BinarySig = true;
			cout << "Binary" << endl;
		}else{
			BinarySig = false;
			cout << "ASCII" << endl;
		}
		cout << endl;
		
		// ****************
		// * Declarations *
		// ****************
		cout << "Creation of parameters ..." << endl;
		// Declaration of variables which control the simulation
		double tStep(Config.gettStepMes()), tmax(Config.gettMax());
		double StepdDisplay(Config.gettDisplay());
		// Declaration of temporary variables
		double t(0.0), ttmpAff(0.0);
		// Gravitationnal wave parameters
		//double FreqGW(1.0e-5), AmplGWhp(1.0), AmplGWhc(0.0e-21), AnglPolGW(MathUtils::deg2rad(0.0));
		//double BetaGW(MathUtils::deg2rad(90.0)), LambdaGW(MathUtils::deg2rad(0.0)); 
		//Order in compute of tdelay
		int order(2);	
		
		cout << endl << "Creation of items ..." << endl;
		// Creation of the gravitationnals waves
		vector<GW *> * m_GWs;
		m_GWs = Config.getGWs();
		//m_GWs.push_back(new GWMono(BetaGW, LambdaGW, AnglPolGW, FreqGW, AmplGWhp, AmplGWhc)); 
		//m_GWs.push_back(new GWFile(BetaGW, LambdaGW, AnglPolGW, "SinGW.txt"));
		//m_GWs.push_back(new GWBinary); //AMCVn
		//for(int i=0; i<1; i++)
		//	m_GWs->push_back(new GWSto(genunf(-0.5*M_PI,0.5*M_PI), genunf(0.0,M_PI), tStep, 1.0*tStep, 10.0*tStep, -100.0*tStep,  -1000.0,  tStep, -3.0,  1.0/(2.0*tStep), 1.0/tmax, 100000, 1.0, 1.0));
		
		// Creation of the geometry
		Geometry * LISAGeo;
		Config.getGeometry(LISAGeo);
		
		// Creation of the transfer fonction
		TrFctGW SigGW(m_GWs, LISAGeo);
		
		// Declaration of used memorys
		ofstream Record_position, Record_tdelay, Record_Sig, Record_GW ;
		FILE * Record_Sig_bin;
		
		// For time display 
		time_t tstart, tcur;
		float EstimTStop;
		int hh, mm;
		float ss;
		
		cout << "  Creation --> OK" << endl << endl;
		
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		
		//File of recording gravitational wave
		if(RecGWs){
			Record_GW.open(GWsFileName); 
			Record_GW << "#Time";
			for(int iGW=0; iGW<m_GWs->size(); iGW++){
				Record_GW << " hp"<< iGW+1 << " hc"<< iGW+1;
			}
			Record_GW << endl;
		}
		//File of recording of the positions
		if(RecPos){
			Record_position.open(PosFileName);
			Record_position<< "#Time px1 py1 pz1 px2 py2 pz2 px3 py3 pz3 nx ny nz" << endl;
		}
		//File of recording of the delays
		if(RecDelay){
			Record_tdelay.open(DelayFileName);
			Record_tdelay << "#Time D12 D23 D31 D13 D21 D32" << endl;
		}
			
		
		//File of recording signals
		if(BinarySig){
			Record_Sig_bin = fopen(SigFileName, "wb");
			fprintf(Record_Sig_bin, "#TITLE %s \n",SigFileName);
			fprintf(Record_Sig_bin, "#RECORD 6 %d \n",  (int)(tmax/tStep));
			fprintf(Record_Sig_bin, "#TIME %lf %lf %lf \n", t, tStep, tmax);
		}else{
			Record_Sig.open(SigFileName);  
			Record_Sig << "#Time s1 s2 s3 sp1 sp2 sp3" << endl;
		}
		
		RecVal = (double*)malloc(6*sizeof(double));
		
		
		
		
		
		cout << "  Initialisations --> OK" << endl << endl;
		
		
		// *************
		// * Execution *
		// *************
		
		cout << "Running in progress (->"<< tmax << "s) ..." << endl;
		Vect tmp_pos;
		Vect tmp_n;
		Record_Sig.precision(15);
		if(RecGWs)
			Record_GW.precision(15);
		if(RecPos)
			Record_position.precision(15);
		if(RecDelay)
			Record_tdelay.precision(15);
		
		
		
		time(&tstart);
		do{
			if(ttmpAff >= StepdDisplay){
				//cout << t/(24.0*3600.0) << " days" << endl;
				time(&tcur);
				//cout << t << " s" << endl;
				EstimTStop = (tcur-tstart)*(tmax-t)/t;
				hh = (int)(floor(EstimTStop/3600.0));
				mm = (int)((EstimTStop - 3600.0*hh)/60);
				ss = EstimTStop - 60*(mm+60*hh);
				//cout << t << " s" << endl;
				printf("%.0lf s    (remaining time : %02d:%02d:%02.0f) #0%03.0f %% \n", t, hh, mm, ss, 100*t/tmax);
				fflush(stdout);
				ttmpAff = 0.0;
				//Record positions
				if(RecPos){
					Record_position << t;
					for(int iSC=1; iSC<4; iSC++) {
						tmp_pos = LISAGeo->gposition(iSC, t);
						Record_position << " " << tmp_pos.p[0] << " " << tmp_pos.p[1] << " " << tmp_pos.p[2];
					}
					tmp_n = LISAGeo->VectNormal(t);
					Record_position << " " << tmp_n.p[0] << " " << tmp_n.p[1] << " " <<tmp_n.p[2] << endl;
				}
				
				//Record Delay
				if(RecDelay){
					Record_tdelay << t ;
					Record_tdelay << " " << LISAGeo->tdelay(1, 2, order, t);
					Record_tdelay << " " << LISAGeo->tdelay(2, 3, order, t);
					Record_tdelay << " " << LISAGeo->tdelay(3, 1, order, t);
					Record_tdelay << " " << LISAGeo->tdelay(1, 3, order, t);
					Record_tdelay << " " << LISAGeo->tdelay(2, 1, order, t);
					Record_tdelay << " " << LISAGeo->tdelay(3, 2, order, t);
					Record_tdelay << endl;
				}
			}
			// Record GWs
			if(RecGWs){
				Record_GW << t;
				for(int iGW=0; iGW<m_GWs->size(); iGW++){
					Record_GW << " " << (*m_GWs)[iGW]->hp(t);
					Record_GW << " " << (*m_GWs)[iGW]->hc(t);
				}
				Record_GW << endl;
			}
			
			RecVal[0] = SigGW.deltanu(1, 2, order, t);
			RecVal[1] = SigGW.deltanu(2, 3, order, t);
			RecVal[2] = SigGW.deltanu(3, 1, order, t);
			RecVal[3] = SigGW.deltanu(1, 3, order, t);
			RecVal[4] = SigGW.deltanu(2, 1, order, t);
			RecVal[5] = SigGW.deltanu(3, 2, order, t);
			
			if(BinarySig){
				fwrite(RecVal,sizeof(double),6,Record_Sig_bin);
			}else{
				Record_Sig << t;
				for(int iV=0; iV<6; iV++)
					Record_Sig << " " << RecVal[iV];
				Record_Sig << endl;
			}
			
			t += tStep;
			//Display and record
			ttmpAff += tStep;
			
			
		}while(t<=tmax);
		
		cout << "Temps final : " << t << " s" << endl;	
		
		Record_position.close();
		Record_tdelay.close();
		
		if(BinarySig)
			fclose(Record_Sig_bin);
		else
			Record_Sig.close();
		
		cout << "Fin" << endl;
	}	
	
	catch(exception & e ) {
		cerr << "TestTrFctGW: error: " << e.what()<<endl;
		cerr << "TestTrFctGW: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-DnonGW.cpp
