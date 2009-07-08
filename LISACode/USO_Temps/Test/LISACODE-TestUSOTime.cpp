// $Id: LISACODE-TestUSOTime.cpp,v 1.2 2007/02/19 09:26:55 ruiloba Exp $
/*
 *  LISACODE-TestUSOTime.cpp
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
#include <fstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-LISAConstants.h"
#include "LISACODE-MathUtils.h"
#include "LISACODE-Couple.h"
#include "LISACODE-Filter.h"
#include "LISACODE-Mat.h"
#include "LISACODE-Vect.h"
#include "LISACODE-Serie.h"
#include "LISACODE-USOClock.h"

using namespace std;



int main (int argc, char * const argv[])
{
	try {		
		
		cout << endl << "   *********************";
		cout << endl << "   *                   *";
		cout << endl << "   *   Test USO Time   *"; 
		cout << endl << "   *   -------------   *";
		cout << endl << "   *                   *";
		cout << endl << "   *********************" << endl << endl; 
		
		
		double t(0.0), dt(1.0);
		USOClock m_USO(0.01, 1.0e-9, 1.0e-12);
		
		cout << " USO Clock Offset = " << m_USO.getOffset() << endl;
		cout << " USO Clock Deriv  = " << m_USO.getDeriv() << endl;
		cout << " USO Clock Noise  = " << m_USO.getNoise() << endl;
		cout << endl;
		cout << "Time";
		cout << "USOGap";
		cout << "USOTime";
		cout << endl;
		cout.precision(15);
		for(int i=0; i<100; i++){
			cout << t << " " << m_USO.gGap(t,dt) << " " << m_USO.gTime(t,dt) << endl;	
			t += dt;
		}
		
		cout << endl;
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestUSOTime: error: " << e.what()<<endl;
		cerr << "TestUSOTime: abort!" << endl;
		exit(1);
	}
	return(0);
}

// end of LISACODE-TestUSOTime.cpp