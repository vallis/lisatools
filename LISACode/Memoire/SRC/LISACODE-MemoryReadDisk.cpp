// $Id: LISACODE-MemoryReadDisk.cpp,v 1.6 2007/04/05 09:33:15 ruiloba Exp $
/*
 *  LISACODE-Memory.cpp
 *  LISACode
 *
 *
 *  Created on 01/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 01/11/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-MemoryReadDisk.h"

/* Constructor */

/*!\brief Constructs an empty instance */
MemoryReadDisk::MemoryReadDisk() : Memory()
{
	
}


/*!\brief Constructs an instance  and initializes it with default values and tStoreData_n, tStepRecord_n and NomFichMem_n inputs.
 *
 * #Memory constructor is called with tStoreData_n and tStepRecord_n inputs.
 *
 * Others attributes are set using data read in NomFichMem_n file :
 * \arg	NbColumns is read in the first line
 * \arg	#TitlesReadData are read (NbColumns elements)
 * \arg	#ReadData are read (NbColumns elements)
*/

MemoryReadDisk::MemoryReadDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n) :
Memory(tStoreData_n, tStepRecord_n)
{
	string junk;
	//char junkline[256];
	int NbColumns(0), NbLinesHeader(0);
	double tmp_tStep(0.0);
	double tmp_data(0.0);
	double t0(0.0), t1(0.0);
	
	NomFichMem =  NomFichMem_n;
	cout << "    - Read " << NomFichMem << " ..." << endl;
	
	/****  Read the number of columns ****/
	FichMem.open(NomFichMem);
	if (!FichMem){
		throw invalid_argument("MemoryReadDisk::MemoryReadDisk : Error while opening file.");
	}
	// Read head file and count the number of line in header
	NbLinesHeader = 0;
	while(FichMem.peek() == '#'){
		FichMem.ignore(512,'\n');
		NbLinesHeader++;
	};
	// Read the first line
	NbColumns = 0;
	while((FichMem.peek() != '\n')&&(!FichMem.eof())){
		FichMem >> junk;
		NbColumns++;
	}
	FichMem.close();
	FichMem.clear();	
	if(NbColumns<2){
		throw invalid_argument("MemoryReadDisk::MemoryReadDisk : Not enough columns in file.");
	}
	ReadData.resize(NbColumns);
	TitlesReadData.resize(NbColumns);
	
	/****  Read the time step ****/
	FichMem.open(NomFichMem);
	if (!FichMem){
		throw invalid_argument("MemoryReadDisk::MemoryReadDisk : Error while opening file.");
	}
	// Read head file
	while(FichMem.peek() == '#'){
		FichMem.ignore(512,'\n');
	};
	// Read the two first line
	FichMem >> t0;
	FichMem.ignore(512,'\n');
	FichMem >> t1;
	FichMem.ignore(512,'\n');
	FichMem.close();
	FichMem.clear();
	tmp_tStep = fabs(t0-t1); 
	if(fabs(tmp_tStep-tStepRecord)/tStepRecord > 1.0e-6)
		throw invalid_argument("MemoryReadDisk::MemoryReadDisk : Time step in the readed file is different simulation time step !");

	/****  Read data ****/
	FichMem.open(NomFichMem);
	if (!FichMem){
		throw invalid_argument("MemoryReadDisk::MemoryReadDisk : Error while opening file.");
	}
	// Read head file less one line
	for(int i=0; i<NbLinesHeader-1; i++){
		FichMem.ignore(512,'\n');
	}
	// Read the title of each data series
	for(int i=0; i<NbColumns; i++){
		FichMem >> TitlesReadData[i];
		//cout << " Title " << i << " : " << TitlesReadData[i] << endl;
	}
		
	// Read and store the data series
	while(!FichMem.eof()){
		for(int i=0; i<NbColumns; i++){
			FichMem >> tmp_data;
			ReadData[i].insert(ReadData[i].begin(), tmp_data);
		}
	};
	FichMem.close();
}


/*!\brief Destructor */
MemoryReadDisk::~MemoryReadDisk()
{
	
}


/* Access methods */
/*! \brief Gets maximum time. 
 *
 * Maximum time is number of data read (#ReadData[0].size()-1) multiplied by #tStepRecord.
 */
double MemoryReadDisk::gettMax()
{
	return((ReadData[0].size()-1)*tStepRecord);
}

/* Others methods */

/*! \brief Adds series in #ListTmpData attribute, updates #AlreadyRecDat attribute, fills #IndexInReadData[SerieNumber] attribute.
 *
 * New series are added in #ListTmpData to have \a SerieNumber series in input #ListTmpData.  
 * Corresponding FALSE flags are added into #AlreadyRecDat attribute. \n 
 * Additionnal series are initialized with 0.0 start value and #tStepRecord time step. \n 
 * #IndexInReadData[SerieNumber] attribute is filled with index found by compairing #TitlesReadData[SerieNumber] 
 * to RequiredTitle build with \a IndirectDirName and \a iSCName inputs.
 */
void MemoryReadDisk::AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName)
{
	ostringstream tmpTitle;
	string RequiredTitle;
	
	//Add series if there is not enough of it
	while(SerieNumber >= int(ListTmpData.size())){
		ListTmpData.push_back(Serie(0.0, tStepRecord)); //All series have the same time step
		AlreadyRecDat.push_back(false);    
		IndexInReadData.push_back(0);
	}
	
	tmpTitle << TypeName;
	if (IndirectDirName==1)
		tmpTitle << "p";
	tmpTitle << iSCName;
	RequiredTitle = tmpTitle.str();
	
	IndexInReadData[SerieNumber] = 0 ;
	while((RequiredTitle!=TitlesReadData[IndexInReadData[SerieNumber]])&&(IndexInReadData[SerieNumber]<TitlesReadData.size())){
		// cout << " Compare : " << RequiredTitle << "  &  " << TitlesReadData[IndexInReadData[SerieNumber]] << endl;
		IndexInReadData[SerieNumber]++;
	};
	
	//cout << " MemoryReadDisk::AddSerieData : Serie " << SerieNumber << " -> "  << IndexInReadData[SerieNumber];
	//cout << " : " << RequiredTitle;
	//cout << " : NbData = " << ReadData[IndexInReadData[SerieNumber]].size() << endl;
}

/*! \brief Empty method */
void MemoryReadDisk::MakeTitles(char *  FileNameHead)
{
	
}




/*! \brief Records received data.
 *
 * tStep and t inputs are unused.\n
 * #AlreadyRecDat attributes are checked : it must be TRUE (for all series).\n
 * #ListTmpData last data are removed (for all series).\n
 * #AlreadyRecDat attributes are set to FALSE (for all series).
 */
void MemoryReadDisk::RecordAccData(double tStep, double t)
{
	//cout << " ListTmpData.size() = " << ListTmpData.size() << endl;
	for(int i=0; i< int(ListTmpData.size()); i++){
		ListTmpData[i].addData((ReadData[IndexInReadData[i]])[ReadData[IndexInReadData[i]].size()-1]);
		ReadData[IndexInReadData[i]].pop_back();
	}
	
	for(int i=0; i< int(ListTmpData.size()); i++){
		ListTmpData[i].delLastData(tStoreData);
		//cout << " ListTmpData[i].getNbVal() = " << ListTmpData[i].getNbVal();
		//cout << " , ReadData[IndexInReadData[i]].size() " << ReadData[IndexInReadData[i]].size() << endl;
	}
	
}


// end of LISACODE-MemoryReadDisk.cpp
