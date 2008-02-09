// $Id: LISACODE-TDI.cpp,v 1.5 2007/04/05 09:33:25 ruiloba Exp $
/*
 *  TDI.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 30/06/05 by Antoine PETITEAU (APC)
 *  Last modification on 30/06/05 by Antoine PETITEAU (APC)
 *
 */


#include "LISACODE-TDI.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with zero value for all attributes.
 *
 * Attributes are :
 * \arg #TDelay = empty
 * \arg	#Eta = empty
 * \arg	#OutFile = "DefTDIGen.txt" opened file
 * \arg	#iSerie = 0
 * \arg	#Sign = empty
 * \arg	#IndexEta = empty
 * \arg	#IndexDelay = empty
 * \arg	#TDIQuickMod = empty
 */
TDI::TDI()
{
	TDelay = new Memory;
	Eta = new TDI_InterData;
	OutFile->open("DefTDIGen.txt");
	OutFileEncoding = 0;
	iSerie = 0;
	Sign.resize(0);
	IndexEta.resize(0);
	IndexDelay.resize(0);
	//LastUnit.resize(0);
	//InterpType = LAG;
	//InterpUtilValue = 6;
	TDIQuickMod = new TDITools;
}



/*! \brief Constructs an instance and initializes it using TDelay_n, Eta_n, ofstream and iSerie_n inputs.
 *
 * Attributes are :
 * \arg #TDelay = TDelay_n
 * \arg	#Eta = Eta_n
 * \arg	#OutFile = OutFile_n
 * \arg	#iSerie = iSerie_n
 * \arg	#Sign = empty
 * \arg	#IndexEta = empty
 * \arg	#IndexDelay = empty
 * \arg	#TDIQuickMod = empty
 */
TDI::TDI(   Memory * TDelay_n,
			TDI_InterData * Eta_n,
			ofstream * OutFile_n,
			int OutFileEncoding_n,
			int iSerie_n)
{
	TDelay = TDelay_n;
	Eta = Eta_n;
	OutFile = OutFile_n;
	OutFileEncoding = OutFileEncoding_n;
	iSerie = iSerie_n;
	Sign.resize(0);
	IndexEta.resize(0);
	IndexDelay.resize(0);
	//LastUnit.resize(0);
	//InterpType = LAG;
	//InterpUtilValue = 6;
	TDIQuickMod = new TDITools;
}


/*! \brief Constructs an instance and initializes it using TDelay_n, Eta_n, OutFile_n, iSerie_n, Sign_n, IndexEta_n, IndexDelay_n and TDIQuickMod_n inputs.
 *
 * Attributes are :
 * \arg #TDelay = TDelay_n
 * \arg #Eta = Eta_n
 * \arg #OutFile = OutFile_n
 * \arg #iSerie = iSerie_n
 * \arg #Sign = Sign_n
 * \arg #IndexEta = IndexEta_n
 * \arg #IndexDelay = IndexDelay_n
 * \arg #TDIQuickMod = TDIQuickMod_n
 * \arg #tmpCountInterDelay = 0
 * \arg #tmpCountInterEta = 0
 */

TDI::TDI( Memory * TDelay_n,
		  TDI_InterData * Eta_n,
		  ofstream * OutFile_n,
		  int OutFileEncoding_n,
		  int iSerie_n,
		  vector<int> Sign_n,
		  vector<int> IndexEta_n,
		  vector< vector<int> > IndexDelay_n,
		  TDITools * TDIQuickMod_n//,
		  //INTERP InterpType_n,
		  //double InterpUtilValue_n
		  )
{
	TDelay = TDelay_n;
	Eta = Eta_n;
	OutFile = OutFile_n;
	OutFileEncoding = OutFileEncoding_n;
	iSerie = iSerie_n;
	Sign = Sign_n;
	IndexEta = IndexEta_n;
	IndexDelay = IndexDelay_n;
	//InterpType = InterpType_n;
	//InterpUtilValue = InterpUtilValue_n;
	//LastUnit.resize(0);
	//CreateAllPack();
	TDIQuickMod = TDIQuickMod_n;
	tmpCountInterDelay = 0;
	tmpCountInterEta = 0;
	
}


/*! \brief Constructs an instance and initializes it using TDelay_n, Eta_n, OutFile_n, iSerie_n, SignEtaDelays and TDIQuickMod_n inputs.
 *
 * \arg #TDelay = TDelay_n
 * \arg #Eta = Eta_n
 * \arg #OutFile = OutFile_n
 * \arg #iSerie = iSerie_n
 * \arg #Sign : set by #ReadSignEtaDelays, using SignEtaDelays input
 * \arg #IndexEta : set by #ReadSignEtaDelays, using SignEtaDelays input
 * \arg #IndexDelay : set by #ReadSignEtaDelays, using SignEtaDelays input
 * \arg #TDIQuickMod = TDIQuickMod_n
 * \arg #tmpCountInterDelay = 0
 * \arg #tmpCountInterEta = 0
 */
TDI::TDI( Memory * TDelay_n,
		  TDI_InterData * Eta_n,
		  ofstream * OutFile_n,
		  int OutFileEncoding_n,
		  int iSerie_n,
		  vector<int> SignEtaDelays,
		  TDITools * TDIQuickMod_n//,
		  //INTERP InterpType_n,
		  //double InterpUtilValue_n
		  )
{
	TDelay = TDelay_n;
	Eta = Eta_n;
	OutFile = OutFile_n;
	OutFileEncoding = OutFileEncoding_n;
	iSerie = iSerie_n;
	//InterpType = InterpType_n;
	//InterpUtilValue = InterpUtilValue_n;
	Sign.resize(0);
	IndexEta.resize(0);
	IndexDelay.resize(0);
	//LastUnit.resize(0);
	ReadSignEtaDelays(SignEtaDelays);
	//CreateAllPack();
	TDIQuickMod = TDIQuickMod_n;
	tmpCountInterDelay = 0;
	tmpCountInterEta = 0;
}

/*! \brief Destructor */
TDI::~TDI()
{

}


/* Methodes d'acces */


/* Autres methodes */
/*! \brief Reads lists of packs that are in the following form : 1231 for  D1 D2 D3 Eta1.
 *
 * Reads TDI combinaisons and push them in #Sign #IndexEta and #IndexDelay attributes.
 *
 * \arg for \f$iPack=0, \dots, size(SignEtaDelays)-1\f$ :\n
 * SignEtaDelays[iPack] is checked :  \f$ SignEtaDelays[iPack] \ne 0 \f$
 * \f[ \left\{ \begin{array}{ll}
  \textrm{if } (SignEtaDelays[iPack] > 0)   & +1 \textrm{ is pushed back in Sign attribute} \\
  \textrm{if } (SignEtaDelays[iPack] < 0)   & -1 \textrm{ is pushed back in Sign attribute} 
  \end{array} \right. \f]
 * \f[ TmpInfo=abs(SignEtaDelays[iPack]) \f]
 * \f[ TmpIndexEta=ceil(10 \cdot (\frac{TmpInfo}{10}-floor(\frac{TmpInfo}{10})+10^{-}6)) \f]
 * TmpIndexEta is checked : \f$ 1 \le TmpIndexEta \le 6 \f$\n
 * TmpIndexEta is pushed back in IndexEta attribute\n
 * \arg \f$ TmpInfo=ceil(\frac{TmpInfo}{10}) \f$ ; while \f$tmpInfo \ne 0\f$ :\n
 * \f[ TmpIndexDelay=ceil(10 \cdot (\frac{TmpInfo}{10}-floor(\frac{TmpInfo}{10})+10^{-}6)) \f]
 * TmpIndexDelay is checked :\f$ 1 \le TmpIndexDelay \le 6 \f$\n
 * TmpIndexDelay is pushed back in IndexDelay attribute\n
 * \f[ TmpInfo=ceil(\frac{TmpInfo}{10}) \f]
 */
void TDI::ReadSignEtaDelays (vector<int> SignEtaDelays)
{
	cout << endl <<  "     Read TDI combinaisons ... " << endl;
	Sign.resize(0);
	IndexEta.resize(0);
	IndexDelay.resize(0);
	for(int iPack=0; iPack<SignEtaDelays.size(); iPack++){
		
		int TmpInfo(SignEtaDelays[iPack]);
		if (TmpInfo == 0)
			throw invalid_argument("TDI::ReadSignEtaDelays : The ReadSignEtaDelays can not be 0 !");
		
		// Determination du signe
		if(TmpInfo > 0)
			Sign.push_back(1);
		else
			Sign.push_back(-1);
		TmpInfo = abs(TmpInfo);
		
		// Determination du signal Eta sur lequel le pack agit
		int TmpIndexEta((int)(10.0*((TmpInfo/10.0)-floor(TmpInfo/10.0))+1e-6));
		//cout << "  " << TmpInfo << " --> " << TmpIndexEta << endl;	
		//cout << ((TmpInfo/10.0)-floor(TmpInfo/10.0)) << endl;
		//cout << (10.0*((TmpInfo/10.0)-floor(TmpInfo/10.0))+1e-6) << endl;
		//cout << (int)(10.0*((TmpInfo/10.0)-floor(TmpInfo/10.0))+1e-6) << endl;
		if((TmpIndexEta<1)||(TmpIndexEta>6))
			throw invalid_argument("TDI::ReadSignEtaDelays : Eta index must be 1, 2, 3, 4, 5 or 6 !");
		IndexEta.push_back(TmpIndexEta);
		TmpInfo = (int)floor(TmpInfo/10.0);
		
		// Determination de la liste des retards
		vector<int> TmpListIndexDelay;
		TmpListIndexDelay.resize(0);
		while(TmpInfo != 0){
			int TmpIndexDelay((int)(10.0*((TmpInfo/10.0)-floor(TmpInfo/10.0))+1e-6));
			//cout << "  " << TmpInfo << " --> " << TmpIndexDelay << endl;		
			if((TmpIndexDelay<1)||(TmpIndexDelay>6))
				throw invalid_argument("TDI::ReadSignEtaDelays : Delay index must be 1, 2, 3, 4, 5 or 6 !");
			TmpListIndexDelay.insert(TmpListIndexDelay.begin(), TmpIndexDelay);	
			TmpInfo = (int)floor(TmpInfo/10.0);
		};
		IndexDelay.push_back(TmpListIndexDelay);
	}
	
	for(int i=0; i<IndexEta.size(); i++){
		cout << "       Pack " << i << " :  Sign = " << Sign[i] <<  " , Eta = " << IndexEta[i] << " , Delays = ";
		for(int j=0; j<IndexDelay[i].size(); j++)
			cout << (IndexDelay[i])[j] << " ";
		cout << endl;
	}
	cout << "     Read TDI combinaisons --> OK " << endl;
}





/*! \brief Computes the result of generator.
 *
 * Computes delay using tComputeDelay input and class attributes.
 *
 * If approximations are done in order to compute delays more quickly
 * \f[ TotalDelay=\sum_{iPack=0}^{size(IndexEta)-1}
 {\sum_{iDelay=0}^{size(IndexDelay[iPack])-1}} \f]
 * \f[ TDIQuickMod->getDelay((IndexDelay[iPack])[iDelay]) \f]
 * else
 * \f[ TotalDelay=\sum_{iPack=0}^{size(IndexEta)-1}
 {\sum_{iDelay=0}^{size(IndexDelay[iPack])-1}} \f]
 * \f[ TDelay->gData((IndexDelay[iPack])[iDelay]-1, TotalDelay, LAG, 6) \f]
 * TDITools::getDelay and Memory::gData methods are  called.\n
 * \return
 * if enough data are stored
 * \f[ \sum_{iPack=0}^{size(IndexEta)-1}
 {\sum_{iDelay=0}^{size(IndexDelay[iPack])-1}} \f]
 * \f[ Sign[iPack] \cdot Eta->gData(IndexEta[iPack], TotalDelay) \f]
 * else \f$ 0 \f$
 */
double TDI::Compute(double tComputeDelay)
{
	try{
		double Result(0.0);
		bool usable(true);
		double TotalDelay(0.0), InterResult(0.0);
		for(int iPack=0; iPack<(int)(IndexEta.size()); iPack++){
			TotalDelay = tComputeDelay;
			InterResult = 0.0;
			//cout << "Pack " << iPack << " (NbDelay = " << IndexDelay[iPack].size() << "):" << endl;
			if(TDIQuickMod->getRapidOption()){
				for(int iDelay=0; iDelay< IndexDelay[iPack].size(); iDelay++){
					TotalDelay += TDIQuickMod->getDelay((IndexDelay[iPack])[iDelay]);
				}				
			}else{
				for(int iDelay=0; iDelay< IndexDelay[iPack].size(); iDelay++){
					TotalDelay += TDelay->gData((IndexDelay[iPack])[iDelay]-1, TotalDelay, LAG, 6);
					//tmpCountInterDelay++;
				}
			}
			//cout << "Total Delay = " << TotalDelay << endl;
			if(Eta->getUsable()){
				InterResult = Sign[iPack] * Eta->gData(IndexEta[iPack], TotalDelay);
				//cout << "  Base -> Eta(D" << (IndexDelay[iPack])[0] << ",Eta" << IndexEta[iPack] ;
				//cout << ") :\n    Retard de " << TotalDelay << " sur " << Eta << endl;
				//tmpCountInterEta++;
			}else{
				usable = false;
			}
			//cout << " Result Pack= " << InterResult<< endl;
			Result += InterResult;
			//cout << " Pack " << iPack << " usable = " << usable <<endl;
		}
		//cout << "**  Usable = " << usable <<endl;
		//cout << "**  Result Pack = " << Result << endl;
		//cout << endl << endl ;
		return(Result);
	}
	catch(exception & e ) {
		cerr << "TDI::Compute : error: " << e.what()<<endl;
		throw;
	}		
		
}


///*! \brief Records the result of generator
// *
// * Writes tComputeDelay input into #OutFile.
// */
//void TDI::RecordResult(double tComputeDelay)
//{
//	double RecordAndReturnResult;
//}


/*! \brief Records the result of generator and returns the result.
 *
 * Computes delay using tComputeDelay input, writes result into #OutFile and returns it.
 */
double TDI::RecordAndReturnResult(double tComputeDelay)
{
	double DataRes(Compute(tComputeDelay));
	if(OutFileEncoding>0) // Write the datas in BINARY file
		OutFile->write((char*) &DataRes, sizeof(double));
	else // Write the datas in ASCII file
		(*OutFile) << " " << DataRes;
	return(DataRes);
}

/*! \brief Gives maximum number of delays.
 * 
 * Maximum number of delays is size of #IndexDelay attribute.
 */
int TDI::NbDelayMax()
{
	int NbMax(0);
	for(int i=0; i<IndexDelay.size(); i++){
		if(IndexDelay[i].size()>NbMax)
			NbMax = IndexDelay[i].size();
	}
	return(NbMax);
}


// end of TDI.cpp
