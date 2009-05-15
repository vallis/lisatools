// $Id: LISACODE-TestFilter.cpp,v 1.1 2007/02/19 09:50:25 ruiloba Exp $
/*
 *  LISACODE-TestFilter.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 *  
 *
 *
 *  Created on 21/12/2006 by  A.Petiteau
 *  Last modification on 21/12/2006 by A.Petiteau
 *
 */

#include <stdexcept>
#include <iostream>
#include <fstream.h>
#include "LISACODE-Filter.h"
#include "randlib.h"

using namespace std;


int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *********************";
		cout << endl << "   *                   *";
		cout << endl << "   *    Test Filter    *"; 
		cout << endl << "   *    -----------    *";
		cout << endl << "   * (LISACode v "<<LCVersion<<") *";
		cout << endl << "   *                   *";
		cout << endl << "   *********************" << endl << endl; 
		
		
		double tStepPhy(0.1);
		double tStepMes(1.0);
		double tMax(5000000.0*tStepPhy);
		
		int NbData((int)(tMax/tStepPhy));
		vector<double> RawData(NbData);
		vector<double> FilterData(NbData);
		ofstream OutDataFile;
		double r1(0.0), r2(0.0);
		
		for(int i=0; i<NbData; i++){
			RawData[i] = 0.0;
			FilterData[i] = 0.0;
		}
		
		cout << "Number of data = " << NbData << endl;
		
		// Creation of elliptic filter
		cout << "Creation of elliptic filter ..." << endl;
		Filter MyFilter(1.0/tStepPhy, 140.0, 0.1, 0.1/tStepMes,  0.3/tStepMes);
		
		// Generation of white bruit
		cout << "Generation of noise data ..." << endl;
		for(int i=0; i<NbData; i++){
			r1 = (double)genunf(0.0, 1.0);
			r2 = (double)genunf(0.0, 1.0);
			RawData[i] = (1.0/sqrt(2.0*tStepPhy))*sqrt(-2.0*log(r2))*cos(2*M_PI*r1);
		}
		
		cout << "Filtering noise data ..." << endl;
		// Filtering data
		MyFilter.App(NbData-1, RawData, FilterData);
		
		// Save result
		cout << "Save result ..." << endl;
		OutDataFile.open("TestFilterResult.txt");
		OutDataFile << "#Time RawData FilterData" << endl;
		for(int i=0; i<NbData; i++){
			OutDataFile << i*tStepPhy << " " << RawData[i] << " " << FilterData[i] << endl;
		}
		OutDataFile.close();
		
		cout << endl;
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestFilter: error: " << e.what()<<endl;
		cerr << "TestFilter: abort!" << endl;
		exit(1);
	}
	return(0);
}