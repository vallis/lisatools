// $Id: LISACODE-MemoryReadDisk.h,v 1.6 2007/04/05 09:32:34 ruiloba Exp $
/*
 *  MemoryReadDisk.h
 *  LISACode
 *
 *
 * Description : DERIV CLASS OF Memory
 * ----------
 * Cette classe (module) lit les donnees dans un fichier et les stocke sur un certain temps.
 *
 *
 *  Created on 01/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 01/11/05 by Antoine PETITEAU (APC)
 *
 */

#ifndef __MEMORYREADDISK_H
#define __MEMORYREADDISK_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "LISACODE-PhysicConstants.h"
#include "LISACODE-Serie.h"
#include "LISACODE-Memory.h"
using namespace std;
/*!\ingroup mem*/
/*!\brief Class to manage disk reading.
 *
 */
class MemoryReadDisk : public Memory
{
 protected:
/*!\brief File name on which data are read*/
    char * NomFichMem; // Name of recording file
/*!\brief File object managing reading file #NomFichMem */
	ifstream FichMem; // File where datas are stored
/*!\brief Data read from file. */
	vector< vector<double> > ReadData; // Data readed in file
/*!\brief Titles of read series.  */
	vector<string> TitlesReadData; // List of title of readed series
/*!\brief Index in read data. */
	vector<int> IndexInReadData; // 
	  
 public:
  /* Constructor */
  MemoryReadDisk();
  MemoryReadDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n); 
  ~MemoryReadDisk();

  /* Access methods */
  double gettMax();
  
  /*  Others methods */
  void AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName); // Add serie.
  void MakeTitles(char *  FileNameHead = " "); // 
  void ReceiveData(int SerieNumber, double Data); // Receive data
  void RecordAccData(double tStep, double t); // Record received datas (make it between each step of time)
};

#endif // __MEMORYREADDISK_H   

// end of LISACODE-MemoryReadDisk.h
