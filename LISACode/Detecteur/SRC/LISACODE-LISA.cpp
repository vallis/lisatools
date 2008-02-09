// $Id: LISACODE-LISA.cpp,v 1.8 2007/04/20 12:20:37 ruiloba Exp $
/*
 *  LISACODE-LISA.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created on 02/00/05 by Antoine PETITEAU (APC)
 *  Last modification on 06/06/06 by Antoine PETITEAU (APC)
 *
 */

#include "LISACODE-LISA.h"

/* Constructeur */

/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg #tStepPhy = 0.01
 * \arg #tStepMes = 1.0
 * \arg #tMemRAM  = 60.0
* \arg #SCPos = #Geometry instance with default values 		
* \arg #sGW = #TrFctGW initialized with default values
* \arg #GWB = NULL
* \arg #RecordPDPM = 3 Memory elements intitalized with #tMemRAM and #tStepMes attributes
* \arg #USOs = 3 USOClock elements intitalized with 0.0 value	
* \arg #NoisePointers = 18 NULL elements
* \arg #PhotoDetects = 12 elements (2 directions, 3 spacecrafts, 2 interference types : 6 first are S interference type, 6 last are TAU interference type)
 */
LISA::LISA()
{
	try{
		vector<GW *> * GW_n;
		tStepPhy = 0.01;
		tStepMes = 1.0;
		tMemRAM  = 60.0;
		
		GW_n = new vector<GW *>;
		
		SCPos = new Geometry();
		
		sGW.init(GW_n, SCPos);
		
		GWB = NULL;
		
		RecordPDPM = new vector<Memory *>;
		RecordPDPM->push_back(new Memory(tMemRAM, tStepMes));
		RecordPDPM->push_back(new Memory(tMemRAM, tStepMes));
		RecordPDPM->push_back(new Memory(tMemRAM, tStepMes));
		
		for(int i=0; i<3; i++)
			USOs.push_back(USOClock(0.0));
		
		for(int i=0; i<(6+6+6); i++)
			NoisePointers.push_back(NULL);
		
		//Creation of photodetector-phasemeters which make a s interference's type
		cout << "   Creation of photodetector-phasemeters which make a S interference's type";
		for(int IndDir=0; IndDir<2; IndDir++){
			for(int iSC=1; iSC<4; iSC++){
				PhotoDetects.push_back(PhoDetPhaMet(S,
													IndDir,
													iSC,
													SCPos,
													& NoisePointers,
													& USOs[iSC-1],
													(*RecordPDPM)[iSC-1],
													& sGW,
													GWB,
													tStepPhy,
													tStepMes));
			} 
		}
		cout << " --> OK !" << endl;
		
		//Creation of photodetector-phasemeters which make a tau interference's type
		cout << "   Creation of photodetector-phasemeters which make a TAU interference's type";
		for(int IndDir=0; IndDir<2; IndDir++){
			for(int iSC=1; iSC<4; iSC++){
				PhotoDetects.push_back(PhoDetPhaMet(TAU,
													IndDir,
													iSC,
													SCPos,
													& NoisePointers,
													& USOs[iSC-1],
													(*RecordPDPM)[iSC-1],
													& sGW,
													GWB,
													tStepPhy,
													tStepMes));
			}
		}
		cout << " --> OK !" << endl;
		
		Stabilization();
		
	}
	catch( exception & e ) 
{       
		cerr << "LISA::LISA : error : " << e.what() << endl;
		throw;
}
}



/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * \arg #tStepPhy = \a tStepPhy_n
 * \arg #tStepMes = \a tStepMes_n
 * \arg #tMemRAM  = \a tMemRAM_n
 * \arg #SCPos = #Geometry instance with default values 		
 * \arg #sGW = initialized using \a GW_n and #SCPos parameters  
 * \arg #GWB = \a GWB_n if not NULL, else it is initialized by Background::setGeometry method, using #SCPos attribute
 * \arg #RecordPDPM = \a RecordDPDM_n 
 * \arg #USOs = 3 USOClock elements intitalized with 0.0 value	
 * \arg #NoisePointers = 6 white noises (NULL if \a PSDLaser input = 0.0), 6 opticals benchs noises (NULL if \a PSDOptBench input = 0.0), 6 inertials masses noises  (NULL if \a PSDInertialMass input = 0.0).
 * \arg #PhotoDetects = 12 elements (2 directions, 3 spacecrafts, 2 interference types : 6 first are S interference type, 6 last are TAU interference type).
 * All noises are initialized using NoiseWhite (ex. NoiseWhite(tStepPhy, tStepMes, \a tMemNoiseFirst_n, \a tMemNoiseLast_n, {PSDLaser,PSDOptBench,PSDInertialMass}).

 */

LISA::LISA(	double tStepPhy_n,
				double tStepMes_n,
				double tMemNoiseFirst_n,
				double tMemNoiseLast_n,
				double tMemRAM_n,
				double PSDLaser,
				double PSDOptBench,
				double PSDInertialMass,
				vector<Memory *> * RecordPDPM_n,
				vector<GW *> * GW_n,
				Background * GWB_n)
{
	try{
		tStepPhy = tStepPhy_n;
		tStepMes = tStepMes_n;
		tMemRAM  = tMemRAM_n;
		
		RecordPDPM = RecordPDPM_n;
		
		SCPos = new Geometry();
		
		sGW.init(GW_n, SCPos);
		
		GWB = GWB_n;
		if(GWB != NULL) GWB->setGeometry(SCPos);
		
		for(int i=0; i<3; i++)
			USOs.push_back(USOClock(0.0));
		
		//Creation of lasers' noise
		cout << "      Creation Lasers' Noise";
		for(int i=0; i<6; i++){
			if(PSDLaser == 0.0){
				NoisePointers.push_back(NULL);
			}else{
				NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst_n, tMemNoiseLast_n, PSDLaser));
				// WARNING : Change for all the laser have PSD (test TDI)
				//if(i==3)
				//	NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, 5.0, -20.0, PSDLaser)); 
				//else
				//	NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes, 5.0, -20.0, 0.0)); 
			}
		}
		cout << " --> OK !" << endl;
		
		//Creation of opticals benchs' noise
		cout << "      Creation of Opticals Benchs' Noise";
		for(int i=0; i<6; i++){
			if(PSDOptBench == 0.0)
				NoisePointers.push_back(NULL);
			else
				NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes,  tMemNoiseFirst_n, tMemNoiseLast_n, PSDOptBench));
		}
		cout << " --> OK !" << endl;
		
		//Creation of inertials masses' noise
		cout << "      Creation of Inertials Masses' Noise";	
		for(int i=0; i<6; i++){
			if(PSDInertialMass == 0.0)
				NoisePointers.push_back(NULL);
			else
				NoisePointers.push_back(new NoiseWhite(tStepPhy, tStepMes,  tMemNoiseFirst_n, tMemNoiseLast_n, PSDInertialMass));
		}
		cout << " --> OK !" << endl;
		
		//Creation of photodetector-phasemeters which make a s interference's type
		cout << "      Creation of photodetector-phasemeters which make a S interference's type";
		for(int IndDir=0; IndDir<2; IndDir++){
			for(int iSC=1; iSC<4; iSC++){
				PhotoDetects.push_back(PhoDetPhaMet(S,
													IndDir,
													iSC,
													SCPos,
													& NoisePointers,
													& USOs[iSC-1],
													(*RecordPDPM)[iSC-1],
													& sGW,
													GWB,
													tStepPhy,
													tStepMes));
			} 
		}
		cout << " --> OK !" << endl;
		
		//Creation of photodetector-phasemeters which make a tau interference's type
		cout << "      Creation of photodetector-phasemeters which make a TAU interference's type";
		for(int IndDir=0; IndDir<2; IndDir++){
			for(int iSC=1; iSC<4; iSC++){
				PhotoDetects.push_back(PhoDetPhaMet(TAU,
													IndDir,
													iSC,
													SCPos,
													& NoisePointers,
													& USOs[iSC-1],
													(*RecordPDPM)[iSC-1],
													& sGW,
													GWB,
													tStepPhy,
													tStepMes));
			}
		}
		cout << " --> OK !" << endl;
		
		Stabilization();
	}
	catch( exception & e ) 
{       
		cerr << "LISA::LISA : error : " << e.what() << endl;
		throw;
}	
}




/*! \brief Constructs an instance and initializes it with default values and \a ConfigLISA and \a RecordPDPM_n inputs.
 *
 * \arg #tStepPhy = extracted from \a ConfigLISA input
 * \arg #tStepMes = extracted from \a ConfigLISA input
 * \arg #tMemRAM  = extracted from \a ConfigLISA input
 * \arg #RecordPDPM  = RecordPDPM_n input
* \arg #SCPos is initialized with inputs extracted from \a ConfigLISA
* \arg #sGW is initialized with inputs extracted from \a ConfigLISA
* \arg #GWB = extracted from \a ConfigLISA input
* \arg #USOs = 3 USOClock elements intitalized with \a ConfigLISA input value	
* \arg #NoisePointers = extracted from \a ConfigLISA input
* \arg #PhotoDetects = 12 elements (2 directions, 3 spacecrafts, 2 interference types : 6 first are S interference type, 6 last are TAU interference type)
 */

LISA::LISA(ConfigSim * ConfigLISA, vector<Memory *> * RecordPDPM_n)
{
	try{
		tStepPhy = ConfigLISA->gettStepPhy();
		tStepMes = ConfigLISA->gettStepMes();
		tMemRAM  = ConfigLISA->gettMemSig();
		RecordPDPM = RecordPDPM_n;
		
		SCPos = new Geometry(ConfigLISA->getOrbStartTime(), ConfigLISA->getOrbInitRot(), ConfigLISA->getArmlength(), ConfigLISA->getOrbOrder(), ConfigLISA->getOrbMove(), ConfigLISA->gettStepPhy());
		
		sGW.init(ConfigLISA->getGWs(), SCPos);
		
		USOs = ConfigLISA->getUSOs();
		
		GWB = ConfigLISA->getGWB();
		if(GWB != NULL) GWB->setGeometry(SCPos);
		
		//Creation of lasers' noise
		cout << "      Creation Noises";
		NoisePointers = ConfigLISA->getNoises();
		cout << " --> OK !" << endl;
		
		//Filter of phasemeter
		vector<double> FilterParam_tmp(4);
		FilterParam_tmp[0] = 140.0;
		FilterParam_tmp[1] = 0.1;
		FilterParam_tmp[2] = 0.1;
		FilterParam_tmp[3] = 0.3;
		
		//Creation of photodetector-phasemeters which make a s interference's type
		cout << "      Creation of photodetector-phasemeters which make a S interference's type" << endl;
		for(int IndDir=0; IndDir<2; IndDir++){
			for(int iSC=1; iSC<4; iSC++){
				//cout << endl << "Phasemetre S " << iSC << " " << IndDir << " :" << endl;
				PhotoDetects.push_back(PhoDetPhaMet(S,
													IndDir,
													iSC,
													SCPos,
													& NoisePointers,
													& USOs[iSC-1],
													(*RecordPDPM)[iSC-1],
													& sGW,
													GWB,
													tStepPhy,
													tStepMes,
													ConfigLISA->getPhaMetFilter(),
													ConfigLISA->getPhaMetFilterParam()));
			} 
		}
		cout << " --> OK !" << endl;
		
		//Creation of photodetector-phasemeters which make a tau interference's type
		if(!(ConfigLISA->getNoNoise())){
		cout << "      Creation of photodetector-phasemeters which make a TAU interference's type" << endl;
			for(int IndDir=0; IndDir<2; IndDir++){
				for(int iSC=1; iSC<4; iSC++){
					//cout << endl << "Phasemetre TAU " << iSC << " " << IndDir << " :" << endl;
					PhotoDetects.push_back(PhoDetPhaMet(TAU,
														IndDir,
														iSC,
														SCPos,
														& NoisePointers,
														& USOs[iSC-1],
														(*RecordPDPM)[iSC-1],
														& sGW,
														GWB,
														tStepPhy,
														tStepMes,
														ConfigLISA->getPhaMetFilter(),
														ConfigLISA->getPhaMetFilterParam()));
				}
			}
			cout << " --> OK !" << endl;
		}
		Stabilization();
	}
	
	catch( exception & e ) 
{       
		cerr << "LISA::LISA : error : " << e.what() << endl;
		throw;
}
}



LISA::~LISA()
{
	
}


/* Access methods */
/*! \brief Gives delay depending on inputs : \a i index, \a IndirectDir, and \a  t time.
 *
 * \a i (arm index), \a IndirectDir, emitter and receiver satellites
 * are related as follows:
 * \arg from satellite 1 to 2: arm i=3, IndirectDir=1
 * \arg from satellite 2 to 3: arm i=1, IndirectDir=1
 * \arg from satellite 3 to 1: arm i=2, IndirectDir=1
 * \arg from satellite 2 to 1: arm i=3, IndirectDir=0
 * \arg from satellite 3 to 2: arm i=1, IndirectDir=0
 * \arg from satellite 1 to 3: arm i=2, IndirectDir=0
 *
 * So, emitter and receiver index depend on \a IndirectDir and 
 * arm index \a  i:
 * \arg if (IndirectDir = 0) em = mod(i+2,3) and rec = mod(i+1,3)
 * \arg else em = mod(i+1,3) and rec = mod(i+2,3)
 *
 * Geometry::gtdelay method is called with \a em, \a rec  index, order = 2 and \a t input. 
 * Returned value the is opposite of Geometry::gtdelay result.
 */
double LISA::gDelayT(int i, int IndirectDir, double t)
{
	int em(0), rec(0);
	if(IndirectDir == 0){
		em = i+2;
		rec = i+1;
	}else{
		em = i+1;
		rec = i+2;
	}
	if (em>3) em -= 3;
    if (rec>3) rec -= 3;
	return(-1.0*SCPos->gtdelay(em, rec, 2, t));
}



/*! \brief Gives arm length depending for on input i index, IndirectDir, and t time.
 *
 * \param i arm index from 1 to 3
 * \param IndirectDir  optical bench direction (0 if the optical bench is in the direct direction, 1 else)
 *  \param t time
 * 
 * \a i (arm index), \a IndirectDir, emitter and receiver satellites
 * are related as follows:
 * \arg from satellite 1 to 2: arm i=3, IndirectDir=1
 * \arg from satellite 2 to 3: arm i=1, IndirectDir=1
 * \arg from satellite 3 to 1: arm i=2, IndirectDir=1
 * \arg from satellite 2 to 1: arm i=3, IndirectDir=0
 * \arg from satellite 3 to 2: arm i=1, IndirectDir=0
 * \arg from satellite 1 to 3: arm i=2, IndirectDir=0
 *
 * So, emitter and receiver index depend on \a IndirectDir and 
 * arm index \a  i:
 * \arg if (IndirectDir = 0) em = mod(i+2,3) and rec = mod(i+1,3)
 * \arg else em = mod(i+1,3) and rec = mod(i+2,3)
 *
 * Geometry::gtdelay method is called, with order = 2
 * returned value is opposite of Geometry::gtdelay result, multiplied by #c_SI.
 */
double LISA::gArmLength(int i, int IndirectDir, double t)
{
	int em(0), rec(0);
	if(IndirectDir == 0){
		em = i+2;
		rec = i+1;
	}else{
		em = i+1;
		rec = i+2;
	}
	if (em>3) em -= 3;
    if (rec>3) rec -= 3;
	return(-1.0*SCPos->gtdelay(em, rec, 2, t)*c_SI);
}


/*! \brief Gives satellite position depending at a given time \a t. 
 * \param i:  spacecraft number [1,3]
 * \param t: time.
 * It calls  Geometry::gposition with inpute \a i and \a t.
 */
Vect LISA::gPosSC(int i, double t)
{
	return(SCPos->gposition(i, t));
}


/* Others methods */
/*int LISA::ChargeNoises(double t)
{
	SCPos->MoveLISA(t);
	int StepReady = 0;
	for(int i=0; i<int(NoisePointers.size()); i++){
		if(NoisePointers[i] != NULL){
			NoisePointers[i]->addNoise(t, tStep);
			//			NoisePointers[i]->addNoise(1e-15*t*t, tStep);
			StepReady += NoisePointers[i]->unusable(t);
		}
	}
	return(StepReady);
}*/

/*! \brief It does phasemeters stabilization (signal and noise) for a given LISA geometry.
 *
 * While current time is lower than stabilization time ( progressively incremented 
 * by with #tStepMes) :\n
 * \arg For all elements in #NoisePointers (i index): if NoisePointers[i] is not NULL, a noise vector is added to NoisePointers[i], using Noise::addNoise method
 * \arg For all elements in #PhotoDetects: photodetector signal is stored in memory, using PhoDetPhaMet::IntegrateSignal method
 */
void LISA::Stabilization()
{	
	double tdecal(-1000.0);
	double ttmp(tdecal);
	double tStab(PhotoDetects[0].gettStab());
	cout << "      Stabilization of phasemeters (" << tStab << " s)..." << endl;
	while(ttmp<=tStab+tdecal){
		//cout << " NoisePointers.size() = " << NoisePointers.size() << endl;
		for(int i=0; i<int(NoisePointers.size()); i++){
			if(NoisePointers[i] != NULL){
				NoisePointers[i]->addNoise();
			}
		}
		for(int i=0; i<int(PhotoDetects.size()); i++){
			//cout << " PhotoDetects " << i << endl;
			PhotoDetects[i].IntegrateSignal(tdecal);
			//cout << " --> OK ! " << endl;
		}
		ttmp += tStepMes;
	}
	cout << "      Stabilization of phasemeters --> OK !" << endl;
}





/*! \brief Makes one step in time after noises loading.
 * 
 * For all elements in NoisePointers (i index):
 * \arg if NoisePointers[i] is NULL, a noise vector is added to NoisePointers[i], using Noise::addNoise method.
 *
 * For all elements in PhotoDetects:
 * \arg photodetector signal is stored in memory, using PhoDetPhaMet::IntegrateSignal method.
 *
 * For all spacecrafts:
 * \arg photodetector-phasemeters signal (stored in #RecordPDPM) is recorded
 * (Memory::RecordAccData is called, using #tStepMes attribute, and time extracted from #USOs).
 */
void LISA::MakeOneStepOfTime(double t)
{
	//cout << "t = " << t << " : Make one step of time " << endl;
	//SCPos->MoveLISA(t);
	for(int i=0; i<int(NoisePointers.size()); i++){
		if(NoisePointers[i] != NULL){
			NoisePointers[i]->addNoise();
		}
	}
	//cout << endl << endl << "************************************************************" << endl; 
	//cout << t << endl;
	for(int i=0; i<int(PhotoDetects.size()); i++){
		//PhotoDetects[i].DisplayStoredData();
		PhotoDetects[i].IntegrateSignal(t);
		//PhotoDetects[i].DisplayStoredData();
	}
	
	// Record photodetector-phasemeters signals
	for(int iSC=1; iSC<=3; iSC++)
		(*RecordPDPM)[iSC-1]->RecordAccData(tStepMes, USOs[iSC-1].gTime(t, tStepMes));
	//cout << "  t1 = " << USOs[1-1].gTime(t) << " ,  t2 = " << USOs[2-1].gTime(t) << " ,  t3 = " << USOs[3-1].gTime(t) << endl;
}


/*! \brief Returns the list of noises mean values, depending on t time.
 *
 * For all elements in NoisePointers (i index):
 * \arg if NoisePointers[i] is not NULL, noise mean value is computed and pushed back in returned value.
 */
vector<double> LISA::PresentMeanNoise(double t)
{
	vector<double> NoisesValues;
	for(int i=0; i<int(NoisePointers.size()); i++){
		if(NoisePointers[i]!=NULL){
			double MeanNoise(0.0);
			for(int j=(NoisePointers[i]->getNbBinAdd()-1); j>=0; j--)
				MeanNoise += NoisePointers[i]->getNoise(-1.0*j*tStepPhy);
			NoisesValues.push_back(MeanNoise/((double)NoisePointers[i]->getNbBinAdd()));
		}
	}
	return(NoisesValues);  
}

//end of LISACODE-LISA.cpp
