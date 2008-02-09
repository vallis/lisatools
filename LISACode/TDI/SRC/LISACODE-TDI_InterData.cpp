// $Id: LISACODE-TDI_InterData.cpp,v 1.5 2007/04/05 09:33:25 ruiloba Exp $
/*
 *  TDI_InterData.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 13/09/05 by Antoine PETITEAU (APC)
 *  Last modification on 13/09/05 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-TDI_InterData.h"

/* Constructor */
/*! \brief Constructs an instance and initializes it with default values.
 *
 * Attributes are :
 * \arg #TDelay = NULL
 * \arg #PDPMMem = NULL
 * \arg #TimeStore = 30.0
 * \arg #tShift = 0.0
 * \arg #Eta = 6 elements initialzed with #tShift and 1.0 arguments
 * \arg #Usable = false
 * \arg #InterpType = LAG
 * \arg #InterpUtilValue = 6
 * \arg #NoNoise = false
 */
TDI_InterData::TDI_InterData()
{
	//double tShift_add(0.0);
	TDelay = new Memory();
	PDPMMem = new vector<Memory *>;
	TimeStore = 30.0;
	tShift = 0.0;
	for(int i=0; i<6; i++)
		Eta.push_back(Serie(tShift, 1.0));
	Usable = false;
	InterpType = LAG;
	InterpUtilValue = 6;
	tShift = 0.0;
	NoNoise = false; 
}


/*! \brief Constructs an instance and initializes it with TDelay_n and PDPMMem_n inputs and default values.
 *
 * Attributes are :
 * \arg #TDelay = TDelay_n
 * \arg #PDPMMem = NULL
 * \arg #TimeStore = 30.0
 * \arg #tShift = 0.0
 * \arg #Eta = 6 elements initialzed with #tShift and 1.0 arguments
 * \arg #Usable = false
 * \arg #InterpType = LAG
 * \arg #InterpUtilValue = 6
 * \arg #NoNoise = false
 */
TDI_InterData::TDI_InterData( Memory * TDelay_n,
							  vector<Memory *> * PDPMMem_n)
{
	//double tShift_add(0.0);
	TDelay = TDelay_n;
	PDPMMem = PDPMMem_n;
	TimeStore = 30.0;
	tShift = 0.0;
	for(int i=0; i<6; i++)
		Eta.push_back(Serie(tShift, (*PDPMMem)[1]->gettStepRecord()));
	Usable = false;
	InterpType = LAG;
	InterpUtilValue = 6;
	tShift = 0.0;
	NoNoise = false; 
}


/*! \brief Constructs an instance and initializes it with TDelay_n and PDPMMem_n inputs and default values.
 *
 * Attributes are :
 * \arg #TDelay = TDelay_n
 * \arg #PDPMMem = PDPMMem_n
 * \arg #TimeStore = TimeStore_n (checked; expected positive or null value)
 * \arg #tShift = tShift_n
 * \arg #Eta = 6 elements initialzed with #tShift and step (from #PDPMMem)  arguments
 * \arg #Usable = false
 * \arg #InterpType = InterpUtilValue_n
 * \arg #InterpUtilValue = 6
 * \arg #NoNoise = false
 * 
 */
TDI_InterData::TDI_InterData( Memory * TDelay_n,
							  vector<Memory *> * PDPMMem_n,
							  double TimeStore_n,
							  double tShift_n,
							  bool NoNoise_n,
							  INTERP InterpType_n,
							  double InterpUtilValue_n)
{
	//double tShift_add(0.0);
	TDelay = TDelay_n;
	PDPMMem = PDPMMem_n;
	InterpType = InterpType_n;
	InterpUtilValue = InterpUtilValue_n;
	tShift = tShift_n;
	NoNoise = NoNoise_n;
	if(TimeStore_n < 0)
		throw invalid_argument("TDI_InterData::setTimeStored : The stored time must be superior than 0 !");
	TimeStore = TimeStore_n;
	for(int i=0; i<6; i++)
		Eta.push_back(Serie(tShift, (*PDPMMem)[1]->gettStepRecord()));
	Usable = false;
	
}



/*! \brief Desctrutor.
 */
TDI_InterData::~TDI_InterData()
{

}


/* Methodes d'acces */
/*! \brief Sets #TimeStore attribute using TimeStore_n input.
 * 
 * TimeStore_n input is checked : it is expected to be positive or null.
 */
void TDI_InterData::setTimeStore(double TimeStore_n)
{
	if(TimeStore_n < 0)
		throw invalid_argument("TDI_InterData::setTimeStored : The stored time must be superior than 0 !");
	TimeStore = TimeStore_n;
}


/* Autres methodes */

/*! \brief Computes Eta using #TimeStore and #NoNoise attributes.
 * 
 * for all spacecrafts (index iSC=1,2,3) :
 * \arg if there is no noise
 * \f[ Eta[iSC-1].addData((*PDPMMem)[iSC-1]->gData( 0, tShift, InterpType, InterpUtilValue )) \f]
 * \f[ Eta[iSC+2].addData((*PDPMMem)[iSC-1]->gData( 1, tShift, InterpType, InterpUtilValue )) \f]
 * using Memory::gData method
 * \arg else
 * the following value is added at the begining of Eta[iSC-1] serie, using Serie::addData method 
 * \f[ (*PDPMMem)[iSC-1]->gData( 0, tShift, InterpType, InterpUtilValue ) \f] 
 * \f[ - \frac{1}{2} \cdot (*PDPMMem)[mod(iSC+1,3)]->gData( 2, tShift + TDelay->gData(mod(iSC+2,3), tShift), \f] 
 * \f[ InterpType, InterpUtilValue ) \f]
 * \f[ - \frac{1}{2} \cdot (*PDPMMem)[mod(iSC+1,3)]->gData( 3, tShift + TDelay->gData(mod(iSC+2,3), tShift), \f] 
 * \f[ InterpType, InterpUtilValue ) \f]
 * and the following value is added at the begining of Eta[iSC+2] serie, using Serie::addData method 
 * \f[ (*PDPMMem)[iSC-1]->gData( 1, tShift, InterpType, InterpUtilValue ) \f]  
 * \f[ + \frac{1}{2} \cdot (*PDPMMem)[iSC-1]->gData( 2, tShift, InterpType, InterpUtilValue ) \f]
 * \f[ - \frac{1}{2} \cdot (*PDPMMem)[iSC-1]->gData( 3, tShift, InterpType, InterpUtilValue ) \f]
 *
 * Last data of series Eta[iSC+2] and Eta[iSC-1], while x reference is greater than TimeStore input, using Serie::delLastData method.
 */
void TDI_InterData::ComputeEta()
{
	try{
		if(NoNoise){
			for(int iSC=1; iSC<=3; iSC++){
				int iSCpe1(iSC+1), iSCpe2(iSC+2);
				if (iSCpe1>3) iSCpe1 -= 3;
				if (iSCpe2>3) iSCpe2 -= 3;
				
				// ETA1 = S1
				Eta[iSC-1].addData((*PDPMMem)[iSC-1]->gData( 0, tShift, InterpType, InterpUtilValue ));
				Eta[iSC-1].delLastData(TimeStore);
				
				// ETAp1 = Sp1
				Eta[iSC+2].addData((*PDPMMem)[iSC-1]->gData( 1, tShift, InterpType, InterpUtilValue ));
				Eta[iSC+2].delLastData(TimeStore);
			}
		}else{
		for(int iSC=1; iSC<=3; iSC++){
			int iSCpe1(iSC+1), iSCpe2(iSC+2);
			if (iSCpe1>3) iSCpe1 -= 3;
			if (iSCpe2>3) iSCpe2 -= 3;
 
			// ETA1 = S1 - 1/2 (D3 TAU2 - D3 TAUp2)
			Eta[iSC-1].addData( (*PDPMMem)[iSC-1]->gData( 0, tShift, InterpType, InterpUtilValue ) 
								- 0.5*( (*PDPMMem)[iSCpe1-1]->gData( 2, tShift + TDelay->gData(iSCpe2-1, tShift), InterpType, InterpUtilValue )
								- (*PDPMMem)[iSCpe1-1]->gData( 3, tShift + TDelay->gData(iSCpe2-1, tShift), InterpType, InterpUtilValue ) ) );
			Eta[iSC-1].delLastData(TimeStore);
		
			// ETAp1 = Sp1 - 1/2 (TAU1 - TAUp1)
			Eta[iSC+2].addData( (*PDPMMem)[iSC-1]->gData( 1, tShift, InterpType, InterpUtilValue )  
								+ 0.5*((*PDPMMem)[iSC-1]->gData( 2, tShift, InterpType, InterpUtilValue )
								- (*PDPMMem)[iSC-1]->gData( 3, tShift, InterpType, InterpUtilValue ) ) );
			Eta[iSC+2].delLastData(TimeStore);
		}
		}
		if(Usable == false){
			//cout << "ceil(TimeStore/Eta[0].getRefStep()) = " << ceil(TimeStore/Eta[0].getRefStep()) << endl;
			//cout << "Eta[0].getNbVal() = " << Eta[0].getNbVal() << endl;
			if(Eta[0].getNbVal()>=ceil(TimeStore/Eta[0].getRefStep()))
				Usable = true;
		}
		/*	for(int i=0; i<6; i++)
			cout << "Eta " << i << " : Nb data = " << Eta[i].getNbVal() << endl;
		cout << " ceil(TimeStore/Eta[0].getRefStep() = " << ceil(TimeStore/Eta[0].getRefStep()) << " ; Utilisable = " << Usable << endl;
		*/
	}
	catch(exception & e ) {
		cerr << "TDI_InterData::ComputeEta : error: " << e.what()<<endl;
		throw;
	}
}
	

/*! \brief Returns value interpolated for the delay (iSC=[1,3] and Indirect=[0,1]
 *
 * Inputs are checked :
 * \arg Spacecraft index iSC must be 1, 2 or 3
 * \arg  The travel direction IndirectDir must be 0 or 1
 *
 * Eta index is computed : \f$ iSC-1+2 \cdot IndirectDir \f$. \n
 * Interpolation for Delay reference is computed by Serie::gData method, using #InterpType and #InterpUtilValue attributes.
 */
double TDI_InterData::gData(int iSC, int IndirectDir, double Delay)
{
	try {
		if((iSC<1)||(iSC>3))
			throw invalid_argument("TDI_InterData::gData : Spacecraft index must be 1, 2 or 3 !");
		if ((IndirectDir < 0)||(IndirectDir > 1))
			throw invalid_argument("TDI_InterData::gData : The travel direction is 0 or 1 !");
		return(Eta[iSC-1+2*IndirectDir].gData(Delay, InterpType, InterpUtilValue ));
	}
	catch(exception & e ) {
		cerr << "TDI_InterData::gData : error: " << e.what()<<endl;
		throw;
	}
}

/*! \brief Returns value interpolated for the delay (iSerie=[1,6]).
 *
 * Input is checked :
 * \arg Serie index iSerie must be 1, 2, 3, 4, 5 or 6
 *
 * Eta index is computed : \f$ iSerie - 1 \f$. \n
 * Interpolation for Delay reference is computed by Serie::gData method, using #InterpType and #InterpUtilValue attributes
 */
double TDI_InterData::gData(int iSerie, double Delay)
{
	try{
		if((iSerie<1)||(iSerie>6))
			throw invalid_argument("TDI_InterData::gData : Serie index must be 1, 2, 3, 4, 5 or 6 !");
		//cout << "Eta" << iSerie-1 << "(a " << Delay << "s) sur [" << Delay-50 << " - " << Delay+50 << "] (de " << Eta[iSerie-1].getRef(0) << "s a " << Eta[iSerie-1].getRef(Eta[iSerie-1].getNbVal()-1) << "s)" << endl;  
		return(Eta[iSerie-1].gData(Delay, InterpType, InterpUtilValue ));
	}
	catch(exception & e ) {
		cerr << "TDI_InterData::gData : error: " << e.what()<<endl;
		throw;
	}
}



// end of TDI_InterData.cpp
