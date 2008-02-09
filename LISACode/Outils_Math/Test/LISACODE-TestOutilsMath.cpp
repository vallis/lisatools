// $Id: LISACODE-TestOutilsMath.cpp,v 1.2 2007/02/19 09:50:28 ruiloba Exp $
/*
 *  LISACODE-TestOutilsMath.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 04/01/2006 by  A.Petiteau, T.Regimbau
 *  Last modification on 04/01/2006 by A.Petiteau, T.Regimbau
 *
 */


#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-Couple.h"
#include "LISACODE-Filter.h"
#include "LISACODE-Mat.h"
#include "LISACODE-Vect.h"
#include "LISACODE-Serie.h"
#include "time.h"
#include "randlib.h"

using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   ********************************";
		cout << endl << "   *                              *";
		cout << endl << "   *   Test Mathematical Tools    *"; 
		cout << endl << "   *   -----------------------    *";
		cout << endl << "   *                              *";
		cout << endl << "   ********************************" << endl << endl; 
		
		

		cout << "Test couple :" << endl;
		cout << "----------- :" << endl;
		Couple c1(12.2, 3.6e-6);
		Couple c2(-2.3, 2.364e10);
		Couple ctmp;
		cout << " - c1 = " << c1.x << "  " << c1.y << endl;
		cout << " - c2 = " << c2.x << "  " << c2.y << endl;
		ctmp = c1+c2;
		cout << " --> c1+c2 = " << ctmp.x << "  " << ctmp.y << endl;
		ctmp = c1-c2;
		cout << " --> c1-c2 = " << ctmp.x << "  " << ctmp.y << endl;
		ctmp = 6.2*c2;
		cout << " --> 6.2*c2 = " << ctmp.x << "  " << ctmp.y << endl;
		ctmp = c1/5.1;
		cout << " --> c1/5.1 = " << ctmp.x << "  " << ctmp.y << endl;		
		
		cout << "Test Random :" << endl;
		cout << "----------- :" << endl;
		
		cout << "Taille time_t = " << sizeof(time_t) << endl;
		cout << "Taille long = " << sizeof(long) << endl;
		cout << "Taille int = " << sizeof(int) << endl;
		
		int i;
		float rmin, rmax; 
		rmin = 0.0;
		rmax = 1.0;
		
		// Initialisation du generateur aleatoire
		time_t tc;
		long is1, is2;
		tc = time(NULL); // Temps : mot de 32 bits
		is1 = is2 = 0; // Tous les bits a 0
		/* On fait :
			tc  =bit= b[n-1]b[n-2]...b1b0
			is1 =bit= b0b2...b[n/2-1]
			is2 =bit= b1b3...b[n/2]
		*/
		for (i=0;i<16;i++) {
			is1 |= (tc&1)<<(16-i); // On prend le bit de poids le plus faible que l'on met a la correspondance sur le bit de poid le plus fort en 16 bits !
			tc>>=1; // Decale de 1 bit = divise par 2
			is2 |= (tc&1)<<(16-i); // idem
			tc>>=1; // Redecalage de 1 bit = divise par 2
		}
		setall(is2,is1);
		
		cout << "is1 = " << is1 << " is2 = " << is2 << endl;
		ofstream OutFile;
		OutFile.open("RandomOut.txt");
		cout << " " << genunf(rmin,rmax) << endl;
		for(int i=0; i<10000; i++){
			OutFile << i << " " << genunf(rmin,rmax) << endl;
		}
		
		
		cout << endl;
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestGeometry: error: " << e.what()<<endl;
		cerr << "TestGeometry: abort!" << endl;
		exit(1);
	}
	return(0);
}
