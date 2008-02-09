// $Id: LISACODE-TestTrFctGW.cpp,v 1.2 2007/02/19 10:24:46 ruiloba Exp $
/*
 *  LISACODE-TestTrFctGW.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 26/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 18/05/06 by A.Petiteau
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
#include "LISACODE-Geometry.h"
#include "LISACODE-TrFctGW.h"
using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *******************************";
		cout << endl << "   *                             *";
		cout << endl << "   *   Test the LISA response    *"; 
		cout << endl << "   *   ----------------------    *";
		cout << endl << "   *   to a gravitational wave   *";
		cout << endl << "   *   -----------------------   *";
		cout << endl << "   *                             *";
		cout << endl << "   *******************************" << endl << endl; 
		
		
		
		
		// ************************
		// * begin of declaration *
		// ************************
		cout << "Creation of parameters ..." << endl;
		// Declaration of variables which control the simulation
		double tStep(1.0), tmax(1e6);//tmax(pow(2,25));
			double StepdDisplay(24*3600.0);
			// Declaration of temporary variables
			double t(0.0), ttmpAff(0.0);
			// Gravitationnal wave parameters
			double FreqGW(1.0e-5), AmplGWhp(1.0), AmplGWhc(0.0e-21), AnglPolGW(MathUtils::deg2rad(0.0));
			double BetaGW(MathUtils::deg2rad(90.0)), LambdaGW(MathUtils::deg2rad(0.0)); 
			//Order in compute of tdelay
			int order(2);	
			
			cout << endl << "Creation of items ..." << endl;
			// Creation of the gravitationnals waves
			vector<GW *> m_GWs;
			m_GWs.push_back(new GWMono(BetaGW, LambdaGW, AnglPolGW, FreqGW, AmplGWhp, AmplGWhc)); 
			//m_GWs.push_back(new GWFile(BetaGW, LambdaGW, AnglPolGW, "SinGW.txt"));
			//m_GWs.push_back(new GWBinary); //AMCVn
			
			// Creation of the geometry
			Geometry LISAGeo;
			// Creation of the transfer fonction
			TrFctGW SigGW(&m_GWs, &LISAGeo);
			
			// Declaration of used memorys
			ofstream Record_position, Record_tdelay, Record_Sig, Record_GW ; 
			Record_position.open("TRFCTposition.txt"); //File of recording of the positions
			Record_tdelay.open("TRFCTtdelay.txt"); //File of recording of the delays
			Record_GW.open("TRFCTGW.txt"); //File of recording gravitational wave
			Record_Sig.open("TRFCTSigFctTrGW.txt"); //File of recording signals
			
			cout << "  Creation --> OK" << endl << endl;
			
			
			
			
			// *******************
			// * Initialisations *
			// *******************
			cout << "Initialisations ..." << endl;
			Record_position<< "#Time px1 py1 pz1 px2 py2 pz2 px3 py3 pz3" << endl;
			Record_tdelay << "#Time D12 D23 D31 D13 D21 D32" << endl;
			Record_GW << "#Time hp hc" << endl;
			Record_Sig << "#Time s1 s2 s3 sp1 sp2 sp3" << endl;
			
			cout << "  Initialisations --> OK" << endl << endl;
			
			
			// *****************************************
			// * Display gravitational wave parameters *
			// *****************************************
			for(int iGW=0; iGW<m_GWs.size(); iGW++){
				cout << "Gravitational wave " << iGW << " :" << endl;
				cout << "------------------------" << endl;
				//cout << "  + Frequency = " << m_GW.getFreq() << " Hz" << endl;
				//cout << "  + Amplitude of component h+   = " << m_GW.getAmplhp() << endl;
				//cout << "  + Amplitude of component hx   = " << m_GW.getAmplhc() << endl;
				cout << "  + Angle of polarisation (proj{x},i) = " << m_GWs[iGW]->getAnglPol() << " rad" << endl;
				cout << "  + Source direction : - Beta   =  " << m_GWs[iGW]->getBeta() << " rad" << endl;
				cout << "                       - Lambda =  " << m_GWs[iGW]->getLambda() << " rad" << endl;
				
				vector<double> tmpDir;
				tmpDir = m_GWs[iGW]->getDirProp();
				
				cout << " Propagation direction unit vector : - x =  " << tmpDir[0] << endl;
				cout << "                                     - y =  " << tmpDir[1] << endl;
				cout << "                                     - z =  " << tmpDir[2] << endl;
				
				cout << endl;
			}
			
			
			// *************
			// * Execution *
			// *************
			
			cout << "Running in progress (->"<< tmax << "s) ..." << endl;
			Vect tmp_pos;
			do{
				t += tStep;
				//Display and record
				ttmpAff += tStep;
				if(ttmpAff >= StepdDisplay){
					cout << t/(24.0*3600.0) << " days" << endl;
					ttmpAff = 0.0;
					//Record positions
					Record_position << t;
					for(int iSC=1; iSC<4; iSC++) {
						tmp_pos = LISAGeo.gposition(iSC, t);
						Record_position << " " << tmp_pos.p[0] << " " << tmp_pos.p[1] << " " << tmp_pos.p[2];
					}
					Record_position << endl;
					
					//Record Delay
					Record_tdelay << t ;
					Record_tdelay << " " << LISAGeo.tdelay(1, 2, order, t);
					Record_tdelay << " " << LISAGeo.tdelay(2, 3, order, t);
					Record_tdelay << " " << LISAGeo.tdelay(3, 1, order, t);
					Record_tdelay << " " << LISAGeo.tdelay(1, 3, order, t);
					Record_tdelay << " " << LISAGeo.tdelay(2, 1, order, t);
					Record_tdelay << " " << LISAGeo.tdelay(3, 2, order, t);
					Record_tdelay << endl;
				}
				Record_GW << t;
				Record_GW << " " << m_GWs[0]->hp(t);
				Record_GW << " " << m_GWs[0]->hc(t);
				Record_GW << endl;
				Record_Sig << t;
				Record_Sig << " " << SigGW.deltanu(1, 2, order, t);
				Record_Sig << " " << SigGW.deltanu(2, 3, order, t);
				Record_Sig << " " << SigGW.deltanu(3, 1, order, t);
				Record_Sig << " " << SigGW.deltanu(1, 3, order, t);
				Record_Sig << " " << SigGW.deltanu(2, 1, order, t);
				Record_Sig << " " << SigGW.deltanu(3, 2, order, t);
				Record_Sig << endl;
				
			}while(t<=tmax);
			
			cout << "Temps final : " << t << " s" << endl;	
			
			Record_position.close();
			Record_tdelay.close();
			Record_Sig.close();
			//Record_Psi.close();
			
			cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestTrFctGW: error: " << e.what()<<endl;
		cerr << "TestTrFctGW: abort!" << endl;
		exit(1);
	}
	return(0);
}


// end of LISACODE-TestTrFctGW.cpp