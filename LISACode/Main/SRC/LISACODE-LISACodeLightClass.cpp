/*
 *  LISACODE-LISACodeLightClass.cpp
 *  V 1.4
 *  LISACode
 *
 *
 *  Created by Antoine Petiteau (AEI) on 09/09/08.
 *  Last modification on 09/09/08 by Antoine PETITEAU (AEI)
 *
 */

#include "LISACODE-LISACodeLightClass.h"


// *****************
// *  Constructor  *
// *****************

LISACodeLightClass::LISACodeLightClass()
{
	t0 = 0.0;;
	dt = 15.0;
	Tobs = pow(2.0,21)*dt;
	TDIsPacks.resize(0);
	TDIsPacksFact.resize(0);
	TDIAE = false;
	NbMaxDelays = 0;	
	NoUsed = true;
	GWs = new vector <GW *>;
	(*GWs).resize(0);
	NbParams = 0;
}


LISACodeLightClass::LISACodeLightClass(double t0_n, double dt_n, double Tobs_n)
{
	t0 = t0_n;
	dt = dt_n;
	Tobs = Tobs_n;
	TDIsPacks.resize(0);
	TDIsPacksFact.resize(0);
	TDIAE = false;
	NbMaxDelays = 0;	
	NoUsed = true;
	GWs = new vector <GW *>;
	(*GWs).resize(0);
	NbParams = 0;
}


LISACodeLightClass::LISACodeLightClass(double t0_n, double dt_n, double Tobs_n, vector <GW *> * GWs_n)
{
	t0 = t0_n;
	dt = dt_n;
	Tobs = Tobs_n;
	TDIsPacks.resize(0);
	TDIsPacksFact.resize(0);
	TDIAE = false;
	NbMaxDelays = 0;	
	NoUsed = true;
	GWs = GWs_n;
	CountNbGWsParams();
	NbParams = 0;
}

LISACodeLightClass::LISACodeLightClass(double t0_n, double dt_n, double Tobs_n, int * GWSrcTypes, int NbGWSrc)
{
	t0 = t0_n;
	dt = dt_n;
	Tobs = Tobs_n;
	TDIsPacks.resize(0);
	TDIsPacksFact.resize(0);
	TDIAE = false;
	NbMaxDelays = 0;	
	NoUsed = true;
	GWs = new vector <GW *>;
	(*GWs).resize(0);
	for(int iGW=0; iGW<NbGWSrc; iGW++){
		switch (GWSrcTypes[iGW]) {
		case 1 :
			(*GWs).push_back(new GWMono);
			break;
		case 2 :
			(*GWs).push_back(new GWBinary);
			break;
		case 3 :
			(*GWs).push_back(new GWNewton2);
			break;
		case 4 :
			(*GWs).push_back(new GWFastSpinBBH);
			break;
		case 5 :
			(*GWs).push_back(new GWCusp);
			break;
		default :
				throw invalid_argument("LISACodeLightClass::LISACodeLightClass :: Undefined GW source type.");
		}
	}
	CountNbGWsParams();
	NbParams = 0;
}


LISACodeLightClass::~LISACodeLightClass()
{
	
}


// **********************
// **  Access methods  **
// **********************
void LISACodeLightClass::ChangeParamsGWs(double * NewParams)
{
	int iP(0);
	for (int iGW=0; iGW<GWs->size(); iGW++) {
		//cout << "Nb parameters of src " << iGW << " = " << (*GWs)[iGW]->getNParam() << endl;
		for(int iPGW=0; iPGW<(*GWs)[iGW]->getNParam(); iPGW++){
			(*GWs)[iGW]->setParam(iPGW, NewParams[iP]);
			iP++;
		}
	}
}


void LISACodeLightClass::AddGWMono()
{
	GWs->push_back(new GWMono);
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}


void LISACodeLightClass::AddGWBinaryFixFreq()
{
	GWs->push_back(new GWBinary);
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}


void LISACodeLightClass::AddGWBinaryInspiralPN()
{
	GWs->push_back(new GWNewton2);
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}


void LISACodeLightClass::AddGWSpinBBH()
{
	GWs->push_back(new GWFastSpinBBH(-900.0, Tobs));
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}

void LISACodeLightClass::AddGWSpinBBH (double Beta_n,
										   double Lambda_n, 
										   double Mass1_n,                
										   double Mass2_n,                 
										   double CoalescenceTime_n,
										   double Distance_n,
										   double Spin1_n,
										   double Spin2_n,
										   double PolarAngleOfSpin1_n,
										   double PolarAngleOfSpin2_n,    
										   double AzimuthalAngleOfSpin1_n,
										   double AzimuthalAngleOfSpin2_n,
										   double PhaseAtCoalescence_n,
										   double InitialPolarAngleL_n,
										   double InitialAzimuthalAngleL_n,
										   double t0, 
										   double Tobs)
{
	GWs->push_back(new GWFastSpinBBH (Beta_n, Lambda_n, Mass1_n, Mass2_n, CoalescenceTime_n, Distance_n, 
									  Spin1_n, Spin2_n, PolarAngleOfSpin1_n, PolarAngleOfSpin2_n, AzimuthalAngleOfSpin1_n, AzimuthalAngleOfSpin2_n,
									  PhaseAtCoalescence_n,InitialPolarAngleL_n, InitialAzimuthalAngleL_n,
									  0.0, t0, Tobs, 7.0, 150.0, 6.0) );
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}


void LISACodeLightClass::AddGWCusp()
{
	GWs->push_back(new GWCusp(dt, Tobs, 900.0));
	NbParams += (*GWs)[GWs->size()-1]->getNParam();
}


// **********************
// **  Others methods  **
// **********************
void LISACodeLightClass::initFirst()
{
	TDIInterpUtilVal = 4;
	tMemInterpTDI = dt*(2.0+ceil(TDIInterpUtilVal/2.0));
	tMinDelay     = dt*ceil(0.8*L0_m_default/(dt*c_SI));
	tMaxDelay     = dt*ceil(1.2*L0_m_default/(dt*c_SI));
	tTDIShift     = 2.0 * MAX(0.0 , tMemInterpTDI-tMinDelay );
	tMemTDI       = tTDIShift + tMemInterpTDI + NbMaxDelays*tMaxDelay;
	
	cout << " Shift for TDI = " << tTDIShift << endl;
	cout << " Duration for storing SC data = " << tMemTDI << endl; 
	
	LISAGeo = new GeometryMLDC(t0, 0.0, L0_m_default, 0, 1, dt);
	SigGWs = new TrFctGW(GWs, LISAGeo);
	LISAGeo->settRangeStorePos(3600.0);
	LISAGeo->settRangeStoreDelay(3600.0);
	cout << "Geometry : Range Store Position = " << LISAGeo->gettRangeStorePos() << " , Range Store Delay = " <<  LISAGeo->gettRangeStoreDelay() << endl;
	
	// Spacecraft signals
	SCSig = new Memory(tMemTDI, dt);
	SCSig->AddSerieData(0, "s", 0, 1);
	SCSig->AddSerieData(1, "s", 0, 2);
	SCSig->AddSerieData(2, "s", 0, 3);
	SCSig->AddSerieData(3, "s", 1, 1);
	SCSig->AddSerieData(4, "s", 1, 2);
	SCSig->AddSerieData(5, "s", 1, 3);
	
	// Delay
	DelayTDI = new Memory(tMemTDI, dt);
	DelayTDI->AddSerieData(0, "D", 0, 1);
	DelayTDI->AddSerieData(1, "D", 0, 2);
	DelayTDI->AddSerieData(2, "D", 0, 3);
	DelayTDI->AddSerieData(3, "D", 1, 1);
	DelayTDI->AddSerieData(4, "D", 1, 2);
	DelayTDI->AddSerieData(5, "D", 1, 3);
	
	TDIQuick = new TDITools(DelayTDI, true);
	
	NbTDI = TDIsPacks.size();
	NbtDat = (int) (floor(Tobs/dt+0.1));
	NbfDat = NbtDat/2+1;
	
	TDIGens = (TDI**) malloc(NbTDI*sizeof(TDI*));
	for(int iGen=0; iGen<NbTDI; iGen++){
		TDIGens[iGen] = new TDI(DelayTDI, SCSig, TDIsPacks[iGen], TDIsPacksFact[iGen], TDIQuick);
	}
	
	if(TDIAE)
		NbTDI = 2;
	
	tDat = (double**)malloc(NbTDI*sizeof(double*));
	for(int i=0; i<NbTDI; i++)
		tDat[i] = (double*)malloc(NbtDat*sizeof(double));
	tmptDat = (double*)malloc(NbtDat*sizeof(double));
	tmpfDat = (fftw_complex*)malloc(NbfDat*sizeof(fftw_complex));
	FwdPlan = fftw_plan_dft_r2c_1d(NbtDat, tmptDat, tmpfDat, FFTW_ESTIMATE);
	
	NoUsed = false;
	init();
}


void LISACodeLightClass::init()
{
	double t;
	if(NoUsed)
		initFirst();
	for(int iGW=0; iGW<GWs->size(); iGW++)
		(*GWs)[iGW]->init();
	
	SigGWs->init(GWs, LISAGeo);
	
	t = t0 - tMemTDI - tTDIShift;
	while(t<t0+tTDIShift){
		RunLISA(t);
		t += dt;
	}
}



void LISACodeLightClass::RunLISA(double t)
{
	// Compute and store signal for each spacecraft
	SCSig->ReceiveData(0, SigGWs->deltanu(1, 2, 0, t));
	SCSig->ReceiveData(1, SigGWs->deltanu(2, 3, 0, t));
	SCSig->ReceiveData(2, SigGWs->deltanu(3, 1, 0, t));
	SCSig->ReceiveData(3, SigGWs->deltanu(1, 3, 0, t));
	SCSig->ReceiveData(4, SigGWs->deltanu(2, 1, 0, t));
	SCSig->ReceiveData(5, SigGWs->deltanu(3, 2, 0, t));
	SCSig->RecordAccData(dt, t);
	// Compute and store delays
	DelayTDI->ReceiveData(0, -1.0*LISAGeo->gtdelay(3, 2, 0, t));
	DelayTDI->ReceiveData(1, -1.0*LISAGeo->gtdelay(1, 3, 0, t));
	DelayTDI->ReceiveData(2, -1.0*LISAGeo->gtdelay(2, 1, 0, t));
	DelayTDI->ReceiveData(3, -1.0*LISAGeo->gtdelay(2, 3, 0, t));
	DelayTDI->ReceiveData(4, -1.0*LISAGeo->gtdelay(3, 1, 0, t));
	DelayTDI->ReceiveData(5, -1.0*LISAGeo->gtdelay(1, 2, 0, t));
	DelayTDI->RecordAccData(dt, t);
	//cout << " RunLISA at " <<  t << " : ";
	//cout << SCSig->gData(0, 0.0) << " " << SCSig->gData(1, 0.0) << " " << SCSig->gData(2, 0.0) << " || ";
	//cout << DelayTDI->gData(0, 0.0) << " " << DelayTDI->gData(1, 0.0) << " " << DelayTDI->gData(2, 0.0);
	//cout << endl;
}


void LISACodeLightClass::ComputeTDI(double ** &tDatExt)
{
	double t, tRefreshDelay(0.0);
	double Xtmp, Ytmp, Ztmp;
	
	t = t0+tTDIShift;
	for (int iT=0; iT<NbtDat; iT++) {
		RunLISA(t);
		
		//for(int i=0; i<6; i++)
		//	tDat[i][iT] = SCSig->gData(i, 0.0);
		//for(int i=0; i<6; i++)
		//	tDat[i+6][iT] = DelayTDI->gData(i, 0.0);
		
		TDIQuick->RefreshDelay(tTDIShift);
		//cout << "*************** t = " << t+tTDIShift << endl;
		if(TDIAE){
			Xtmp = TDIGens[0]->ComputeNoEta(tTDIShift);
			Ytmp = TDIGens[1]->ComputeNoEta(tTDIShift);
			Ztmp = TDIGens[2]->ComputeNoEta(tTDIShift);
			tDatExt[0][iT] = (2.0*Xtmp-Ytmp-Ztmp)/3.0;
			tDatExt[1][iT] = (Ztmp-Ytmp)/sqrt(3.0);
		}else{
			for(int iTDI=0; iTDI<NbTDI; iTDI++){
				tDatExt[iTDI][iT] = TDIGens[iTDI]->ComputeNoEta(tTDIShift);
				//cout << " " << tDatExt[iTDI][iT];
			}
		}
		//cout << endl;
		t += dt;
	}
}

void LISACodeLightClass::ComputeFTTDI(fftw_complex ** &fDatExt)
{
	ComputeTDI(tDat);
	for(int iTDI=0; iTDI<NbTDI; iTDI++){
		memcpy(tmptDat, tDat[iTDI], NbtDat*sizeof(double));
		fftw_execute(FwdPlan);
		memcpy(fDatExt[iTDI], tmpfDat, NbtDat*sizeof(double));
	}
}

void LISACodeLightClass::TDINewParams(double ** &tDatExt, double * NewParams)
{
	//cout << " Change parameters ... "; fflush(stdout);
	ChangeParamsGWs(NewParams);
	//cout << " Initialization ... "; fflush(stdout);
	init();
	//cout << " Compute TDI ... "; fflush(stdout);
	ComputeTDI(tDatExt);
	//cout << " --> OK" << endl;
	
}

void LISACodeLightClass::FTTDINewParams(fftw_complex ** &fDatExt, double * NewParams)
{
	ChangeParamsGWs(NewParams);
	init();
	ComputeFTTDI(fDatExt);
	
}


void LISACodeLightClass::CountNbGWsParams()
{
	NbParams = 0;
	for(int iGW=0; iGW<GWs->size(); iGW++)
		NbParams += (*GWs)[iGW]->getNParam();
}



void LISACodeLightClass::AddTDI(char * generatorname)
{
	bool UnKnowTDI(true);
	vector<int> tmp_TDIPacks(0);
	vector<double> tmp_TDIPacksFact(0);
	
	
	// **** TDI generator without factor applied on packs
	if((strcmp(generatorname,"Alpha")==0)||(strcmp(generatorname,"alpha")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(-1);
		tmp_TDIPacks.push_back(-32);
		tmp_TDIPacks.push_back(-133);
		tmp_TDIPacks.push_back(4);
		tmp_TDIPacks.push_back(455);
		tmp_TDIPacks.push_back(56);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	if((strcmp(generatorname,"Beta")==0)||(strcmp(generatorname,"beta")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(-121);
		tmp_TDIPacks.push_back(-2);
		tmp_TDIPacks.push_back(-13);
		tmp_TDIPacks.push_back(64);
		tmp_TDIPacks.push_back(5);
		tmp_TDIPacks.push_back(566);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	if((strcmp(generatorname,"Gamma")==0)||(strcmp(generatorname,"gamma")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(-21);
		tmp_TDIPacks.push_back(-232);
		tmp_TDIPacks.push_back(-3);
		tmp_TDIPacks.push_back(464);
		tmp_TDIPacks.push_back(45);
		tmp_TDIPacks.push_back(6);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	if((strcmp(generatorname,"Zeta")==0)||(strcmp(generatorname,"zeta")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(-11);
		tmp_TDIPacks.push_back(-22);
		tmp_TDIPacks.push_back(-33);
		tmp_TDIPacks.push_back(44);
		tmp_TDIPacks.push_back(55);
		tmp_TDIPacks.push_back(66);
		if(1>NbMaxDelays)
			NbMaxDelays = 1;
	}
	if((strcmp(generatorname,"X1s1")==0)||(strcmp(generatorname,"X")==0)||(strcmp(generatorname,"Xf")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"X1s2")==0)||(strcmp(generatorname,"Y")==0)||(strcmp(generatorname,"Yf")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"X1s3")==0)||(strcmp(generatorname,"Z")==0)||(strcmp(generatorname,"Zf")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"X2s1")==0)||(strcmp(generatorname,"X2")==0)||(strcmp(generatorname,"Xf2")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(36524);
		tmp_TDIPacks.push_back(365253);
		tmp_TDIPacks.push_back(3652521);
		tmp_TDIPacks.push_back(36525235);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacks.push_back(-52361);
		tmp_TDIPacks.push_back(-523635);
		tmp_TDIPacks.push_back(-5236364);
		tmp_TDIPacks.push_back(-52363653);
		if(7>NbMaxDelays)
			NbMaxDelays = 7;
	}
	if((strcmp(generatorname,"X2s2")==0)||(strcmp(generatorname,"Y2")==0)||(strcmp(generatorname,"Yf2")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(14635);
		tmp_TDIPacks.push_back(146361);
		tmp_TDIPacks.push_back(1463632);
		tmp_TDIPacks.push_back(14636316);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacks.push_back(-63142);
		tmp_TDIPacks.push_back(-631416);
		tmp_TDIPacks.push_back(-6314145);
		tmp_TDIPacks.push_back(-63141461);
		if(7>NbMaxDelays)
			NbMaxDelays = 7;
	}
	if((strcmp(generatorname,"X2s3")==0)||(strcmp(generatorname,"Z2")==0)||(strcmp(generatorname,"Zf2")==0)){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(25416);
		tmp_TDIPacks.push_back(254142);
		tmp_TDIPacks.push_back(2541413);
		tmp_TDIPacks.push_back(25414124);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacks.push_back(-41253);
		tmp_TDIPacks.push_back(-412524);
		tmp_TDIPacks.push_back(-4125256);
		tmp_TDIPacks.push_back(-41252542);
		if(7>NbMaxDelays)
			NbMaxDelays = 7;
	}
	// Beacon
	if(strcmp(generatorname,"P1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(25);
		tmp_TDIPacks.push_back(-63);
		tmp_TDIPacks.push_back(-22);
		tmp_TDIPacks.push_back(66);
		tmp_TDIPacks.push_back(642);
		tmp_TDIPacks.push_back(-216);
		tmp_TDIPacks.push_back(1463);
		tmp_TDIPacks.push_back(-1425);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"Q1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(36);
		tmp_TDIPacks.push_back(-41);
		tmp_TDIPacks.push_back(-33);
		tmp_TDIPacks.push_back(44);
		tmp_TDIPacks.push_back(453);
		tmp_TDIPacks.push_back(-324);
		tmp_TDIPacks.push_back(2541);
		tmp_TDIPacks.push_back(-2536);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"R1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(14);
		tmp_TDIPacks.push_back(-52);
		tmp_TDIPacks.push_back(-11);
		tmp_TDIPacks.push_back(55);
		tmp_TDIPacks.push_back(561);
		tmp_TDIPacks.push_back(-135);
		tmp_TDIPacks.push_back(3652);
		tmp_TDIPacks.push_back(-3614);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	// Monitor
	if(strcmp(generatorname,"E1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(542);
		tmp_TDIPacks.push_back(56);
		tmp_TDIPacks.push_back(-316);
		tmp_TDIPacks.push_back(-32);
		tmp_TDIPacks.push_back(-144);
		tmp_TDIPacks.push_back(141);
		tmp_TDIPacks.push_back(4);
		tmp_TDIPacks.push_back(-1);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	if(strcmp(generatorname,"F1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(653);
		tmp_TDIPacks.push_back(64);
		tmp_TDIPacks.push_back(-124);
		tmp_TDIPacks.push_back(-13);
		tmp_TDIPacks.push_back(-255);
		tmp_TDIPacks.push_back(252);
		tmp_TDIPacks.push_back(5);
		tmp_TDIPacks.push_back(-2);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	if(strcmp(generatorname,"G1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(461);
		tmp_TDIPacks.push_back(45);
		tmp_TDIPacks.push_back(-235);
		tmp_TDIPacks.push_back(-21);
		tmp_TDIPacks.push_back(-366);
		tmp_TDIPacks.push_back(363);
		tmp_TDIPacks.push_back(6);
		tmp_TDIPacks.push_back(-3);
		if(2>NbMaxDelays)
			NbMaxDelays = 2;
	}
	// Relay
	if(strcmp(generatorname,"U1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1464);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-64);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(-6542);
		tmp_TDIPacks.push_back(-656);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"V1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2545);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-45);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(-4653);
		tmp_TDIPacks.push_back(-464);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"W1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3656);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-56);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(-5461);
		tmp_TDIPacks.push_back(-545);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"Zeta1")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(3214);
		tmp_TDIPacks.push_back(-414);
		tmp_TDIPacks.push_back(-3252);
		tmp_TDIPacks.push_back(452);
		tmp_TDIPacks.push_back(3255);
		tmp_TDIPacks.push_back(-455);
		tmp_TDIPacks.push_back(-5633);
		tmp_TDIPacks.push_back(133);
		tmp_TDIPacks.push_back(5636);
		tmp_TDIPacks.push_back(-136);
		tmp_TDIPacks.push_back(-5641);
		tmp_TDIPacks.push_back(141);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"Zeta2")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(1325);
		tmp_TDIPacks.push_back(-525);
		tmp_TDIPacks.push_back(-1363);
		tmp_TDIPacks.push_back(563);
		tmp_TDIPacks.push_back(1366);
		tmp_TDIPacks.push_back(-566);
		tmp_TDIPacks.push_back(-6411);
		tmp_TDIPacks.push_back(211);
		tmp_TDIPacks.push_back(6414);
		tmp_TDIPacks.push_back(-214);
		tmp_TDIPacks.push_back(-6452);
		tmp_TDIPacks.push_back(252);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if(strcmp(generatorname,"Zeta3")==0){
		UnKnowTDI = false;
		tmp_TDIPacks.push_back(2136);
		tmp_TDIPacks.push_back(-636);
		tmp_TDIPacks.push_back(-2141);
		tmp_TDIPacks.push_back(641);
		tmp_TDIPacks.push_back(2144);
		tmp_TDIPacks.push_back(-644);
		tmp_TDIPacks.push_back(-4522);
		tmp_TDIPacks.push_back(322);
		tmp_TDIPacks.push_back(4525);
		tmp_TDIPacks.push_back(-325);
		tmp_TDIPacks.push_back(-4563);
		tmp_TDIPacks.push_back(363);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	
	if(!UnKnowTDI){
		tmp_TDIPacksFact.resize(tmp_TDIPacks.size(),1.0);
	}
	
	// **** TDI generator with factor applied on packs
	if((strcmp(generatorname,"A")==0)||(strcmp(generatorname,"Aa")==0)||(strcmp(generatorname,"AX")==0)){
		UnKnowTDI = false;
		// Packs of X * -1/sqrt(2)
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,-1.0/sqrt(2.0));
		// Pack of Z * -1/3
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(16,-1.0/sqrt(2.0));
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"E")==0)||(strcmp(generatorname,"Ea")==0)||(strcmp(generatorname,"EX")==0)){
		UnKnowTDI = false;
		// Packs of X * 1.0/sqrt(6.0)
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,1.0/sqrt(6.0));
		// Packs of Y * -2.0/sqrt(6.0)
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(16,-2.0/sqrt(6.0));
		// Pack of Z * 1.0/sqrt(6.0)
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(24,1.0/sqrt(6.0));
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"T")==0)||(strcmp(generatorname,"Ta")==0)||(strcmp(generatorname,"TX")==0)){
		UnKnowTDI = false;
		// Packs of X * 1.0/sqrt(3.0)
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,1.0/sqrt(3.0));
		// Packs of Y * 1.0/sqrt(3.0)
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(16,1.0/sqrt(3.0));
		// Pack of Z * 1.0/sqrt(3.0)
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(24,1.0/sqrt(3.0));
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"Ab")==0)||strcmp(generatorname,"AMLDC")==0){
		UnKnowTDI = false;
		// Packs of X * 2/3
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,2.0/3.0);
		// Packs of Y * -1/3
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(16,-1.0/3.0);
		// Pack of Z * -1/3
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(24,-1.0/3.0);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"Eb")==0)||(strcmp(generatorname,"EMLDC")==0)){
		UnKnowTDI = false;
		// Packs of Y * -1/sqrt(3)
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(8,-1.0/sqrt(3.0));
		// Pack of Z * 1/sqrt(3)
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(16,1.0/sqrt(3.0));
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	if((strcmp(generatorname,"Tb")==0)||(strcmp(generatorname,"TMLDC")==0)){
		UnKnowTDI = false;
		// Packs of X * -sqrt(2)/3
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,-sqrt(2.0)/3.0);
		// Packs of Y * -sqrt(2)/3
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(16,-sqrt(2.0)/3.0);
		// Pack of Z * -sqrt(2)/3
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(24,-sqrt(2.0)/3.0);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	
	if(!UnKnowTDI){
		TDIsPacks.push_back(tmp_TDIPacks);
		TDIsPacksFact.push_back(tmp_TDIPacksFact);
	}
	
}


void LISACodeLightClass::AddTDI_AEMLDC()
{
	TDIAE = true;
	AddTDI("X");
	AddTDI("Y");
	AddTDI("Z");
	NbTDI = 2;
}



// end of LISACode-LISACodeLightClass.cpp