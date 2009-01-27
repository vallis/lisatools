// $Id: LISACODE-MemoryWriteDisk.cpp,v 1.5 2007/04/05 09:33:15 ruiloba Exp $
/*
 *  LISACODE-MemoryWriteDisk.cpp
 *  LISACode
 *
 *
 *  Created on 01/11/05 by Antoine PETITEAU (APC)
 *  Last modification on 01/11/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-MemoryWriteDisk.h"

/* Constructor */

/*!\brief Constructs an empty instance */
MemoryWriteDisk::MemoryWriteDisk() : Memory()
{
	
}


/*!\brief Constructs an instance  and initializes it with default values and tStoreData_n, tStepRecord_n and NomFichMem_n inputs.
 *
 * #Memory constructor is called with tStoreData_n and tStepRecord_n inputs.
 *
 * Others attributes are set using data read in NomFichMem_n file :
 * \arg	#NomFichMem =  NomFichMem_n
 * \arg	#FichMem = manages data of \a NomFichMem file
 * \arg #SCSerie = empty
 * \arg #TitleSerie = empty
 */
MemoryWriteDisk::MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n) :
Memory(tStoreData_n, tStepRecord_n)
{
	NomFichMem =  NomFichMem_n;
	FEncoding = 0;
	FichMem.open(NomFichMem);
	SCSerie.resize(0);
	TitleSerie.resize(0);
	BinHeader = false;
}


/*!\brief Constructs an instance  and initializes it with default values and tStoreData_n, tStepRecord_n and NomFichMem_n inputs.
*
* #Memory constructor is called with tStoreData_n and tStepRecord_n inputs.
*
* Others attributes are set using data read in NomFichMem_n file :
* \arg	#NomFichMem =  NomFichMem_n
* \arg	#FichMem = manages data of \a NomFichMem file
* \arg #SCSerie = empty
* \arg #TitleSerie = empty
*/
MemoryWriteDisk::MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n) :
Memory(tStoreData_n, tStepRecord_n)
{
	NomFichMem =  NomFichMem_n;
	FEncoding = Encoding_n;
	if(FEncoding > 0){
		cout << "     Opening a BINARY file " << NomFichMem << "    Encoding = " << FEncoding <<endl ;
		FichMem.open(NomFichMem,ios::binary);
	}else{
		cout << "     Opening an ASCII file " <<NomFichMem << "    Encoding = " << FEncoding <<endl ;
		FichMem.open(NomFichMem);
	}
	SCSerie.resize(0);
	TitleSerie.resize(0);
	BinHeader = false;
}


/*!\brief Constructs an instance and initializes it with default values and tStoreData_n, tStepRecord_n and NomFichMem_n inputs.
 *
 * #Memory constructor is called with tStoreData_n and tStepRecord_n inputs.
 *
 * Others attributes are set using data read in NomFichMem_n file :
 * \arg	#NomFichMem =  NomFichMem_n
 * \arg	#FichMem = manages data of \a NomFichMem file
 * \arg #SCSerie = empty
 * \arg #TitleSerie = empty
 * \arg #BinHeader = BinHeader_n
 */
MemoryWriteDisk::MemoryWriteDisk(double tStoreData_n, double tStepRecord_n, char * NomFichMem_n, int Encoding_n, bool BinHeader_n, double TimeOffset_n, double TimeEnd_n) :
Memory(tStoreData_n, tStepRecord_n)
{
	NomFichMem =  NomFichMem_n;
	FEncoding = Encoding_n;
	if(FEncoding > 0){
		cout << "     Opening a BINARY file " << NomFichMem << "    Encoding = " << FEncoding <<endl ;
		FichMem.open(NomFichMem,ios::binary);
	}else{
		cout << "     Opening an ASCII file " <<NomFichMem << "    Encoding = " << FEncoding <<endl ;
		FichMem.open(NomFichMem);
	}
	SCSerie.resize(0);
	TitleSerie.resize(0);
	BinHeader = BinHeader_n;
	TimeOffset = TimeOffset_n;
	TimeEnd = TimeEnd_n;
}


/*!\brief Destructor */
MemoryWriteDisk::~MemoryWriteDisk()
{
	CloseFile();
}


/* Access methods */

/*! \brief Gets maximum time 
 *
 * returned value = #tStoreData attribute
 */
double MemoryWriteDisk::gettMax()
{
	return(tStoreData);
}

/* Others methods */

/*! \brief Adds series in #ListTmpData, updates corresponding elements in #AlreadyRecDat, fills #TitleSerie[SerieNumber] attribute and sets #SCSerie[SerieNumber].
 *
 * New series are added in #ListTmpData to have \a SerieNumber series in input #ListTmpData.  
 * Corresponding FALSE flags are added into #AlreadyRecDat attribute. \n 
 * Additionnal series are initialized with 0.0 start value and #tStepRecord time step. \n 
 * #TitleSerie[\a SerieNumber] is set using \a  TypeName and #SCSerie[\a SerieNumber]=\a iSCName.
 */
void MemoryWriteDisk::AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName)
{
	ostringstream tmpTitle;
	//Add series if there is not enough of it
	
	while(SerieNumber >= int(ListTmpData.size())){
		ListTmpData.push_back(Serie(0.0, tStepRecord)); //All series have the same time step
		TitleSerie.push_back(" ");
		SCSerie.push_back(0);
		AlreadyRecDat.push_back(false);
	}
	
	tmpTitle << " " << TypeName;
	if (IndirectDirName==1)
		tmpTitle << "p";
	TitleSerie[SerieNumber] = tmpTitle.str();
	
	SCSerie[SerieNumber] = iSCName;
}


/*! \brief Copies header from \a FileNameHead file to #FichMem.
 *
 * For all series (index i) in #TitleSerie, writes corresponding #TitleSerie[i] and #SCSerie[i] into #FichMem.
 *
 *
 *
 */
void MemoryWriteDisk::MakeTitles(char * FileNameHead)
{
	ifstream FileHead;
	char Buf[256] ;
	//FichMem << "# " <<  FileNameHead << " " << SCSerie[0] << endl;
	//FichMem << "# TimeStep : " << tStepRecord << endl;
	//FichMem << "# Columns : " << TitleSerie.size()+1 << endl;
	//FichMem << "#Time";
	
	
	// Copy of configuration file in header
	if(!FEncoding){
		FileHead.open(FileNameHead);
		if (!FileHead){
			throw invalid_argument("MemoryWriteDisk::MakeTitles : Can not open the configuration file ! ");
		}
		FichMem << "############## " << LCVersion <<  " ##############" << endl;
		FichMem << "############## Configuration : " << FileNameHead <<  " ##############" << endl;
		while(!FileHead.eof()) {
			FileHead.getline(Buf, 256);
			FichMem << "# " << Buf << endl;
		};
		FichMem << "############## End of configuration ##############" << endl;
		FileHead.close();
		
		
		// Write title
		FichMem << "#Time";
		for(int i=0; i< int(TitleSerie.size()); i++)
			FichMem << TitleSerie[i] << SCSerie[i];
		FichMem << endl;
		
		cout << "  Display all serie :" << endl ;
		for(int i=0; i< int(TitleSerie.size()); i++)
			cout << "   - Title of serie " << i << " is : " << TitleSerie[i] << SCSerie[i] << endl;
	}else{
		if(BinHeader){
			FichMem << "#TITLE t";
			for(int i=0; i<(int)(TitleSerie.size()); i++)
				FichMem << "," << TitleSerie[i] << SCSerie[i];
			FichMem << endl; 
			FichMem << "#RECORD " << (int)(ListTmpData.size())+1<< " " << (int)ceil((TimeEnd-TimeOffset)/tStepRecord+1) <<  endl;
			FichMem << "#TIME " << TimeOffset << " " << tStepRecord << " " << TimeEnd << endl;
		}
	}
		
}


/*! \brief Records received data in file #FichMem (make it between each step of time)
 *
 * #AlreadyRecDat attributes are checked : it must be TRUE (for all series). \n
 * #ListTmpData data are written into #FichMem (for all series) and last ones are removed by Serie::delLastData.\n
 * #AlreadyRecDat attributes are set to FALSE (for all series).
 *
 * \a tStep input is unused. Time \a t is written into #FichMem. \n
 */
void MemoryWriteDisk::RecordAccData(double tStep, double t)
{
	double tmpdata;
	
	// Control that all the series are filled before recording
	for(int i=0; i< int(AlreadyRecDat.size()); i++){
		if (AlreadyRecDat[i] == false)
			throw invalid_argument("MemoryWriteDisk: There is no data for one serie !");
		AlreadyRecDat[i] = false;
	}
	
	if(FEncoding){ // Write the datas in BINARY file
		FichMem.write((char*) &t, sizeof(double));
		for(int i=0; i< int(ListTmpData.size()); i++){
			tmpdata =  ListTmpData[i].getBinValue(0);
			FichMem.write((char*) &tmpdata, sizeof(double));
		}
	}else{ // Write the datas in ASCII file
		FichMem.precision(15);
		FichMem << t;
		for(int i=0; i< int(ListTmpData.size()); i++)
			FichMem << " " << ListTmpData[i].getBinValue(0) ;
		FichMem << endl;
	}
	// Delete last data
	for(int i=0; i< int(ListTmpData.size()); i++){
		ListTmpData[i].delLastData(tStoreData);
	}
}

/*! \brief Closes #FichMem
 */

void MemoryWriteDisk::CloseFile()
{
	if(FEncoding > 0){
		cout << " Closing BINARY file." << endl ;
	}else{
		cout << " Closing ASCII file." << endl ;
	}
	FichMem.flush();
	FichMem.close();
}

// end of LISACODE-MemoryWriteDisk.cpp
