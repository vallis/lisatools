// $Id: LISACODE-Memory.cpp,v 1.5 2007/04/05 09:33:15 ruiloba Exp $
/*
 *  LISACODE-Memory.cpp
 *  LISACode
 *
 *
 *  Created on 19/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 05/07/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-Memory.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg	#tStoreData = 200.0
 * \arg	#tStepRecord = 0.01
 */
Memory::Memory()
{
	tStoreData = 200.0;
	tStepRecord = 1.0e-2;
}


/*! \brief Constructs an instance and initializes it with \a tStoreData_n and \a tStepRecord_n inputs.
 *
 * \arg	#tStoreData =\a tStoreData_n
 * \arg	#tStepRecord =\a tStepRecord_n
 */
Memory::Memory(double tStoreData_n, double tStepRecord_n)
{
	tStoreData = tStoreData_n;
	tStepRecord = tStepRecord_n;
}


/*! \brief Destructor */
Memory::~Memory()
{
	
}


/* Access methods */
/*! \brief Sets #tStoreData attribute using \a tStoreData_n input.
 *
 * \a tStoreData_n input is checked : it is expected to be positive or null.
 */
void Memory::settStoreData(double tStoreData_n)
{
	if ( tStoreData_n < 0.0 )
		throw invalid_argument("Memory::settStoreData : The memorization time must be superior 0 !");
	tStoreData = tStoreData_n;
	for(int iS=0; iS<ListTmpData.size(); iS++)
		ListTmpData[iS].setNmax((int)(floor(tStoreData/tStepRecord)));
}

/*! \brief Sets #tStepRecord attribute using \a tStepRecord_n input.
 *
 * \a tStepRecord_n input is checked : it is expected to be positive or null.
 */
void Memory::settStepRecord(double tStepRecord_n)
{
	if ( tStepRecord_n < 0.0 )
		throw invalid_argument("Memory::settStepRecord : The measurement's time must be superior 0 !");
	tStepRecord = tStepRecord_n;
}

/*! \brief Gets maximum time, #tStoreData attribute
 */
double Memory::gettMax()
{
	return(tStoreData);
}

/* Others methods */

/*! \brief Adds series in #ListTmpData and updates #AlreadyRecDat set to false for added series. 
 *
 *
 * New series are added to #ListTmpData while #ListTmpData size is lower than SerieNumber input.\n
 * Corresponding FALSE flags are added into #AlreadyRecDat vector.\n
 * Additionnal series are initialized with 0.0 start value and #tStepRecord time step.
 * Only \a SerieNumber input is used (\a TypeName,\a IndirectDirName, \a iSCName are unused).\n
 * Virtual unused method.
 */
void Memory::AddSerieData(int SerieNumber, char * TypeName, int IndirectDirName, int iSCName)
{
	//Add series if there is not enough of it
	while(SerieNumber >= int(ListTmpData.size())){
		ListTmpData.push_back(Serie2(0.0, tStepRecord,(int)(floor(tStoreData/tStepRecord)))); //All series have the same time step
		AlreadyRecDat.push_back(false);
	}
	
}


/*! \brief Empty method */
void Memory::MakeTitles(char * FileNameHead)
{
	
}


/*! \brief Sets first value of #ListTmpData[SerieNumber] to \a data input value and sets #AlreadyRecDat[SerieNumber] to TRUE.
 *
 * SerieNumber input is checked : it must be positive or null, and lower than #ListTmpData size
 *
 */
void Memory::ReceiveData(int SerieNumber, double data)
{
	if((SerieNumber<0)||(SerieNumber>=ListTmpData.size()))
		throw invalid_argument("Memory::ReceiveData : This serie's number does not exist !");
	if(AlreadyRecDat[SerieNumber]){
		ListTmpData[SerieNumber].setBinValue(0,data);
	}else{
		ListTmpData[SerieNumber].addData(data);
		AlreadyRecDat[SerieNumber] = true;
	}
}


/*! \brief Records received data (make it between each step of time)
 *
 * \a tStep and \a t inputs are unused.\n
 * #AlreadyRecDat attributes are checked : it must be TRUE (for all series).\n
 * Last data of #ListTmpData are deleted for all series.\n
 * Virtual unused method.
 */
void Memory::RecordAccData(double tStep, double t)
{
	// Control that all the series are filled before recording
	for(int i=0; i< int(AlreadyRecDat.size()); i++){
		if (AlreadyRecDat[i] == false)
			throw invalid_argument("Memory: There is no data for one serie !");
		AlreadyRecDat[i] = false;
	}
	
	//for(int i=0; i< int(ListTmpData.size()); i++){
	//	ListTmpData[i].delLastData(tStoreData);
	//}
}



/*! \brief Returns the data of specified serie (\a SerieNumber) delayed by \a tDelay delay.
 *
 * SerieNumber input is checked : it must be positive or null, and lower than #ListTmpData size
 *
 */
double Memory::gData(int SerieNumber, double tDelay)
{
	try{/*
	 cout << endl;
	 cout << " tDelay = " << tDelay << endl;
	 cout << " ListTmpData[" << SerieNumber << "] :" << endl;
	 cout << "   - NbVal = " << ListTmpData[SerieNumber].getNbVal() << endl;
	 cout << "   - 0 : Ref = " << ListTmpData[SerieNumber].getRef(0);
	 cout << " , Val = " << ListTmpData[SerieNumber].getBinValue(0) << endl;
	 cout << "   - end : Ref = " << ListTmpData[SerieNumber].getRef(ListTmpData[SerieNumber].getNbVal()-1);
	 cout << " , Val = " << ListTmpData[SerieNumber].getBinValue(ListTmpData[SerieNumber].getNbVal()-1) << endl;
	 cout << endl;
	 */
		if((SerieNumber<0)||(SerieNumber>=ListTmpData.size()))
			throw invalid_argument("Memory: This serie's number does not exist !");
		return(ListTmpData[SerieNumber].gData(tDelay, LAG, 6));
	}
	catch(exception & e ) {
		cerr << "Memory::gData : error: " << e.what()<<endl;
		throw;
	}
}

/*! \brief Returns the data of specified serie (\a SerieNumber), delayed by \a tDelay  and interpolated using interpolation parameters (\a InterpolType, \a InterpUtilValue).
 *
 * SerieNumber input is checked : it must be positive or null, and lower than #ListTmpData size.\n
 * Calls Serie::gData with \a tDelay, \a InterpolType and \a InterpUtilValue inputs.
 *
 */
double Memory::gData(int SerieNumber, double tDelay, INTERP InterpolType, double InterpUtilValue)
{
	try{/*
	 cout << endl;
	 cout << " tDelay = " << tDelay << endl;
	 cout << " ListTmpData[" << SerieNumber << "] :" << endl;
	 cout << "   - NbVal = " << ListTmpData[SerieNumber].getNbVal() << endl;
	 cout << "   - 0 : Ref = " << ListTmpData[SerieNumber].getRef(0);
	 cout << " , Val = " << ListTmpData[SerieNumber].getBinValue(0) << endl;
	 cout << "   - end : Ref = " << ListTmpData[SerieNumber].getRef(ListTmpData[SerieNumber].getNbVal()-1);
	 cout << " , Val = " << ListTmpData[SerieNumber].getBinValue(ListTmpData[SerieNumber].getNbVal()-1) << endl;
	 cout << endl;
	 */
		if((SerieNumber<0)||(SerieNumber>=ListTmpData.size()))
			throw invalid_argument("Memory: This serie's number does not exist !");
		return(ListTmpData[SerieNumber].gData(tDelay, InterpolType, InterpUtilValue));
	}
	catch(exception & e ) {
		cerr << "Memory::gData : error: " << e.what()<<endl;
		throw;
	}
}


/*! \brief Returns 0 if there are enough stored data to use them.
 *
 * \return 0 if \a tSinceFirstReception input is greater than #tStoreData attribute , else 1.
 */
int Memory::unusable(double tSinceFirstReception) const
{
	if (tSinceFirstReception > tStoreData)
		return(0);
	else
		return(1);
}

// end of LISACODE-Memory.cpp
