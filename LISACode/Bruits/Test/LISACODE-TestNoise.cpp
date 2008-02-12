// $Id: LISACODE-TestNoise.cpp,v 1.2 2007/02/19 10:29:40 ruiloba Exp $
/*
 *  LISACODE-TestNoise.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 14/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <math.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-Noise.h"
#include "LISACODE-NoiseWhite.h"
#include "LISACODE-NoiseFilter.h"
#include "LISACODE-NoiseFile.h"
#include "LISACODE-NoiseFShape.h"
#include "LISACODE-NoiseOof.h"
#include "LISACODE-NoiseTwoFilter.h"
//#include "Memory.h"

using namespace std;


int main(int argc, char *argv[])
{
	try {
		
		cout << endl << "   ************************* ";
		cout << endl << "   *                       * ";
		cout << endl << "   *       LISACode        * ";
		cout << endl << "   *   -----------------   * ";
		cout << endl << "   *      Test Noise       * ";
		cout << endl << "   *                       * ";
		cout << endl << "   ************************* " << endl << endl;
		
		
		double tStepPhy(5.0), tStepMes(5.0), tFirst(10.0), tLast(-30.0);
		double t(0.0), t_inter(0.0), tMax(1000000.0); //tMax(tStepPhy*pow(2,17));
		Noise * p;
		
		double SqPSD(30);
		cout << tMax << endl;
		double D_0(L0_m_default/c_SI);
		ofstream RecordNoise;
		RecordNoise.open("NoiseFileTest.txt");
		
		// Initialisation du generateur aleatoire
		//srand(123456);
		//srand((unsigned)time(NULL));
		time_t tc;
		long is1, is2;
		tc = time(NULL); // Temps : mot de 32 bits
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
		setall(is2,is1);
		
		
		cout << " Delay = " << D_0 << endl << endl;
		
		cout << "Creation of noise ..." << endl;
		vector< vector<double> > alpha(0), beta(0);
		vector<double> TmpVect(0);
		double Fact(0.0);
		int NbDataStab(0);
		int NFPowP, NFPowN;
		double * FactF;
		
		//Filtre pour les accelerometres a PSD = 1.59e-24 f-1 
		/*
		NbDataStab = 1000;
		TmpVect.resize(0);
		TmpVect.push_back(1);
		alpha.push_back(TmpVect); 
		TmpVect.resize(0);
		TmpVect.push_back(5e-12);
		TmpVect.push_back(5e-12);
		beta.push_back(TmpVect);
		*/
		NbDataStab = 1000;
		TmpVect.resize(0);
		TmpVect.push_back(-1.0);
		alpha.push_back(TmpVect); 
		TmpVect.resize(0);
		TmpVect.push_back(1.0 / (M_PI * tStepPhy));
		TmpVect.push_back(-1.0 / (M_PI * tStepPhy));
		beta.push_back(TmpVect);
		
		 
		// Test de filtre en a_-NFm f^-NFm + ... + a_-1 f^-1 + a_0 + a_1 f + a_2 f^2 + ... + a_NFp f^NFp
		NFPowP = 0;
		NFPowN = 2;
		FactF = (double*)malloc((NFPowN+NFPowP+1)*sizeof(double));
		FactF[0] = 1e-4 * 3.0e-15/(2.0*M_PI*c_SI);
		FactF[1] = 3.0e-15/(2.0*M_PI*c_SI);
		FactF[2] = 0.0;
		
		//p = new Noise(tStepPhy, tStepMes, tFirst, tLast);
		//p = new NoiseWhite(tStepPhy, tStepMes, tFirst, tLast, SqPSD);
		//p = new NoiseFilter(tStepPhy, tStepMes, tFirst, tLast, alpha, beta, NbDataStab);
		//p = new NoiseFile(tStepPhy, tStepMes, tFirst, tLast, "Noise1.txt");
		//p = new NoiseFShape(tStepPhy, tStepMes, tFirst, tLast, NFPowP, NFPowN, FactF);
		p = new NoiseOof(tStepPhy, tStepMes, tFirst, tLast, 1.0/tMax, 1.0/(2.0*tStepPhy), -3.0, 10000);
		cout << "Creation of noise --> OK !" << endl;
		
		//RecordNoise << "#Time t Noise(t) t-D Noise(t-D)" << endl;
		RecordNoise << "# Time  : Step = " << tStepPhy << " , Duration = " << tMax << " ;" << endl;
		RecordNoise << "#t Noise(t)" << endl;
		
		cout << "Execution ..." << endl;
		RecordNoise.precision(15);
		do{
			t += tStepPhy;
			p->addNoise();
			//cout << "***********************************************************************************" << endl;
			//cout << t << " " << p.getNoise(0.0) << " " << endl;
			//cout << "***********************************************************************************" << endl;
			//RecordNoise << t << " " << p.getNoise(0.0) << endl;
			RecordNoise << t << " " << p->getNoise(0.0) << endl;
			/*			t_inter = -1.0*tStepMes;
			do{.
				t_inter += tStepPhy;
				//RecordNoise << t << " ";
				RecordNoise << t+t_inter << " " << p.getNoise(t_inter) << " ";
				//RecordNoise << t-D_0 << " " << p.getNoise(t_inter-D_0);
				RecordNoise << endl;
			}while((t_inter< 0.0)&&(t+t_inter<tMax));*/
		}while(t<tMax); 
		
		cout << "  Final time = " << t << endl;
		cout << "Execution --> OK !" << endl;
		
		RecordNoise.close();
		cout << "End" << endl;
	}
	
	catch(exception & e ) {
		cerr << "TestNoise: error: " << e.what()<<endl;
		cerr << "TestNoise: abort!" << endl;
		exit(1);
	}
	return(0);
}

// end of LISACODE-TestNoise.cpp
