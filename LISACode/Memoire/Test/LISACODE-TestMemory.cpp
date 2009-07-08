// $Id: LISACODE-TestMemory.cpp,v 1.2 2007/02/19 10:04:02 ruiloba Exp $
/*
 *  LISACODE-TestMemory.cpp
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
#include "LISACODE-Memory.h"
#include "LISACODE-MemoryReadDisk.h"
#include "LISACODE-MemoryWriteDisk.h"


using namespace std;



int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   *********************";
		cout << endl << "   *                   *";
		cout << endl << "   *    Test Memory    *"; 
		cout << endl << "   *    -----------    *";
		cout << endl << "   * (LISACode v "<<LCVersion<<") *";
		cout << endl << "   *                   *";
		cout << endl << "   *********************" << endl << endl; 
		
		
		double t(0.0), dt(1.0), tStored(30.0), tmax(100.0), tdelay(16.6);
		char * FileName ("OutTestMemory.txt");
		
		cout << "Test memory in write mode :" << endl;
		cout << "---------------------------" << endl;
		
		MemoryWriteDisk OutputData(tStored, dt, FileName);
		
		OutputData.AddSerieData(0,"tcarre_", 0, 1);
		OutputData.AddSerieData(1,"20t_", 1, 2);
		
		OutputData.MakeTitles();
		
		while((t<tStored)&&(t<tmax)){
			OutputData.ReceiveData(0,t*t);
			OutputData.ReceiveData(1,t*20.0);
			OutputData.RecordAccData(dt,t);
			t+=dt;
		};
		
		while(t<tmax){
			OutputData.ReceiveData(0,t*t);
			OutputData.ReceiveData(1,t*20.0);
			OutputData.RecordAccData(dt,t);
			cout << "  ";
			cout << "tcarre(" << t << ") = " << OutputData.gData(0,0.0) << "  ;   ";
			cout << "tcarre(" << t << "-" << tdelay << ") = " << OutputData.gData(0,tdelay) << "  ;   ";
			cout << "20t(" << t << ") = " << OutputData.gData(1,0.0) << "  ;   ";
			cout << "20t(" << t << "-" << tdelay << ") = " << OutputData.gData(1,tdelay) << "  ;   ";
			cout << endl;
			t+=dt;
		};
		
		OutputData.CloseFile();
		
		
		cout << "Test memory in read mode :" << endl;
		cout << "--------------------------" << endl;
		
		MemoryReadDisk InputData(tStored, dt, FileName);
		
		t = 0.0;
		InputData.AddSerieData(0,"tcarre_", 0, 1);
		InputData.AddSerieData(1,"20t_", 1, 2);
		
		
		while((t<tStored)&&(t<tmax)){
			InputData.RecordAccData(dt,t);
			t+=dt;
		};
		
		while(t<tmax){
			InputData.RecordAccData(dt,t);
			cout << "  ";
			cout << "tcarre(" << t << ") = " << InputData.gData(0,0.0) << "  ;   ";
			cout << "tcarre(" << t << "-" << tdelay << ") = " << InputData.gData(0,tdelay) << "  ;   ";
			cout << "20t(" << t << ") = " << InputData.gData(1,0.0) << "  ;   ";
			cout << "20t(" << t << "-" << tdelay << ") = " << InputData.gData(1,tdelay) << "  ;   ";
			cout << endl;
			t+=dt;
		};
		
		
		cout << endl;
		cout << "Fin" << endl;
	}
	
	
	catch(exception & e ) {
		cerr << "TestMemory: error: " << e.what()<<endl;
		cerr << "TestMemory: abort!" << endl;
		exit(1);
	}
	return(0);
}

// end of LISACODE-TestMemory.cpp
