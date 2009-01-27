// $Id: LISACODE-TestConfigSim.cpp,v 1.2 2007/02/19 10:13:35 ruiloba Exp $
/*
 *  LISACODE-TestConfigSim.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 04/01/2006 by  A.Petiteau (APC)
 *  Last modification on 206/06/06 by A.Petiteau (APC)
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include <vector.h>
#include "LISACODE-ConfigSim.h"


using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *******************************";
		cout << endl << "   *                             *";
		cout << endl << "   *       Test Reading of       *"; 
		cout << endl << "   *       ---------------       *";
		cout << endl << "   *  Simulator's Configuration  *";
		cout << endl << "   *  -------------------------  *";
		cout << endl << "   *      ("<<LCVersion<<")      *";
		cout << endl << "   *                             *";
		cout << endl << "   *******************************" << endl << endl; 
		
		
		// ***************************************
		// * Lecture du fichier de configuration *
		// ***************************************
		//char * FileName ("ConfigTest");
		//char * FileName ("ConfigTest.xml");
		//char * FileName ("ConfigLISACodeTest.xml");
		char * FileName ("ConfigRefBase.xml");
		//char * FileName ("ConfigLISACode.xml");
		//char * FileName ("challenge2.2-frequency-training.xml");
		//char * FileName ("ConfigInterface.xml");
		if(argc>1)
			FileName = argv[1];
		cout << endl << "Read configuration (" << FileName << ")..." << endl;
		ConfigSim m_Conf(FileName);
		cout << "Read configuration (" << FileName << ") --> OK !" << endl;
		cout << endl;
		
		
		vector<int> tmpTDIPack;
		
		
		cout << " tStepPhy         = " << m_Conf.gettStepPhy() << " s" << endl;
		cout << " tStepMes         = " << m_Conf.gettStepMes() << " s" << endl;
		cout << " tMax             = " << m_Conf.gettMax() << " s" << endl;
		cout << " tMemNoiseFirst   = " << m_Conf.gettMemNoiseFirst() << " s" << endl;
		cout << " tMemNoiseLast    = " << m_Conf.gettMemNoiseLast() << " s" << endl;
		cout << " tMemSig          = " << m_Conf.gettMemSig() << " s" << endl;
		cout << " tDisplay         = " << m_Conf.gettDisplay() << " s" << endl;
		cout << " tDeltaTDIDelay   = " << m_Conf.gettDeltaTDIDelay() << " s" << endl;
		cout << " InterpUtilVal    = " << m_Conf.getTDIInterpUtilVal() << endl;
		cout << " Armlenght        = " << m_Conf.getArmlength() << " m" << endl;
		cout << " OrbStartTime     = " << m_Conf.getOrbStartTime() << " s" << endl;
		cout << " OrbInitRot       = " << m_Conf.getOrbInitRot()<< " rad" << endl;
		cout << " OrbOrder         = " << m_Conf.getOrbOrder() << endl;
		cout << " OrbType          = " << m_Conf.getOrbType() << endl;
		cout << " OrbApprox        = " << m_Conf.getOrbApprox() << endl;
		cout << " LaserPower       = " << m_Conf.getLaserPower() << " W" << endl;
		cout << " PhaMetFilter     = " << m_Conf.getPhaMetFilter() << endl;
		cout << " FileNameSig 1    = " << m_Conf.getFileNameSig(1) << endl;
		cout << " FileNameSig 2    = " << m_Conf.getFileNameSig(2) << endl;
		cout << " FileNameSig 3    = " << m_Conf.getFileNameSig(3) << endl;
		cout << " FileNameTDI      = " << m_Conf.getFileNameTDI() << endl;
		cout << " FileNameDelays   = " << m_Conf.getFileNameDelays() << endl;
		cout << " FileNamePostions = " << m_Conf.getFileNamePositions() << endl;
		cout << " FileEncodingSig1 = " << m_Conf.getFileEncodingSig(1) << endl;
		cout << " FileEncodingSig2 = " << m_Conf.getFileEncodingSig(2) << endl;
		cout << " FileEncodingSig3 = " << m_Conf.getFileEncodingSig(3) << endl;
		cout << " FileEncodingTDI  = " << m_Conf.getFileEncodingTDI() << endl;
		cout << " FileEncodingDel  = " << m_Conf.getFileEncodingDelays() << endl;
		cout << " FileEncodingPos  = " << m_Conf.getFileEncodingPositions() << endl;
		cout << " NbGenTDI         = " << m_Conf.NbGenTDI() << endl;
		cout << " NbMaxDelays      = " << m_Conf.getNbMaxDelays() << endl;
		cout <<" TDI Generators :" << endl;
		for(int i=0; i<m_Conf.NbGenTDI(); i++){
			cout << "   " << m_Conf.getNameGenTDI(i) << " :";
			tmpTDIPack = m_Conf.getGenTDIPacks(i);
			for(int j=0; j<tmpTDIPack.size(); j++)
				cout << " " << tmpTDIPack[j];
			cout << endl;
		}
		
		cout << endl;
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestConfigSim: error: " << e.what()<<endl;
		cerr << "TestConfigSim: abort!" << endl;
		exit(1);
	}
	return(0);
}

// end of LISACODE-TestConfigSim.cpp
