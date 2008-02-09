// $Id: LISACODE-PhoDetPhaMet.cpp,v 1.8 2007/04/05 09:33:12 ruiloba Exp $
/*
 *  PhoDetPhaMet.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 18/04/05 by Antoine PETITEAU (APC)
 *  Last modification on 06/06/06 by Antoine PETITEAU (APC)
 *
 */


#include "LISACODE-PhoDetPhaMet.h"

/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * #init method is called with the following arguments :
 * \arg Interf_type_n = S
 * \arg IndirectDir_n = 0
 * \arg iSC_n = 1
 * \arg SCPos_n = empty
 * \arg NPs_n = 6 empty Noise vectors
 * \arg USO_n = empty
 * \arg RecordData_n = empty
 * \arg sGW_n = empty
 * \arg GWB_n = empty
 * \arg tStepPhy_n = 0.01
 * \arg tStepMes_n = 1.0
 * \arg FilterON_n = FALSE
 * \arg attenuation : FilterParam_n[0] = 140.0 dB
 * \arg oscillations in bandwidth FilterParam_n[1] = 0.1 dB
 * \arg low transition frequency / measurement frequency : FilterParam_n[2] = 0.1
 * \arg high transition frequency / measurement frequency : FilterParam_n[3] = 0.3
 */
PhoDetPhaMet::PhoDetPhaMet()
{
	Geometry * SCPos_tmp = new Geometry;
	vector<Noise *> * NPs_tmp = new vector<Noise *>;
	for(int i=0; i<(6+6+6); i++)
		NPs_tmp->push_back(NULL);
	USOClock * USO_n = new USOClock;
	Memory * RecordData_tmp = new Memory();
	TrFctGW * sGW_tmp = new TrFctGW;
	Background * GWB_tmp = new Background();
	vector<double> FilterParam_tmp(4);
	FilterParam_tmp[0] = 140.0;
	FilterParam_tmp[1] = 0.1;
	FilterParam_tmp[2] = 0.1;
	FilterParam_tmp[3] = 0.3;
	
    init(S, 0, 1, SCPos_tmp, NPs_tmp, USO_n,  RecordData_tmp, sGW_tmp, GWB_tmp, 0.01, 1.0, false, FilterParam_tmp);
}



/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * #init method is called with the following arguments :
 * \arg Interf_type_n input
 * \arg IndirectDir_n input
 * \arg iSC_n input
 * \arg SCPos_n input
 * \arg NPs_n input
 * \arg USO_n input
 * \arg RecordData_n input
 * \arg sGW_n = empty
 * \arg GWB_n = empty
 * \arg tStepPhy_n input
 * \arg tStepMes_n input
 * \arg FilterON_n = TRUE
 * \arg attenuation : FilterParam_n[0] = 140.0 dB
 * \arg oscillations in bandwidth FilterParam_n[1] = 0.1 dB
 * \arg low transition frequency / measurement frequency : FilterParam_n[2] = 0.1
 * \arg high transition frequency / measurement frequency : FilterParam_n[3] = 0.3
 */
PhoDetPhaMet::PhoDetPhaMet( PDPMINTERF InterfType_n,
							int IndirectDir_n,
							int iSC_n,
							Geometry * SCPos_n,
							vector<Noise *> * NPs_n,
							USOClock * USO_n,
							Memory * RecordData_n,
							double tStepPhy_n,
							double tStepMes_n)
{
	TrFctGW * sGW_tmp = new TrFctGW;
	Background * GWB_tmp = new Background();
	vector<double> FilterParam_tmp(4);
	FilterParam_tmp[0] = 140.0;
	FilterParam_tmp[1] = 0.1;
	FilterParam_tmp[2] = 0.1;
	FilterParam_tmp[3] = 0.3;
	init(InterfType_n, IndirectDir_n, iSC_n, SCPos_n, NPs_n, USO_n, RecordData_n, sGW_tmp, GWB_tmp, tStepPhy_n, tStepMes_n, true, FilterParam_tmp);
}


/*! \brief Constructs an instance and initializes it with default values and inputs. It is like previous constructor with added inputs for the transfer function (\a sGW_n) and the background (\a GWB_n).
 *
 * #init method is called with the following arguments :
 * \arg Interf_type_n input
 * \arg IndirectDir_n input
 * \arg iSC_n input
 * \arg SCPos_n input
 * \arg NPs_n input
 * \arg USO_n input
 * \arg RecordData_n input
 * \arg sGW_n input
 * \arg GWB_n input
 * \arg tStepPhy_n input
 * \arg tStepMes_n input
 * \arg FilterON_n = TRUE
 * \arg attenuation : FilterParam_n[0] = 140.0 dB
 * \arg oscillations in bandwidth FilterParam_n[1] = 0.1 dB
 * \arg low transition frequency / measurement frequency : FilterParam_n[2] = 0.1
 * \arg high transition frequency / measurement frequency : FilterParam_n[3] = 0.3
 */
PhoDetPhaMet::PhoDetPhaMet(	PDPMINTERF InterfType_n,
							int IndirectDir_n,
							int iSC_n,
							Geometry * SCPos_n,
							vector<Noise *> * NPs_n,
							USOClock * USO_n,
							Memory * RecordData_n,
                            TrFctGW * sGW_n,
							Background * GWB_n,
                            double tStepPhy_n,
                            double tStepMes_n)
{
	vector<double> FilterParam_tmp(4);
	FilterParam_tmp[0] = 140.0;
	FilterParam_tmp[1] = 0.1;
	FilterParam_tmp[2] = 0.1;
	FilterParam_tmp[3] = 0.3;
	init(InterfType_n, IndirectDir_n, iSC_n, SCPos_n, NPs_n, USO_n, RecordData_n, sGW_n, GWB_n, tStepPhy_n, tStepMes_n, true, FilterParam_tmp);
}


/*! \brief Constructs an instance and initializes it with inputs. It is like previous constructor with 
 * added inputs for  filter description.
 *
 * #init method is called with the following arguments :
 * \arg Interf_type_n input
 * \arg IndirectDir_n input
 * \arg iSC_n input
 * \arg SCPos_n input
 * \arg NPs_n input
 * \arg USO_n input
 * \arg RecordData_n input
 * \arg sGW_n input
 * \arg GWB_n input
 * \arg tStepPhy_n input
 * \arg tStepMes_n input
 * \arg FilterON_n input
 * \arg attenuation : FilterParam_n[0] input
 * \arg oscillations in bandwidth FilterParam_n[1] input
 * \arg low transition frequency / measurement frequency : FilterParam_n[2] input
 * \arg high transition frequency / measurement frequency : FilterParam_n[3] input
 */
PhoDetPhaMet::PhoDetPhaMet(	PDPMINTERF InterfType_n,
							int IndirectDir_n,
							int iSC_n,
							Geometry * SCPos_n,
							vector<Noise *> * NPs_n,
							USOClock * USO_n,
							Memory * RecordData_n,
                            TrFctGW * sGW_n,
							Background * GWB_n,
                            double tStepPhy_n,
                            double tStepMes_n,
							bool FilterON_n,
							vector<double> FilterParam_n)
{
	//cout << "In PhoDetPhaMet::PhoDetPhaMet" << endl;
	init(InterfType_n, IndirectDir_n, iSC_n, SCPos_n, NPs_n, USO_n, RecordData_n, sGW_n, GWB_n, tStepPhy_n, tStepMes_n, FilterON_n, FilterParam_n);
}


/*!\brief Destructor */

PhoDetPhaMet::~PhoDetPhaMet()
{
	
}


/* Access methods */

/*! \brief Initializes an instance with default values and inputs.
 *
 * Inputs are checked :
 * \arg travel direction IndirectDir_n must have to be 0 and 1
 * \arg Spacecraft index iSC_n expected values are 1, 2 and 3
 * \arg Physical time step tStepPhy_n must be positive or null
 * \arg Measurement time step tStepMes_n must be positive or null
 *
 * Set attributes are :
 * \arg #InterfType = InterfType_n
 * \arg #IndirectDir = IndirectDir_n
 * \arg #iSC = iSC_n
 * \arg #SCPos = SCPos_n
 * \arg #NPs = NPs_n
 * \arg #USO = USO_n
 * \arg #RecordData = RecordData_n + additinal serie data (using Memory::AddSerieData method, with IndirectDir, InterfType and iSC information)
 * \arg #sGW = sGW_n
 * \arg #GWB = GWB_n
 * \arg #tStepPhy = tStepPhy_n
 * \arg #tStepMes = tStepMes_n
 * \arg #FilterON = FilterON_n
 * \arg #FilterParam = FilterParam_n
 * \arg #NoNoise  is informed using #getNoNoise method
 * \f[ \textrm{InterfPhyData = NbDataStored null elements, with } NbDataAdd = 2 \cdot (int)(\frac{tStepMes}{tStepPhy}+\frac{1}{2}) \f]
 * \f[  \textrm{if (FilterON)} \left\{ \begin{array}{l}
 \textrm{PBFilter = new Filter(1/tStepPhy, FilterParam[0], FilterParam[1], FilterParam[2]/tStepMes, FilterParam[3]/tStepMes)} \\
 \textrm{FilterPhyData = NbDataStored null elements} \end{array} \right. \f]
 */
void PhoDetPhaMet::init(PDPMINTERF InterfType_n,
                        int IndirectDir_n,
                        int iSC_n,
                        Geometry * SCPos_n,
                        vector<Noise *> * NPs_n,
                        USOClock * USO_n,
                        Memory * RecordData_n,
                        TrFctGW * sGW_n,
						Background * GWB_n,
                        double tStepPhy_n,
                        double tStepMes_n, 
						bool FilterON_n,
						vector<double> FilterParam_n)
{
	try {
		if ((IndirectDir_n < 0)||(IndirectDir_n > 1))
			throw invalid_argument(" The travel direction is 0 or 1 !");
		if ((iSC_n < 1)||(iSC_n > 3))
			throw invalid_argument(" Spacecraft index must be 1, 2 or 3 !");
		if (tStepPhy_n < 0.0)
			throw invalid_argument(" Physical time step cannot be negative !");
		if (tStepMes_n < 0.0) 
			throw invalid_argument(" Measurement time step cannot be negative !");
		InterfType = InterfType_n;
		IndirectDir = IndirectDir_n;
		iSC = iSC_n;
		SCPos = SCPos_n;
		NPs = NPs_n;
		USO = USO_n;
		RecordData = RecordData_n;
		sGW = sGW_n;
		GWB = GWB_n;
		tStepPhy = tStepPhy_n;
		tStepMes = tStepMes_n;
		FilterON = FilterON_n;
		FilterParam = FilterParam_n;
		NoNoise = getNoNoise();
		
		//cout << " InterfPhyData.size() = " << InterfPhyData.size() << endl;
		switch (InterfType){
			case (S) :
				RecordData->AddSerieData(IndirectDir, "s", IndirectDir, iSC);  // Columns in memory : s -> 0 , s' -> 1
				break;
			case (TAU) :
				RecordData->AddSerieData(2+IndirectDir, "tau", IndirectDir, iSC);  // Columns in memory : tau -> 2 , tau' -> 3
				break;
			default :
				throw invalid_argument(" The type of photodetector-phasemeter is unknown !");
		}
		
		// Compute the number of added data
		//cout << "tStepMes/tStepPhy = " << tStepMes/tStepPhy << endl;
		NbDataAdd = (int)(tStepMes/tStepPhy+0.5);
		//cout << "NbDataAdd = " << NbDataAdd << endl;
		
		// Compute the number of stored data
		NbDataStored = 2*NbDataAdd;
		
		// Creation of filter
		if(FilterON){ 
			
			//PBFilter = new Filter(1.0/tStepPhy, 140.0, 0.1, 0.1/tStepMes,  0.3/tStepMes);
			PBFilter = new Filter(1.0/tStepPhy, FilterParam[0], FilterParam[1], FilterParam[2]/tStepMes, FilterParam[3]/tStepMes);
			//PBFilter.init(alpha, beta, NbDataStab);
			cout << "        Creation of filter (Stabilization time = " << PBFilter->getNbDataStab()*tStepPhy << "s) --> OK !" << endl;
			
			// Recompute the number of stored data
			NbDataStored = MAX(2*NbDataAdd, 2*PBFilter->getDepth());
			//cout << "PBFilter->getDepth() = " << PBFilter->getDepth() << endl;
			//cout << "NbDataStored = " << NbDataStored << endl;
		}
		
		InterfPhyData.resize(NbDataStored , 0.0);
		if(FilterON) FilterPhyData.resize(NbDataStored , 0.0);
	}
	catch( exception & e ) 
{       
		cerr << "PhoDetPhaMet::init : " << e.what() << endl;
		throw;
}

}

  /*!\brief Displays stored data
   *
   * Displayed data are :
   * \arg Phasemeter informations : #InterfType, #iSC and #IndirectDir
   * \arg physical data #InterfPhyData and filtered physical data #FilterPhyData if present (depending on #FilterON)
   */

void PhoDetPhaMet::DisplayStoredData()
{
	cout << endl << "Phasemetre " << InterfType << "  " << iSC << " " << IndirectDir << " : " << endl;
	for(int i=0; i<InterfPhyData.size(); i++){
		cout << "  InterfPhyData[" << i << "] = " << InterfPhyData[i];
		if(FilterON) cout << "  &  FilterPhyData[" << i << "] = " << FilterPhyData[i] << endl;
	}
}

  /*!\brief Gets stabilization time.
   *
   *\return Product between low pass filter #PBFilter stabilization data number and physical simulation time step #tStepPhy
   */

double PhoDetPhaMet::gettStab(){
	double res(0.0);
	if(FilterON) res = PBFilter->getNbDataStab()*tStepPhy;
	return(res);
}



/* Others methods */
/*!\brief Returns value of specified noise after delay computation.
 *
 * \param OrigN noise origin
 * \param IndirectDir optical bench direction (0: direct ; 1 : indirect)
 * \param iSC space craft number (1,2,3)
 * \param tDelay delay time
 *
 * Inputs are checked : OrigN expected values are
 * \arg LA (laser noise),
 * \arg OB (optical bench),
 * \arg IM (inertial mass),
 * \arg OP (optical paths noise).
 *
 * First, index is computed :\n
 * \f$ indexNPs = \left\{ \begin{array}{ll}
 * 3 \cdot IndirectDir+iSC-1 & \textrm{if OrigN=LA}\\
 * 3 \cdot IndirectDir+iSC-1+6 & \textrm{if OrigN=OB}\\
 * 3 \cdot IndirectDir+iSC-1+12 & \textrm{if OrigN=IM}\\
 * 3 \cdot IndirectDir+iSC-1+18 & \textrm{if OrigN=OP}\\
 * 24 & \textrm{else} \end{array} \right. \f$ \n
 * Then, noise corresponding to that index and tDelay time is given by Noise::getNoise method and its value is returned.
 */
double PhoDetPhaMet::gN(NOISEORIG OrigN, int iSC, int IndirectDir, double tDelay)
{
	try{
		//WARNING  : Choice one configuration : LISA or Optical Bench (the unuse configuration is in comment)
		int indexNPs(3*IndirectDir+iSC-1); // Configuration LISA : 6 lasers
										   //int indexNPs(iSC-1); // Configuration Optical Bench : 3 lasers 
		
		//cout << "OrigN = " << OrigN << "  ,  indexNPs = " << indexNPs << "  ,  tDelay = " << tDelay << endl;
		
		switch (OrigN) {
			case LA : // Laser Noise
				break;
			case OP : // Optical path noise (with shot noise)
				indexNPs += 6;
				break;
			case IM : // Inertial mass noise
				indexNPs += 12;
				break;
			case OB : // Optical bench noise 
				indexNPs += 18;
				break;				
			default :
				if((*NPs).size()>24) 
					indexNPs = 24;
				else
					throw invalid_argument("PhoDetPhaMet::gN : There is no specified noise !");
		};
		if ((*NPs)[indexNPs] == NULL){
			return(0.0);
		}else{
			/*if (indexNPs > 11)
			cout << " gN for Noise " << indexNPs << " at " << tDelay << " : " << (*NPs)[indexNPs]->getNoise(tDelay) << endl;*/
			return((*NPs)[indexNPs]->getNoise(tDelay));
		}
	}
	catch(exception & e ) {
		cerr << "PhoDetPhaMet::gN : error: " << e.what()<<endl;
		throw;
	}
}


/*!\brief Returns value of Gravitationnal Wave Background (iSC={1,2,3}, IndirectDir={0,1})
 * 
 *\param iSC = {1,2,3} is the spacecraft index
 *\param IndirectDir = {0,1}
 *\param t = time.
 *\return If GWB pointer is NULL, returned value is 0, else \n
 * it is result of Background::deltanu method, called with \a iSC, \a IndirectDir and \a t inputs.
 */
double PhoDetPhaMet::gGWB(int iSC, int IndirectDir, double t)
{
	try{
		if (GWB == NULL){
			return(0.0);
		}else{
			return(GWB->deltanu(iSC, IndirectDir, t));
		}
	}
	catch(exception & e ) {
		cerr << "PhoDetPhaMet::gGWB : error: " << e.what()<<endl;
		throw;
	}
}


/*!\brief Computes the signal received by photodetector-phasemeters.
 *
 * \param t time
 *
 * #InterfType attribute is checked : expected values are S and TAU.
 *
 * Phasemeter S : Between distant and local laser beam : \n
 * \f$ s_1   = s_1^{GW} + d_1^{OPN} + D_3 p'_2 - p_1 - 2 * d_1^{IM} ... ( + D_3 {d'}_2^{OB}  + d_1^{OB} ) \f$ \n
 * \f$ s'_1  = {s'}_1^{GW} + {d'}_1^{OPN} + {D'}_2 {p}_3 - {p'}_1 + 2 * {d'}_1^{IM} ... ( - D_2 {d}_33^{OB}  - {d'}_1^{OB} ) \f$ \n
 * Phasemeter S : Between distant and local laser beam : \n
 * \f$ \tau_1   = {p'}_1 - p_1 - 2 * d_1^{IM} ... ( + 2 {d'}_1^{OB} ) \f$ \n
 * \f$ \tau'_1  = p_1 - {p'}_1 + 2 * d_1^{IM} ... ( - 2 {d}_1^{OB} ) \f$ \n
 *
 * Computations :
 * \arg if (InterfType=S) \n
 * \f$  GWSignal=sGW->deltanu(iSC, modulo(iSC+1+IndirectDir,3), 2, t)  \f$ \n
 * \f$  + gGWB(iSC, IndirectDir, t) \f$ \n
 * for \f$i=0, \dots, NbDataAdd-1\f$, \n
 * \f$ \left\{ \begin{array}{ll}
 * \textrm{if (NoNoise)}   & InterfPhyData[i] = GWSignal\\
 * \textrm{else}   & InterfPhyData[i] = GWSignal + noise_i
 * \end{array} \right. \f$ \n
 * \f$ noise_i= gN(OP, iSC, IndirectDir, tDPhy_i) \f$ \n
 * \f$  + gN(LA, iSCpe1, 1-IndirectDir, tDPhy_i \f$ \n
 * \f$+(*SCPos).gtdelay(modulo(iSC+1+IndirectDir,3),iSC,2,t+tDPhy_i))  \f$ \n
 * \f$  + gN(OB, iSCpe1, 1-IndirectDir, tDPhy_i \f$ \n
 * \f$+(*SCPos).gtdelay(modulo(iSC+1+IndirectDir,3),iSC,2,t+tDPhy_i)) \f$ \n
 * \f$  - 2 \cdot gN(IM, iSC, 0, tDPhy_i) \f$ \n
 * \f$  - gN(LA, iSC, IndirectDir, tDPhy_i) \f$ \n
 * \f$  + gN(OB, iSC, IndirectDir, tDPhy_i) \f$ \n
 * \f$ tDPhy_i = USO->gGap(t,tStepPhy) - i \cdot tStepPhy  \f$ \n
 * \arg if (InterfType=TAU)\n
 * for \f$i=0, \dots, NbDataAdd-1\f$ \n
 * \f$ InterfPhyData[i]= gN(LA, iSC, 1, tDPhy_i) \f$ \n
 * \f$ - 2 \cdot gN(IM, iSC, 1-IndirectDir, tDPhy_i) \f$ \n
 * \f$ + 2 \cdot gN(OB, iSC, 1-IndirectDir, tDPhy_i) \f$ \n
 * \f$ -  gN(LA, iSC, IndirectDir, tDPhy_i) \f$ \n
 *
 * PhoDetPhaMet::gN , TrFctGW::deltanu USOClock::gGap and Geometry::gtdelay methods are called.
 */
void PhoDetPhaMet::ReceiveSignal(double t)
{
    double GWSignal(0.0); // Gravitational waves response for the measurement
	double tDPhy(0.0); // Physical time delay   
    int iSCpe1(iSC+1), iSCpe2(iSC+2);
    if (iSCpe1>3) iSCpe1 -= 3;
    if (iSCpe2>3) iSCpe2 -= 3;
	
	
    switch (InterfType){
        case S :
            if (IndirectDir == 0){
                GWSignal = sGW->deltanu(iSC, iSCpe1, 2, t) + gGWB(iSC, 0, t);
				//cout << "** t = " << t << " : GWSignal = " << GWSignal << endl;
				//cout << "NbDataAdd = " << NbDataAdd << endl;
				if(NoNoise){
					for(int i=NbDataAdd-1; i>=0; i--){
						InterfPhyData[i] = GWSignal;
					}
				}else{
					for(int i=NbDataAdd-1; i>=0; i--){
						tDPhy = USO->gGap(t,tStepPhy) - i*tStepPhy;
						//cout << " S : tDPhy = " << tDPhy << " , tDPhy-(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy) = " << tDPhy-(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy) << endl; 
						//cout << "     USO->gGap(t,tStepPhy) = " << USO->gGap(t,tStepPhy) << endl;
						// s1 = s1GW + s1GW + D3 p'2 + D3 Delta'2 - 2 delta1 - p1 - Delta1 
						InterfPhyData[i] = 
							GWSignal
							+ gN(OP, iSC, 0, tDPhy)
							+ gN(LA, iSCpe1, 1, tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy)) 
							+ gN(OB, iSCpe1, 1, tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy))
							- 2.0 * gN(IM, iSC, 0, tDPhy)
							- gN(LA, iSC, 0, tDPhy)
							+ gN(OB, iSC, 0, tDPhy); 
							// + gN(OB, iSC, 0, tDPhy); // Change of the sign for this noise in version 1.3
						//cout.precision(15);
						//cout << endl;
						//cout << "t+tDPhy = " << t+tDPhy << endl; 
						//cout << "GWSignal = " << GWSignal << endl;
						//cout << "Shot Noise = " << gN(OP, iSC, 0, tDPhy) << endl;
						//cout << "gN(LA, " << iSCpe1 << ", 1, " << tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy) << ") = " << gN(LA, iSCpe1, 1, tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy)) << endl;
						//cout << "gN(OB,iSCpe1, 1, tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy)) = " << gN(OB, iSCpe1, 1, tDPhy+(*SCPos).gtdelay(iSCpe1,iSC,2,t+tDPhy)) << endl;
						//cout << "2.0 * gN(IM, iSC, 0, tDPhy) = " << 2.0 * gN(IM, iSC, 0, tDPhy) << endl;
						//cout << "gN(LA, " << iSC << ", 0, " << tDPhy << ") = " << gN(LA, iSC, 0, tDPhy) << endl;
						//cout << "gN(OB, iSC, 0, tDPhy) = " << gN(OB, iSC, 0, tDPhy) << endl;
						//cout << "S" << iSC << "(" << t + tDPhy << ") : " << InterfPhyData[i] << endl;
						//cout << endl;
						//cout << "   D" << iSCpe2 << " -> D" << iSCpe1 << iSC << " = " << (*SCPos).gtdelay(iSCpe1,iSC,0,t+tDPhy) << endl;
					}
				}
            }else{
				GWSignal = sGW->deltanu(iSC, iSCpe2, 2, t) + gGWB(iSC, 1, t);
				if(NoNoise){
					for(int i=NbDataAdd-1; i>=0; i--){
						InterfPhyData[i] = GWSignal;
					}
				}else{
					for(int i=NbDataAdd-1; i>=0; i--){
						tDPhy = USO->gGap(t,tStepPhy) - i*tStepPhy;
						//cout << " S' : tDPhy = " << tDPhy << " ,tDPhy+(*SCPos).gtdelay(iSCpe2,iSC,2,t+tDPhy) = " << tDPhy+(*SCPos).gtdelay(iSCpe2,iSC,2,t+tDPhy) << endl;
						//cout << "     USO->gGap(t,tStepPhy) = " << USO->gGap(t,tStepPhy) << endl;
						// s'1 = s'1GW + s'1OP + D'2 p3 + D'2 Delta3 - 2 delta'1 - p'1 - Delta'1 
						InterfPhyData[i] = 
							GWSignal
							+ gN(OP, iSC, 1, tDPhy)
							+ gN(LA, iSCpe2, 0, tDPhy+(*SCPos).gtdelay(iSCpe2,iSC,2,t+tDPhy))
							- gN(OB, iSCpe2, 0, tDPhy+(*SCPos).gtdelay(iSCpe2,iSC,2,t+tDPhy)) 
							+ 2.0 * gN(IM, iSC, 1, tDPhy)
							- gN(LA, iSC, 1, tDPhy)
							- gN(OB, iSC, 1, tDPhy);
							// - gN(OB, iSCpe2, 0, tDPhy+(*SCPos).gtdelay(iSCpe2,iSC,2,t+tDPhy)) + 2.0 * gN(IM, iSC, 1, tDPhy) // Change of the sign for this noise in version 1.3
						//cout << "S'" << iSC << "(" << t + tDPhy << ") :"  << InterfPhyData[i] << endl;
						//cout << "   D'" << iSCpe1 << " -> D" << iSCpe2 << iSC << " = " << (*SCPos).gtdelay(iSCpe2,iSC,0,t+tDPhy) << endl;
						
					}
				}
			}
            break;
        case TAU :
            if (IndirectDir == 0){
				for(int i=NbDataAdd-1; i>=0; i--){
					tDPhy = USO->gGap(t,tStepPhy) - i*tStepPhy;
					// tau1 =  p'1 - 2 delta'1 - 2 Delta'1 - p1
					InterfPhyData[i] =
						gN(LA, iSC, 1, tDPhy)
						- 2.0*gN(IM, iSC, 1, tDPhy)
						+ 2.0*gN(OB, iSC, 1, tDPhy) 
						- gN(LA, iSC, 0, tDPhy);
						// - 2.0*(gN(IM, iSC, 1, tDPhy) - gN(OB, iSC, 1, tDPhy)) // Change of the sign for this noise in version 1.3
				}
            }else{
				for(int i=NbDataAdd-1; i>=0; i--){
					tDPhy = USO->gGap(t,tStepPhy) - i*tStepPhy;
					// tau1 =  p'1 - 2 delta'1 - 2 Delta'1 - p1
					InterfPhyData[i] =
						gN(LA, iSC, 0, tDPhy)
                        + 2.0*gN(IM, iSC, 0, tDPhy)
						- 2.0*gN(OB, iSC, 0, tDPhy) 
						- gN(LA, iSC, 1, tDPhy);
						// + 2.0*(gN(IM, iSC, 0, tDPhy) - gN(OB, iSC, 0, tDPhy)) // Change of the sign for this noise in version 1.3
				}
            }
            break;
        default :
            throw invalid_argument("PhoDetPhaMet::ReceiveSignal : The type of photodetector-phasemeter is unknown !");
    }
}


/*!\brief Stores the result in memory (one measurement).
 *
 * #InterfType attribute is checked : expected values are S and TAU.
 *
 * Steps :\n
 * List of physical values (InterfPhyData) and filtered data  (FilterPhyData) (if FilterON) are made. \n
 * Signal received by photodetector-phasemeters is computed. \n
 * Filtering is applied (if FilterON). \n
 * Last data are deleted. \n
 * \result
 * If (FilterON) PhaMetResult = FilterPhyData first value, else InterfPhyData first value. \n
 * Result is stored in memory, with serieNumber=IndirectDir if InterfType is S, 2+IndirectDir if InterfType is TAU.
 */
void PhoDetPhaMet::IntegrateSignal(double t)
{ 
	
	double PhaMetResult(0.0);
	// Make the list of physical values (InterfPhyData) resulting from interferences for a measurement
	InterfPhyData.insert(InterfPhyData.begin(), NbDataAdd, 0.0);
	if(FilterON) FilterPhyData.insert(FilterPhyData.begin(), NbDataAdd, 0.0);
	
	ReceiveSignal(t);
	
	// Filtering the list of physical values (InterfPhyData -> FilterPhyData)
	//cout << endl << "Filtering ( StartBin = " << NbDataAdd-1 << " ) ..." << endl;	
	if(FilterON) PBFilter->App(NbDataAdd-1, InterfPhyData, FilterPhyData);
	//cout << endl << "Filtering --> OK !" << endl;
	
	// Delete last data
	InterfPhyData.resize(NbDataStored);
	if(FilterON) FilterPhyData.resize(NbDataStored);
	
	
	// Result of phasemeter measurement :
	// Extract the mean value of FilterPhyData
	//for(int i=NbDataAdd; i>=0; i--)
	//	PhaMetResult += FilterPhyData[i];
	//PhaMetResult = PhaMetResult/((double)FilterPhyData.size());
	
	// Extract the last value of FilterPhyData 
	//cout << "InterfPhyData[0] = " << InterfPhyData[0] << endl;
	if(FilterON){
		PhaMetResult = FilterPhyData[0];
		//cout << "Filter ON : PhaMetResult = " << PhaMetResult << endl;
	}else{
		PhaMetResult = InterfPhyData[0];
		//cout << "Filter OFF : PhaMetResult = " << PhaMetResult << endl;
	}
	//cout << " -----  PhaMetResult = " << PhaMetResult << endl;
	
	// Send the result of phasemeter measurement at the memory
	switch (InterfType){
		case S :
			RecordData->ReceiveData(IndirectDir, PhaMetResult); // Columns in memory : s -> 0 , s' -> 1
			break;
		case TAU :
			RecordData->ReceiveData(2+IndirectDir, PhaMetResult); // Columns in memory : tau -> 2 , tau' -> 3
			break;
		default :
			throw invalid_argument("PhoDetPhaMet::IntegrateSignal : The type of photodetector-phasemeter is unknown !");
	}
}

/*! \brief Indicates if noises are present or not. It returns true if there are no noises.
 *
 * Checks all elements of #NPs attribute.\n
 * If all vectors are NULL, returned value is TRUE, else FALSE.
 */
bool PhoDetPhaMet::getNoNoise()
{
	bool NNoise(true);
	for(int i=0; i<(*NPs).size(); i++){
		if((*NPs)[i] != NULL)
			NNoise = false;
	}
	return(NNoise);
}


// end of PhoDetPhaMet.cpp


