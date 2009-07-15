// $Id: LISACODE-ConfigSim.cpp,v 1.8 2007/03/29 15:17:01 lalanne Exp $
/*
 *  LISACODE-ConfigSim.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 30/06/05 by A.Petiteau (APC)
 *  Last modification on 06/06/06 by A.Petiteau (APC)
 *
 */

#include "LISACODE-ConfigSim.h"
/* Constructor */
/*! \brief Base constructor.
 *
 * It sets the default configuration file from which all execution (simulation)
 * parameters are set. Base paremeters are set by calling #DefaultConfig, 
 * the others are read from the file by #ReadFile.
 */ 

ConfigSim::ConfigSim()
{
	//It sets "ConfigBase" as configuration file
	//and set all basical simulation parameters
	DefaultConfig("ConfigBase");
	//Read parameters from the file "ConfigBase"
	ReadFile();
    
}
/*! \brief Constructor setting configuration file.
 *
 * It takes the configuration file given in input to set all execution (simulation)
 * parameters. The behaviour is Base paremeters are set by calling #DefaultConfig, 
 * the others are read from the file by #ReadFile.
 * /param NameConfigFile_n: configuration file name.
 */ 
ConfigSim::ConfigSim(char * NameConfigFile_n)
{
	//It sets input as configuration file
	//and set all basical simulation parameters
	DefaultConfig(NameConfigFile_n);
	//Read parameters from the input file (NameConfigFile_n). 
	//The input file name is stored in variable ConfigFileName
	ReadFile();
	
}


/* Destructor */
/*! \brief Destructor.
 *
 * It does not do any particular action.
 */
ConfigSim::~ConfigSim()
{
	
}


/* Access methods */
/*!\brief It sets default simulation parameters.
 *
 * The default parameters are:
 * \arg DefVectNoise = 0
 * \arg tStepPhy = 0.5
 * \arg tMax = 65736.0 
 * \arg tStepMes = 1.0
 * \arg tMemNoiseFirst = 5.0
 * \arg tMemNoiseLast = -30.0
 * \arg tMemSig = 100.0
 * \arg tDisplay = 1000.0
 * \arg tDeltaTDIDelay = 0.0
 * \arg TDIInterp = LAG
 * \arg TDIInterpVal = 20
 * \arg Armlength = #L0_m_default
 * \arg OrbStartTime = 0.0
 * \arg OrbInitRot = 0.0
 * \arg OrbMove = 1
 * \arg OrbOrder = 2
 * \arg TDIDelayApprox = false
 * \arg LaserPower = LaserPower_W_default
 * \arg PhaMetFilterON = true
 * \arg Phasemeter Filter Parameters :
 * \arg attenuation : PhaMetFilterParam[0]= 180.0 [dB]
 * \arg oscillations in bandwidth : PhaMetFilterParam[1]= 0.1 [dB]
 * \arg low transition frequency in factor of frequency of measurment : PhaMetFilterParam[2]= 0.1 
 * \arg high transition frequency in factor of frequency of measurment : PhaMetFilterParam[3]= 0.3
 * \arg filename for Space Craft 1 Signal : FileNameSigSC1= "None"
 * \arg filename for Space Craft 2 Signal : FileNameSigSC2= "None"
 * \arg filename for Space Craft 3 Signal : =FileNameSigSC3 "None"
 * \arg filename for delays : FileNameDelays= "None"
 * \arg fileNameP for positions : FileNamePositions= "None"
 * \arg filename for TDI generators : FileNameTDI= "Def_SignalTDI.txt"
 * \arg GWB = NULL
 * \arg NoisesData = 24 NULL vectors
 * \arg USOs = 3 USOClock instances set to 0.0
 * \arg NbMaxDelays = 0
 * \arg ConfigFileName = NameConfigFile_n input
 *
 */
void ConfigSim::DefaultConfig(char * NameConfigFile_n)
{
	vector<NoiseSpec> DefVectNoise(0);
    // Default Values
	strcpy(XmlOutputFile,"None");
	GlobalRandomSeed = -1;
    tStepPhy = 0.5;
	tOffset = 0.0;
	settMax(65736.0);
    tStepMes = 1.0;
    tMemNoiseFirst = 5.0;
	tMemNoiseLast = -30.0;
    tMemSig = 100.0;
	tDisplay = -1.0;
	tDeltaTDIDelay = 0.0;
	TDIInterp = LAG;
	TDIInterpVal = 20;
	Armlength = L0_m_default;
	OrbStartTime = 0.0;
	OrbInitRot = 0.0;
	OrbType = 1; // MLDC
	OrbApprox = 1; // rigid
	OrbOrder = 2;
	TDIDelayApprox = false;
	LaserPower = LaserPower_W_default;
	PhaMetFilterON = true;
	PhaMetFilterParam.resize(4);
	PhaMetFilterParam[0] = 180.0;
	PhaMetFilterParam[1] = 0.1;
	PhaMetFilterParam[2] = 0.1;
	PhaMetFilterParam[3] = 0.3;
	InternalPhasemeters = true;
    strcpy(FileNameSigSC1, "None");
    strcpy(FileNameSigSC2, "None");
    strcpy(FileNameSigSC3, "None");
	strcpy(FileNameDelays, "None");
	strcpy(FileNamePositions, "None");
	//strcpy(FileNameTDI,"Def_SignalTDI.txt");
    strcpy(FileNameTDI,"None");
	FileEncodingSC1 = 0;
	FileEncodingSC2 = 0;
	FileEncodingSC3 = 0;
	FileEncodingTDI = 0;
	FileEncodingDelays = 0;
	FileEncodingPos = 0;
	GWB = NULL;
	NoisesData.resize(24,DefVectNoise);
    Noises.resize(24, NULL);
	for(int i=0; i<3; i++)
		USOs.push_back(USOClock(0.0));
	NbMaxDelays = 0;
    ConfigFileName = NameConfigFile_n;
    Endian = 0 ;
	Simulator = "LISACode";
	strcpy(OrbFile, "Orbit_ESA.input.txt");
	
}

/*!\brief
 * Returns FileNameSig corresponding to iSC input.
 *
 * iSC : spacecraft number (expected values : 1, 2, 3)
 */
void ConfigSim::getGeometry(Geometry * & SCPos)
{
	SCPos = NULL;
	switch (OrbType){
		case 0:
			SCPos = new GeometryAnalytic(OrbStartTime, OrbInitRot, Armlength, OrbOrder, OrbApprox, tStepPhy);
			break;
		case 1:
			SCPos = new GeometryMLDC(OrbStartTime, OrbInitRot, Armlength, OrbOrder, OrbApprox, tStepPhy);
			break;
		case 2:
			SCPos = new GeometryFile(OrbStartTime, OrbInitRot, Armlength, OrbOrder, OrbApprox, tStepPhy, OrbFile);
			break;
		default:
			SCPos = new GeometryAnalytic(OrbStartTime, OrbInitRot, Armlength, OrbOrder, OrbApprox, tStepPhy);
	};
}



/*!\brief
 * Returns FileNameSig corresponding to iSC input.
 *
 * iSC : spacecraft number (expected values : 1, 2, 3)
 */

char * ConfigSim::getFileNameSig(int iSC)
{
	switch (iSC){
		case 1 :
			return(FileNameSigSC1);
			break;
		case 2 :
			return(FileNameSigSC2);
			break;
		case 3 :
			return(FileNameSigSC3);
			break;
		default :
			throw invalid_argument("ConfigSim::ReadFile : Bad spacecraft number !");
	};
	
	return(" ");
}

/*!\brief
 * Returns FileEncodingSig corresponding to iSC input.
 *
 * iSC : spacecraft number (expected values : 1, 2, 3)
 */

int ConfigSim::getFileEncodingSig(int iSC)
{
	switch (iSC){
		case 1 :
			return(FileEncodingSC1);
			break;
		case 2 :
			return(FileEncodingSC2);
			//return(FileEncodingSC2);
			break;
		case 3 :
			return(FileEncodingSC3);
			break;
		default :
			throw invalid_argument("ConfigSim::ReadFile : Bad spacecraft number !");
	};
	
	return(0);
}

/*!\brief
 * Returns NameGenTDI corresponding to iGen input.
 *
 * iGen : TDI generator index (expected values : [0, size of #TDIsPacks[)
 *
 */

char * ConfigSim::getNameGenTDI(int iGen)
{
	
	if((iGen<0)||(iGen>=TDIsPacks.size()))
		throw invalid_argument("ConfigSim::getGenTDIPacks : This TDI generator doesn't exist !");
	// Count the number of characters 
	size_t ssize = (TDIsName[iGen]).size() + 1;
	// Creation of buffer for copy string of characters
	char * Name;
	Name = new char[ssize];
	// Copy the string
	strncpy(Name, (TDIsName[iGen]).c_str(), ssize);
	return(Name);
}

/*!\brief
 * Returns TDIsPack corresponding to iGen input.
 *
 * iGen : TDI generator index (expected values : [0, size of #TDIsPacks[)
 *
 */
vector<int> ConfigSim::getGenTDIPacks(int iGen)
{
    if((iGen<0)||(iGen>=TDIsPacks.size()))
        throw invalid_argument("ConfigSim::getGenTDIPacks : This TDI generator doesn't exist ! ");
    return(TDIsPacks[iGen]);
}

/*!\brief
 * Returns TDIsPackFact corresponding to iGen input.
 *
 * iGen : TDI generator index (expected values : [0, size of #TDIsPacksFact)
 *
 */
vector<double> ConfigSim::getGenTDIPacksFact(int iGen)
{
    if((iGen<0)||(iGen>=TDIsPacksFact.size()))
        throw invalid_argument("ConfigSim::getGenTDIPacks : This TDI generator doesn't exist ! ");
    return(TDIsPacksFact[iGen]);
}


void ConfigSim::settMax(double tMax_n)
{
	tMax = tMax_n;
	tDur = tMax - tOffset;
}


void ConfigSim::settDur(double tDur_n)
{
	tDur = tDur_n;
	tMax = tOffset + tDur;
}

/*****************/
/* Other methods */
/*****************/
/*!\brief
 * Reads configuration file
 *
 * Calls ReadXMLFile or ReadASCIIFile depending on #ConfigFileName type
 */
void ConfigSim::ReadFile()
{
	char * ptr;
	
	// Detect system encoding type
	Endian = testbyteorder();       
	if(Endian == 0) {
		BigEndian = true;
		SystemEncoding = "BigEndian";
	} else {
		BigEndian = false;
		SystemEncoding = "LittleEndian";
	}
	cout << endl << "System Encoding for Binary files is  " <<  getSystemEncoding() << endl << endl ;
	
	// Read extention of configuration file
	ptr=ConfigFileName+strlen(ConfigFileName);
	//cout << " filename = " << ConfigFileName << endl;
	//cout << strncmp(ptr-4,".xml",4) << endl;
	
	// Read configuration file
	if(strncmp(ptr-4,".xml",4)==0) {
		ReadXMLFile();
	}else{
		ReadASCIIFile();
	}
	
	//ReadASCIIFile();
}
/*void ConfigSim::ReadFile()
 {
 char * extention;
 char * ptr;
 extention=(char*)malloc(16*sizeof(char));
 ptr=strchr(ConfigFileName,'\0');
 if(ptr!=NULL) strncat(extention,ptr-4,16);
 //cout << " Extention = " << extention << endl;
 //cout << strcmp(extention,".xml") << endl;
 if(strncmp(extention,".xml",4)==0) {
 ReadXMLFile();
 }else{
 ReadASCIIFile();
 }
 
 //ReadASCIIFile();
 }*/

/*!\brief
 * Reads ASCII configuration file
 *
 * Data are read :
 * \arg temporal
 * \arg interpolation
 * \arg accuracy
 * \arg orbits
 * \arg detector
 * \arg gravitational waves background
 * \arg record
 * \arg gravitational waves
 * \arg noises
 * \arg USO Clocks
 * \arg Phasemeter
 * \arg TDI generators
 */
void ConfigSim::ReadASCIIFile()
{
	cout << " Read ASCII configuration file : " << ConfigFileName << endl;
	
    ifstream ConfigFile;
    string Buf;
    bool Readable;
	bool ContinueRead(true);
	int nReadCmd(0);
    ConfigFile.open(ConfigFileName);
	int NbSrc(0);
	
    if (!ConfigFile){
		cerr << "ConfigSim::ReadFile : Error to open the file  " << ConfigFileName << endl;
		exit(1);
	}
	
	while((!ConfigFile.eof())&&(ContinueRead)) {
		nReadCmd++;
		Readable = false;	
		ConfigFile >> Buf;
		if(Buf == "#") { // Commentaire
			Readable = true;
			//cout << "Comment : ";
			do{
				ConfigFile >> Buf;
				//cout << Buf << " ";
			}while((Buf != ";")&&(!ConfigFile.eof()));
			//cout << endl;
		}
		
		/* Informations temporelles 
		 ---------------------------*/
		if(scmp(Buf,"Time")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(scmp(Buf,"TimeOffset")){
				ConfigFile >> tOffset;
				//cout << " Pas en temps = " << tStepPhy << endl;
			}
			if(scmp(Buf,"StepPhysic")){
				ConfigFile >> tStepPhy;
				//cout << " Pas en temps = " << tStepPhy << endl;
			}
			if(scmp(Buf,"StepMeasure")){
				ConfigFile >> tStepMes;
				//cout << " Duree d'une mesure = " << tStepMes << endl;
			}
			if(scmp(Buf,"Max")){
				ConfigFile >> tMax;
				settMax(tMax);
				//cout << " Temps max = " << tMax << endl;
			}
			if(scmp(Buf,"NoiseStoreFirst")){
				//ConfigFile >> tMemNoiseFirst;
				//cout << " Temps relatif de debut de memorisation du bruit = " << tMemNoiseFirst << endl;
				ConfigFile >> Buf;
				cout << " ConfigSim::ReadFile : Warning : NoiseStoreFirst is not use in this new version !" << endl;
			}
			if(scmp(Buf,"NoiseStoreLast")){
				//ConfigFile >> tMemNoiseLast;
				//cout << " Temps relatif de fin de memorisation du bruit = " << tMemNoiseLast << endl;
				ConfigFile >> Buf;
				cout << " ConfigSim::ReadFile : Warning : NoiseStoreLast is not use in this new version !" << endl;
			}			
			if(scmp(Buf,"SignalStore")){
				ConfigFile >> tMemSig;
				//cout << " Duree de memorisation du signal = " << tMemSig << endl;
			}
			if(scmp(Buf,"StepDisplay")){
				ConfigFile >> tDisplay;
				//cout << " Pas d'affichage = " << tDisplay<< endl;
			}
			if(scmp(Buf,"DeltaTDIDelay")){
				ConfigFile >> tDeltaTDIDelay;
				//cout << " Pas d'affichage = " << tDisplay<< endl;
			}
			ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
		}
		
		
		
		/* Informations interpolation 
		 ---------------------------*/
		if(scmp(Buf,"Interpolation")){
			TDIInterpVal = 0.0;
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(scmp(Buf,"LAG")){
				ConfigFile >> TDIInterpVal;
				TDIInterp = LAG;
			}
			ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
		}
		/* Informations la precision 
		 ---------------------------*/
		if(scmp(Buf,"TDIDelayApprox")){
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(scmp(Buf,"ON")){
				TDIDelayApprox = true;
			}else{
				if(scmp(Buf,"OFF")){
					TDIDelayApprox = false;
				}else{
					throw invalid_argument("ConfigSim::ReadFile : Bad syntax for approximation of TDI Delay (ON or OFF) !");
				}
			}
			ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
			cout << "  Approximation of TDI Delay = " << TDIDelayApprox << endl;
		}
		
		
		/* Informations sur les orbites 
		 ------------------------------*/
		if(scmp(Buf,"Orbits")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture de l'element bruite
			if(scmp(Buf,"Armlength")){
				ConfigFile >> Armlength;
				ConfigFile >> Buf ;
			}
			if(scmp(Buf,"StartTime")){
				ConfigFile >> OrbStartTime;
				ConfigFile >> Buf ;
			}
			if(scmp(Buf,"InitialRotation")){
				ConfigFile >> OrbInitRot;
				ConfigFile >> Buf ;
			}
			if(scmp(Buf,"Move")){
				ConfigFile >> Buf;
				if(scmp(Buf, "On")){
					OrbType = 0;
					OrbApprox = 2;
				}else{
					if(scmp(Buf,"Off")){
						OrbType = 0;
						OrbApprox = 0;
					}else{
						if(scmp(Buf,"MLDC_ON")){
							OrbType = 1;
							OrbApprox = 2;
						}else{
							if(scmp(Buf,"MLDC_OFF")){
								OrbType = 1;
								OrbApprox = 0;
							}else{
								if(scmp(Buf,"ESA_ON")){
									OrbType = 2;
									OrbApprox = 2;
								}else{
									if(scmp(Buf,"ESA_OFF")){
										OrbType = 2;
										OrbApprox = 0;
									}else{
										throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the orbit move specification (ON, OFF, MLDC_ON, MLDC_OFF, ESA_ON or ESA_OFF) !");
									}
								}
							}
						}
					}
				}
				ConfigFile >> Buf ;
				cout << "  OrbType =  " << OrbType << endl ;
				cout << "  OrbApprox =  " << OrbApprox << endl ;
			}	
			if(scmp(Buf,"Type")){
				ConfigFile >> Buf;
				if(scmp(Buf,"LISACode_Orbits")||scmp(Buf,"LISACode"))
					OrbType = 0;
				if(scmp(Buf,"PseudoLISA")||scmp(Buf,"MLDC"))
					OrbType = 1;
				if(scmp(Buf,"ESA_Orbits")||scmp(Buf,"ESA"))
					OrbType = 2;
				ConfigFile >> Buf;
				cout << "  OrbType =  " << OrbType << endl ;
			}
			
			if(scmp(Buf,"Approx")){
				ConfigFile >> Buf;
				if(scmp(Buf,"Static")||scmp(Buf,"static"))
					OrbApprox = 0;
				if(scmp(Buf,"Rigid")||scmp(Buf,"rigid"))
					OrbApprox = 1;
				if(scmp(Buf,"Eccentric")||scmp(Buf,"eccentric"))
					OrbApprox = 2;
				ConfigFile >> Buf;
				cout << "  OrbApprox =  " << OrbApprox << endl ;
			}
			
			if(scmp(Buf,"Order")){
				ConfigFile >> OrbOrder;
				ConfigFile >> Buf ;
			}
			if(Buf == ";"){
				Readable = true;
			}
		}		
		
		/* Informations sur le detecteur 
		 ------------------------------*/
		if(scmp(Buf,"Detector")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture du parametre du detecteur
			if(scmp(Buf,"LaserPower")){
				ConfigFile >> LaserPower;
				ConfigFile >> Buf ;
			}
			if(scmp(Buf,"PhaMetFilter")){
				ConfigFile >> Buf;
				if(scmp(Buf,"On")){
					PhaMetFilterON = true;
				}else{
					if(scmp(Buf,"Off")){
						PhaMetFilterON = false;
					}else{
						throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the phasemeter filter specification (ON or OFF) !");
					}
				}
				ConfigFile >> Buf ;
				cout << "  Phasemeter filter : " << PhaMetFilterON << endl;
			}
			if(scmp(Buf,"PhaMetFilterParameters")){
				ConfigFile >> Buf;  // Lecture de :
				ConfigFile >> Buf;
				do{
					if(scmp(Buf,"attenuation")){
						ConfigFile >> PhaMetFilterParam[0];
						//cout << " Frequency = " << freq << endl;
					}
					if(scmp(Buf,"oscillation")){
						ConfigFile >> PhaMetFilterParam[1];
						//cout << " h+ = " << hp << endl;
					}
					if(scmp(Buf,"FactFmesForHighFreq")){
						ConfigFile >> PhaMetFilterParam[2];
						//cout << " hx = " << hc << endl;
					}
					if(scmp(Buf,"FactFmesForLowFreq")){
						ConfigFile >> PhaMetFilterParam[3];
						//cout << " Phi0hp = " << Phi0hp << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}			
			if(scmp(Buf,"InternalPhasemeters")){
				ConfigFile >> Buf;
				if(scmp(Buf,"Always")){
					InternalPhasemeters = true;
				}else{
					if(scmp(Buf,"IfNoises")){
						InternalPhasemeters = false;
					}else{
						throw invalid_argument("ConfigSim::ReadASCIIFile : The InternalPhasemeters arguments must be 'Always' or 'IfNoises' (Ex : 'Detector : InternalPhasemeters Always ;' ) !");
					}
				}
				ConfigFile >> Buf;
			}
			
			if(Buf == ";"){
				Readable = true;
			}
		}
		
		
		/* Informations sur le Fond d'ondes gravitationnelles 
		 -------------------------------------------------*/
		if(scmp(Buf,"GWB")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture du parametre du detecteur
			if(scmp(Buf,"File")){
				char FileName[256];
				double Factor(0.0);
				//cout << "GWB Fichier : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				ConfigFile >> FileName;
				//cout << " Nom du fichier contenant l'onde = " << FileName << endl;
				ConfigFile >> Factor;
				//cout << Factor << endl;
				ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
				if(Buf == ";"){
					Readable = true;
				}
				cout << "  Loading gravitationnal wave background signal from " << FileName << " ... " << endl;
				GWB = new BackgroundGalactic(FileName, Factor);
				cout << "  Loading gravitationnal wave background signal --> OK !" << endl;
			}
		}
		
		
		
		/* Informations sur l'enregistrement
		 ------------------------------------*/
		if(scmp(Buf,"Record")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(scmp(Buf,"SignalSC")){
				int iSC;
				ConfigFile >> iSC;
				switch (iSC){
					case 1 :
						ConfigFile >> FileNameSigSC1 >> Buf;
						FileEncodingSC1 = 0 ;
						if(scmp(Buf,"BINARY"))
							FileEncodingSC1 = 1 ;
						break;
					case 2 :
						ConfigFile >> FileNameSigSC2  >> Buf;
						FileEncodingSC2 = 0 ;
						if(scmp(Buf,"BINARY"))
							FileEncodingSC2 = 1;
						break;
					case 3 :
						ConfigFile >> FileNameSigSC3  >> Buf;
						FileEncodingSC3 = 0 ;
						if(scmp(Buf,"BINARY"))
							FileEncodingSC3 = 1 ;
						break;
					default :
						throw invalid_argument("ConfigSim::ReadFile : The spacecraft number must be 1, 2 or 3 !");
				};
			}
			if(scmp(Buf,"TDI")){
				ConfigFile >> FileNameTDI >> Buf;
				FileEncodingTDI = 0;
				if(scmp(Buf,"BINARY"))
					FileEncodingTDI = 1;
				//cout << " Fichier d'enregistrement de TDI = " << FileNameTDI << endl;
			}
			if(scmp(Buf,"Delay")){
				ConfigFile >> FileNameDelays >> Buf;
				FileEncodingDelays = 0;
				if(scmp(Buf,"BINARY"))
					FileEncodingDelays = 1;
				//cout << " Fichier d'enregistrement des retards = " << FileNamedelays << endl;
			}
			if(scmp(Buf,"Position")){
				ConfigFile >> FileNamePositions >> Buf;
				FileEncodingPos = 0;
				if(scmp(Buf,"BINARY"))
					FileEncodingPos = 1;
				//cout << " Fichier d'enregistrement des positions = " << FileNamePositions << endl;
			}
			//cout << "-----------------------> " << Buf << endl;
			if(Buf != ";")
				ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
		}
		
		/* Informations sur les ondes gravitationnelles 
		 ------------------------------------------------*/
		if(scmp(Buf,"GW")){
			double Beta(0.0), Lambda(0.0), Psi(0.0);
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// ***** Lecture de Beta, Lambda et Psi
			do{
				if(scmp(Buf,"Bet")){
					double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Beta = MathUtils::deg2rad(tmpAng);
					//cout << " Beta = " << Beta << endl;
				}
				if(scmp(Buf,"Lam")){
					double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Lambda = MathUtils::deg2rad(tmpAng);
					//cout << " Lambda = " << Lambda << endl;
				}
				if(scmp(Buf,"Psi")){
					double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Psi = MathUtils::deg2rad(tmpAng);
					//cout << " Psi = " << Psi << endl;
				}
				ConfigFile >> Buf;
			}while((Buf != ":")&&(!ConfigFile.eof()));
			
			// ***** Lecture du type d'onde
			ConfigFile >> Buf;
			// ** Template for New gravitational wave
			if(scmp(Buf,"New")){
				Readable = true;
				//cout << "OG New : " << endl; 
				ConfigFile >> Buf;
				double freq(0.0), hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
				do{
					if(scmp(Buf,"f")){
						ConfigFile >> freq;
						//cout << " Frequency = " << freq << endl;
					}
					if(scmp(Buf,"hp")){
						ConfigFile >> hp;
						//cout << " h+ = " << hp << endl;
					}
					if(scmp(Buf,"hc")){
						ConfigFile >> hc;
						//cout << " hx = " << hc << endl;
					}
					if(scmp(Buf,"Phi0hp")){
						ConfigFile >> Phi0hp;
						//cout << " Phi0hp = " << Phi0hp << endl;
					}
					if(scmp(Buf,"Phi0hc")){
						ConfigFile >> Phi0hc;
						//cout << " Phi0hc = " << Phi0hc << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWNew   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout.precision(15);
				cout << " f=" << freq << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
				GWs.push_back(new GWNew(Beta, Lambda, Psi, freq, hp, hc, Phi0hp, Phi0hc));
				NbSrc++;
			}
			if(scmp(Buf,"Cusp")){
				Readable = true;
				//cout << "GW Cusp : " << endl; 
				ConfigFile >> Buf;
				double Amplitude(1.0e-10), CentralTime(tMax/2.0), MaximumFrequency(0.1), Tpad(900.0);
				do{
					if(scmp(Buf,"Amplitude")){
						ConfigFile >> Amplitude;
						//cout << " Amplitude = " << Amplitude << endl;
					}
					if(scmp(Buf,"CentralTime")){
						ConfigFile >> CentralTime;
						//cout << " CentralTime = " << CentralTime << endl;
					}
					if(scmp(Buf,"MaximumFrequency")){
						ConfigFile >> MaximumFrequency;
						//cout << " MaximumFrequency = " << MaximumFrequency << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWCusp   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout << " Amplitude=" << Amplitude << " CentralTime=" << CentralTime << " MaximumFrequency=" << MaximumFrequency << endl;
				GWs.push_back(new GWCusp(Beta, Lambda, Psi, Amplitude, CentralTime, MaximumFrequency, tStepPhy, tMax, Tpad, tOffset));
				NbSrc++;
			}
			if(scmp(Buf,"Mono")){
				Readable = true;
				//cout << "OG Monochromatique : " << endl; 
				ConfigFile >> Buf;
				double freq(0.0), hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
				do{
					if(scmp(Buf,"f")){
						ConfigFile >> freq;
						//cout << " Frequency = " << freq << endl;
					}
					if(scmp(Buf,"hp")){
						ConfigFile >> hp;
						//cout << " h+ = " << hp << endl;
					}
					if(scmp(Buf,"hc")){
						ConfigFile >> hc;
						//cout << " hx = " << hc << endl;
					}
					if(scmp(Buf,"Phi0hp")){
						ConfigFile >> Phi0hp;
						//cout << " Phi0hp = " << Phi0hp << endl;
					}
					if(scmp(Buf,"Phi0hc")){
						ConfigFile >> Phi0hc;
						//cout << " Phi0hc = " << Phi0hc << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWMono   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout.precision(15);
				cout << " f=" << freq << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
				GWs.push_back(new GWMono(Beta, Lambda, Psi, freq, hp, hc, Phi0hp, Phi0hc));
				NbSrc++;
			}
			// ** Stochastic gravitational wave
			if(scmp(Buf,"Stochastic")){
				Readable = true;
				//cout << "============   OG Stochastic : " << endl; 
				ConfigFile >> Buf;
				double hp_sto(0.0), hc_sto(0.0), Slope_sto(0.0), Fmin_sto(0.0), Fknee_sto(0.0);
				// tFirst = tStepPhy ;
				// tDurAdd = tStepPhy ; 
				// Tsample_sto = tStepPhy ;
				do{
					if(scmp(Buf,"hp")){
						ConfigFile >> hp_sto;
						//cout << " h+ = " << hp << endl;
					}
					if(scmp(Buf,"hc")){
						ConfigFile >> hc_sto;
						//cout << " hx = " << hc << endl;
					}
					if(scmp(Buf,"Slope")){
						ConfigFile >> Slope_sto;
						//cout << " Slope_sto = " << Slope_sto << endl;
					}
					if(scmp(Buf,"Fmin")){
						ConfigFile >> Fmin_sto;
						//cout << " Fmin_sto = " << Fmin_sto << endl;
					}
					if(scmp(Buf,"Fknee")){
						ConfigFile >> Fknee_sto;
						//cout << " Fknee_sto = " << Fknee_sto << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				Beta = M_PI/2.-Beta ;  //  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! a verifier modif_eric !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				cout << "  + GW Stochastic : " << NbSrc << " : Beta,Lambda,hp,hc,dt,Slope_sto,Fknee_sto,Fmin_sto =    " <<MathUtils::rad2deg(Beta)<< "  " <<MathUtils::rad2deg(Lambda)<< "  " << tStepPhy << "  " << hp_sto<< "  "<< hc_sto<< "  " <<Slope_sto<< "  " <<Fknee_sto<< "  " <<Fmin_sto<< endl ;
				GWs.push_back(new GWSto(Beta,Lambda,tStepMes,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));
				NbSrc++;
			}
			if(scmp(Buf,"PeriGate")){
				Readable = true;
				//cout << "OG Porte periodique : " << endl; 
				ConfigFile >> Buf;
				double freq(0.0), hp(0.0), hc(0.0) ;
				do{
					if(scmp(Buf,"f")){
						ConfigFile >> freq;
						//cout << " Frequency = " << freq << endl;
					}
					if(scmp(Buf,"hp")){
						ConfigFile >> hp;
						//cout << " h+ = " << hp << endl;
					}
					if(scmp(Buf,"hc")){
						ConfigFile >> hc;
						//cout << " hx = " << hc << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				GWs.push_back(new GWPeriGate(Beta, Lambda, Psi, freq, hp, hc));
				NbSrc++;
			}			
			// ** Binary gravitational wave
			if(scmp(Buf,"Binary")){ 
				Readable = true;
				//cout << "OG Binaire : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				double M1(0.5), M2(0.033), forb(1./1028.76), inc(1.53921), phi0(0.0), r(0.1); 
				do{
					if(scmp(Buf,"M1")){
						ConfigFile >> M1;
						//cout << " Masse 1 = " << M1 << endl;
					}
					if(scmp(Buf,"M2")){
						ConfigFile >> M2;
						//cout << " Masse 2 = " << M2 << endl;
					}
					if(scmp(Buf,"forb")){
						ConfigFile >> forb;
						//cout << " Frequence orbital = " << forb << endl;
					}
					if(scmp(Buf,"inc")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						inc = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(scmp(Buf,"phi0")){
						ConfigFile >> phi0;
						//cout << " Phase initial = " << phi0 << endl;
					}
					if(scmp(Buf,"r")){
						ConfigFile >> r;
						//cout << " Distance = " << r << endl;
					}			
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout << " M1=" << M1 << " M2=" << M2 << " forb=" << forb << " inc=" << inc << " phi0=" << phi0 << " r=" << r << endl;
				GWs.push_back(new GWBinary(Beta, Lambda, Psi, M1, M2, forb, inc, phi0, r));
				NbSrc++;
			}
			// ** Gravitational wave from a binary system compute at 1 or 2.5 PN
			if(scmp(Buf,"PostNewtonBinary")){ 
				Readable = true;
				//cout << "OG Binaire : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				int PNtype(1);
				double M1(1.0e5), M2(1.0e5), tcoal(600000.0), inc(M_PI/2.0), phase(0.0), dist(1.0e6);
				double omega0(0.), taud0(0.), gw(1.);
				do{
					ConfigFile >> Buf;
					if(scmp(Buf,"M1")){
						ConfigFile >> M1;
						//cout << " Masse 1 = " << M1 << endl;
					}
					if(scmp(Buf,"M2")){
						ConfigFile >> M2;
						//cout << " Masse 2 = " << M2 << endl;
					}
					if(scmp(Buf,"tcoal")){
						ConfigFile >> tcoal;
						//cout << " Frequence orbital = " << forb << endl;
					}
					if(scmp(Buf,"inc")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						inc = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(scmp(Buf,"phase")){
						ConfigFile >> phase;
						//cout << " Phase initial = " << phi0 << endl;
					}
					if(scmp(Buf,"r")){
						ConfigFile >> dist;
						//cout << " Distance = " << r << endl;
					}
					if(scmp(Buf,"type")){
						ConfigFile >> PNtype;
						//cout << " type = " << PNtype << endl;
						do{
							ConfigFile >> Buf;
							if(scmp(Buf,"omega0")){
								ConfigFile >> omega0;
							}
							if(scmp(Buf,"taud0")){
								ConfigFile >> taud0;
							}
							if(scmp(Buf,"gw")){
								ConfigFile >> gw;
							}
						}while((Buf != ";")&&(!ConfigFile.eof()));
					}
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWPostNewtonBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " inc=" << inc << " phase=" << phase << " dist=" << dist;
				cout << " type=" << PNtype << " omega0=" << omega0 << " taud0=" << taud0 << " gw=" << gw << endl;
				GWs.push_back(new GWNewton2(Beta, Lambda, Psi, PNtype, M1, M2, tcoal, inc, phase, dist, taud0, omega0, gw));
				NbSrc++;
			}
			// ** Gravitational wave from a binary system compute at 1 or 2.5 PN
			if(scmp(Buf,"FastSpinBBH")){ 
				Readable = true;
				//cout << "OG Binaire : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				double M1(1.0e6), M2(1.0e6), tcoal(600000.0), dist(1.0e6), phase(0.0);
				double S1(0.7), S2(0.3), polS1(1.7), polS2(2.0), azS1(3.2), azS2(2.5), polL(2.0), azL(4.7);  
				double AmplPNorder(0.0);
				double GWTimeOffset(-2000.0);
				do{
					ConfigFile >> Buf;
					if(scmp(Buf,"M1")){
						ConfigFile >> M1;
						//cout << " Masse 1 = " << M1 << endl;
					}
					if(scmp(Buf,"M2")){
						ConfigFile >> M2;
						//cout << " Masse 2 = " << M2 << endl;
					}
					if(scmp(Buf,"tcoal")){
						ConfigFile >> tcoal;
						//cout << " Frequence orbital = " << forb << endl;
					}
					if(scmp(Buf,"phase")){
						ConfigFile >> phase;
						//cout << " Phase initial = " << phi0 << endl;
					}
					if(scmp(Buf,"r")){
						ConfigFile >> dist;
						//cout << " Distance = " << r << endl;
					}
					if(scmp(Buf,"S1")){
						ConfigFile >> S1;
						//cout << " Spin1 S1 = " << S1 << endl;
					}
					if(scmp(Buf,"S2")){
						ConfigFile >> S2;
						//cout << " Spin1 S2 = " << S2 << endl;
					}
					if(scmp(Buf,"polS1")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						polS1 = MathUtils::deg2rad(tmpAng);
						//cout << " S1 polar angle = " << polS1 << " rd" << endl;
					}
					if(scmp(Buf,"polS2")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						polS2 = MathUtils::deg2rad(tmpAng);
						//cout << " S2 polar angle = " << polS2 << " rd" << endl;
					}
					if(scmp(Buf,"azS1")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						azS1 = MathUtils::deg2rad(tmpAng);
						//cout << " S1 azimuth = " << azS1 << " rd" << endl;
					}
					if(scmp(Buf,"azS2")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						azS2 = MathUtils::deg2rad(tmpAng);
						//cout << " S2 azimuth = " << azS2 << " rd" << endl;
					}
					if(scmp(Buf,"polL")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						polL = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(scmp(Buf,"azL")){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						azL = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(scmp(Buf,"PN")){
						ConfigFile >> AmplPNorder;
						//cout << " type = " << PNtype << endl;
					}
					
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout.precision(15);
				cout << "  + GWFastSpinBBH : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " phase=" << phase << " dist=" << dist << " AmpPN=" << AmplPNorder;
				cout << " L0=(theta=" << polL << ",phi=" << azL << ") S1=" << S1 << "(" << polS1 << "," << azS1 << ") S2=" << S2 << "(" << polS2 << "," << azS2 << ")" << endl;
				GWs.push_back(new GWFastSpinBBH(Beta, Lambda, M1, M2, tcoal, dist, S1, S2, polS1, polS2, azS1, azS2, phase, polL, azL, AmplPNorder, GWTimeOffset, tMax-GWTimeOffset, 7.0, 150.0, 6.0));
				NbSrc++;
			}
			// ** Source dans un fichier
			if(scmp(Buf,"File")){
				char GWFileName[256];
				int GWFileEncoding(0);
				double GWTimeOffset(-1.0), GWTimeStep(-1.0);
				int GWLength(100), GWRecords(2);
				char *ptr;
				
				//cout << "OG Fichier : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				ConfigFile >> GWFileName; // Name of file
				//cout << " Nom du fichier contenant l'onde = " << FileName << endl;
				
				ptr=GWFileName+strlen(GWFileName);
				if(strncmp(ptr-4,".xml",4)==0) {
					int typedata;
					// Binary file described in xml file
					ezxml_t tree, section;
					tree = ezxml_parse_file(GWFileName);
					cout << "  Read GW (SampledPlaneWave) in XML file " << GWFileName << " : " << endl;
					for (section = ezxml_child(tree, "XSIL"); section; section = section->next) {
						if(strcmp(ezxml_attr(section,"Type"),"SourceData")==0){
							ezxml_t source;
							for (source = ezxml_child(section, "XSIL"); source; source = source->next) {
								if(strcmp(ezxml_attr(source,"Type"),"SampledPlaneWave")==0){
									ezxml_t filedata;
									for (filedata = ezxml_child(source, "XSIL"); filedata; filedata = filedata->next) {
										ezxml_t param ;
										for(param = ezxml_child(filedata,"Param"); param; param = param->next){
											if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0)
												GWTimeOffset = gXMLTime(param);
											if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0)
												GWTimeStep = gXMLTime(param);
										}
										char * TmpFileName;
										TmpFileName = gXMLTimeSeries(filedata, typedata, GWFileEncoding, GWLength, GWRecords);
										strcpy(GWFileName,TmpFileName);
									}
								}else
									throw invalid_argument("ConfigSim::ReadASCIIFile : Doesn't find SampledPlaneWave in informations of source !");
							}
						}else
							throw invalid_argument("ConfigSim::ReadASCIIFile : Doesn't find SourceData in XML file !");
					}
					//cout << "   - TimeOffset   = " << GWTimeOffset  << " s" << endl;
					//cout << "   - Cadence      = " << GWTimeStep  << " s" << endl;
					//cout << "   - FileName     = " << GWFileName << endl;
					//cout << "   - TypeData     = " << typedata << " o " << endl;
					//cout << "   - Encoding     = " << GWFileEncoding << endl;
					//cout << "   - Length       = " << GWLength << endl;
					//cout << "   - Records      = " << GWRecords << endl;
					ConfigFile >> Buf;
				}else{
					ConfigFile >> Buf;
					if(scmp(Buf,"BINARY")){
						// Binary file described in configuration file by the following parameters 
						// (TimeOffset, Cadence, Length, Records[2 or 3])
						GWFileEncoding = 1 ;
						ConfigFile >> GWLength ;
						ConfigFile >> GWRecords ;
						ConfigFile >> GWTimeOffset;
						ConfigFile >> GWTimeStep;
						ConfigFile >> Buf;
					}else{
						if(scmp(Buf,"ASCII")){
							ConfigFile >> GWTimeOffset;
							ConfigFile >> GWTimeStep;
							ConfigFile >> Buf;
						}
						// ASCII file with 3 records included time
						GWFileEncoding = 0 ;
					}
				}
				
				if(Buf == ";"){
					Readable = true;
				}
				cout.precision(15);
				cout << "  + GWFile : Beta = " << Beta << " , Lambda = " << Lambda << " , Psi = " << Psi << " : " << GWFileName;
				if(GWFileEncoding){
					cout << ", BINARY , Offset = " << GWTimeOffset << " , TimeStep = " << GWTimeStep << " , GWLength = " << GWLength << " , GWRecords = " << GWRecords;
				}else{
					cout << ", ASCII,  Offset = " << GWTimeOffset << " , TimeStep = " << GWTimeStep;
				}
				cout << " : Load file ..."; fflush(stdout);
				GWs.push_back(new GWFile2(Beta, Lambda, Psi, GWFileName, GWFileEncoding, GWTimeOffset, GWTimeStep, GWLength, GWRecords));
				cout << " --> OK" << endl;
				NbSrc++;				
			}
		}
		
		
		
		/* Informations sur les bruits 
		 ------------------------------*/
		if(scmp(Buf,"Noise")){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			int Index(0);
			NoiseSpec tmp_noise;
			// Lecture de l'element bruite
			if(scmp(Buf,"Laser")){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir;
			}
			if(scmp(Buf,"Shot")){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+6;
			}
			if(scmp(Buf,"OOPN")){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+6;
			}
			if(scmp(Buf,"Mass")){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+12;
			}
			if(scmp(Buf,"Bench")){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+18;
			}
			ConfigFile >> Buf;
			
			if(Buf != ":"){
				throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the noise !");
			}
			ConfigFile >> Buf;
			if(scmp(Buf,"White")){
				double SqPSD(0.0);
				//cout << "Bruit blanc ";
				ConfigFile >> SqPSD;
				//cout << " de SqPSD = " << SqPSD << endl;
				//tmp_noise = new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, SqPSD);
				tmp_noise.NType = 1;
				tmp_noise.NVal0=SqPSD;
				ConfigFile >> Buf;
			}
			if(scmp(Buf,"File")){
				tmp_noise.NType = 2;
				ConfigFile >> tmp_noise.NStr;  // File name
				ConfigFile >> tmp_noise.NVal0; // Multiplication factor
				//cout << "NoiseInFile File Name = " << FileNoiseName << endl;
				//tmp_noise = new NoiseFile(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, FileNoiseName);
				ConfigFile >> Buf;
			}
			if(scmp(Buf,"FilterCoef")){
				tmp_noise.NType = 3;
				//cout << "Bruit filtrer ";
				ConfigFile >> Buf; // Lecture de :
				ConfigFile >> Buf; 
				do{
					if(scmp(Buf,"alpha")){
						do{
							double tmp_coef(0.0);
							ConfigFile >> tmp_coef;
							(tmp_noise.NVal1).push_back(tmp_coef);
							ConfigFile >> Buf;
						}while(Buf == ",");
					}
					if(scmp(Buf,"beta")){
						do{
							double tmp_coef(0.0);
							ConfigFile >> tmp_coef;
							(tmp_noise.NVal2).push_back(tmp_coef);
							//(beta).push_back(tmp_coef*tStepPhy);
							ConfigFile >> Buf;
						}while(Buf == ",");
					}
					if(scmp(Buf,"stablization")){
						ConfigFile >> tmp_noise.NVal0;
						ConfigFile >> Buf;
					}
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}
			
			
			if(scmp(Buf,"Filter_1of")){
				tmp_noise.NType = 4;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(scmp(Buf,"Filter_f")){
				tmp_noise.NType = 5;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(scmp(Buf,"Filter_fLosP")){
				tmp_noise.NType = 6;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(scmp(Buf,"Filter_MLDC_IM")){
				tmp_noise.NType = 7;  // new number
				ConfigFile >> tmp_noise.NVal0; // Level of 1/f
				ConfigFile >> tmp_noise.NVal01; // Frequency at which 1/f becomes 1/f^2
				ConfigFile >> Buf;
			}
			/*
			 if(scmp(Buf,"Filter_Sto")){
			 tmp_noise.NType = 8;  // new number
			 ConfigFile >> tmp_noise.NVal0; // parametre a definir
			 ConfigFile >> Slope_ep; // parametre a definir
			 ConfigFile >> Tsample_ep; // parametre a definir
			 ConfigFile >> Fknee_ep; // parametre a definir
			 ConfigFile >> Fmin_ep; // parametre a definir
			 ConfigFile >> Buf;
			 }*/
			
			if(Buf == ";"){
				//cout << "Add noise in " << Index << endl;
				Readable = true;
				if(Index > NoisesData.size()-1)
					NoisesData.resize(Index+1);
				NoisesData[Index].push_back(tmp_noise);
			}
		}
		
		
		/* Informations sur les USO Clocks
		 ------------------------------*/
		if(scmp(Buf,"USO")){
			int iSC(0);
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(scmp(Buf,"SC"))
				ConfigFile >> iSC;
			cout << "  x Make USO " << iSC << " :" << endl;
			if((iSC<1)||(iSC>3))
				throw invalid_argument("ConfigSim::ReadFile : The spacecraft number must be 1, 2 or 3 !");
			Readable = true;
			ConfigFile >> Buf;
			double USOOffset(0.0), USODerivs(0.0), USONoise(0.0) ;
			do{
				if(scmp(Buf,"offset")){
					ConfigFile >> USOOffset;
					//cout << " offset = " << USOOffset << endl;
				}
				if(scmp(Buf,"derivs")){
					ConfigFile >> USODerivs;
					//cout << " derivs = " << USODerivs << endl;
				}
				if(scmp(Buf,"noise")){
					ConfigFile >> USONoise;
					//cout << " USONoise = " << USONoise << endl;
				}
				ConfigFile >> Buf;
			}while((Buf != ";")&&(!ConfigFile.eof()));
			USOs[iSC-1].init(USOOffset, USODerivs, USONoise);
		}
		
		
		/* Informations about TDI generator
		 ---------------------------------------*/
		if(scmp(Buf,"TDI")){
			char generatorname[256];
			vector<int> tmp_TDIPacks;
			vector<double> tmp_TDIPacksFact;
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> generatorname;
			bool UnKnowTDI(true);
			
			// *** Search if TDI generator is predifined
			UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks, tmp_TDIPacksFact);
			
			// *** Read unknow TDI generator
			if(UnKnowTDI){
				cout << "  o TDI " << generatorname << " : Read package ... " ;
				// ** Read all packs
				do{
					int tmp_pack;
					double tmp_Fact(1.0);
					// * Read one packs
					ConfigFile >> tmp_pack;
					tmp_TDIPacks.push_back(tmp_pack);
					ConfigFile >> Buf;
					// * Count the number of delays and store as NbMaxDelays if the number of delays higher than the previous NbMaxDelays
					int NbDelays(0);
					while(!((double)(tmp_pack/pow(10.0,NbDelays))<1.0)){
						NbDelays++;
					}
					if(NbDelays-1>NbMaxDelays)
						NbMaxDelays = NbDelays-1;
					// * Read factor if it's exist
					if(scmp(Buf,"x")){
						ConfigFile >> tmp_Fact;
						ConfigFile >> Buf;
					}
					tmp_TDIPacksFact.push_back(tmp_Fact);
				}while(Buf == ",");
				cout << " --> OK !" <<  endl;
			}else{
				cout << "  o TDI " << generatorname << " is predefined generator : Packs isn't readed." << endl;
				do{
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}
			if(Buf == ";"){
				Readable = true;
			}
			
			TDIsName.push_back(generatorname);
			TDIsPacks.push_back(tmp_TDIPacks);
			TDIsPacksFact.push_back(tmp_TDIPacksFact);
		}
		
		
		
		/* Read end of configuration's file
		 ---------------------------------------*/
		if(scmp(Buf,"END")){
			ContinueRead = false;
			Readable = true;
		}
		
		
		// Control if there is a good reading
		if(!Readable){
			cerr << "Error in file " <<  ConfigFileName << " , command " << nReadCmd << " : " << Buf << endl;
			throw invalid_argument("ConfigSim::ReadFile : No readable string !");
			exit(1);
		}
	};
	
	
	// ------ Fin de la lecture du fichier ------
	
	// Creation of tMemNoiseFirst and tMemNoiseLast
	double OrderLagrangeNoise(7);
	tMemNoiseFirst = tStepPhy*ceil(2+OrderLagrangeNoise/2.0);
	tMemNoiseLast = -1.0*(tMaxDelay()+tStepPhy*ceil(2+OrderLagrangeNoise/2.0));
	
	// Noises creation
	NoisesCreation();
	
	// Display of TDI generators
	cout << "  ----------" << endl;
	for(int i=0; i<TDIsName.size(); i++){
		cout << "  " << TDIsName[i] << " :";
		for(int j=0; j<TDIsPacks[i].size(); j++){
			cout << " " << (TDIsPacks[i])[j];
			if(fabs((TDIsPacksFact[i])[j]-1.0) > PRECISION)
				cout << "*" << (TDIsPacksFact[i])[j];
		}
		cout << endl;
	}
	
	// ** Default value for display time step
	if(tDisplay<0.0)
		tDisplay = tMax/1000.0;
	
	cout << "  ----------" << endl;
	cout << "  Sources number = " << NbSrc << endl;
	cout << "  tStepPhy       = " << tStepPhy << " s" << endl;
	cout << "  tStepMes       = " << tStepMes << " s" << endl;
	cout << "  tDeltaTDIDelay = " << tDeltaTDIDelay << " s" << endl;
	cout << "  tStepDisplay   = " << tDisplay << " s" << endl;
	cout << "  NbMaxDelays    = " << NbMaxDelays << endl;
	cout << "  InterpUtilVal  = " << TDIInterpVal << endl;
	cout << "  Orbits type    = " << OrbType << endl;
	cout << "  Orbits approx  = " << OrbApprox << endl;
	cout << "  Delay order    = " << OrbOrder << endl; 
	
	/*	for(int i=0; i<3; i++){
	 cout << " - USO " << i+1 << " : offset = " <<  USOs[i].getOffset();
	 cout << " , derivs = " << USOs[i].getDeriv() << " , noise = " << USOs[i].getNoise() << endl;	
	 }
	 */
	
	ConfigFile.close();
}

/*************** XML Configuration File */
/*!\brief
 * Reads XML configuration file
 *
 * Data are read :
 * \arg Time Data
 * \arg Interpolation Data
 * \arg Precision TDI Data
 * \arg Orbits Data
 * \arg Detector Data
 * \arg USO Data
 * \arg Record Data
 */
void ConfigSim::ReadXMLFile()
{
	try{
		int NbSrc(0);
		cout << ">>> Read XML configuration file : " << ConfigFileName << endl;
		
		// Test if  file exist
		ifstream ConfigFile;
		ConfigFile.open(ConfigFileName);
		if (!ConfigFile){
			cerr << "ConfigSim::ReadFile : Error to open the file  " << ConfigFileName << endl;
			exit(1);
		}
		ConfigFile.close();
		ConfigFile.clear();
		
		//modif E.P.
		ezxml_t tree, section,head;
		const char *type;
		tree = ezxml_parse_file(ConfigFileName);
		
		for (head = ezxml_child(tree, "Param") ; head ; head=head->next){
			
			if(strcmp(ezxml_attr(head, "Name"),"Author")==0){
				Author = gXMLstring(head);
			}
			if(strcmp(ezxml_attr(head,"Name"),"Simulator")==0){
				Simulator = gXMLstring(head);
			}
			if(strcmp(ezxml_attr(head,"Name"),"GenerationDate")==0){
				GenerationDate = gXMLstring(head);
				GenerationType = ezxml_attr(head,"Type");
				cout << "   x Type = "<< GenerationType << endl ;
			}
		}
		//modif E.P.
		for (section = ezxml_child(tree, "XSIL"); section; section = section->next) {
			type = ezxml_attr(section, "Type");
			
			// modif E.P.
			/******************/
			/* Read General Data */
			/******************/
			/*if(strcmp(type,"General")==0){
			 ezxml_t param;
			 string str_xml;
			 //string Author,GenerationDate,GenerationType; 
			 double val ;
			 cout << "Section : "<< type  << endl ;
			 for(param = ezxml_child(section,"Param"); param; param = param->next){
			 if(strcmp(ezxml_attr(param,"Name"),"Author")==0){
			 Author = gXMLstring(param);
			 }
			 if(strcmp(ezxml_attr(param,"Name"),"Simulator")==0){
			 Simulator = gXMLstring(param);
			 }
			 if(strcmp(ezxml_attr(param,"Name"),"GenerationDate")==0){
			 GenerationDate = gXMLstring(param);
			 GenerationType = ezxml_attr(param,"Type");
			 cout << " - Type = "<< GenerationType << endl ;
			 }
			 }
			 //throw invalid_argument("Section : General : Stop") ;
			 }*/
			// fin modif E.P.
			/**********************/
			/* Read Simulate Data */
			/**********************/
			if(strcmp(type,"Simulate")==0){
				ezxml_t param;
				//vector<int> tmp_TDIPacks;
				string str_xml;
				int loc_tdi_start, loc_tdi_end;
				bool UnKnowTDI(true);
				cout << "Section : "<< type  << endl ;
				for(param = ezxml_child(section,"Param"); param; param = param->next){
					// Set time offset
					if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0){
						tOffset = gXMLTime(param);
					}
					// Set time step of measurement
					if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0){
						tStepMes = gXMLTime(param);
					}
					// Set duration
					if(strcmp(ezxml_attr(param,"Name"),"Duration")==0){
						settDur(gXMLTime(param));
					}
					// **** Set used TDI generators ****
					if(strcmp(ezxml_attr(param,"Name"),"Observables")==0){					
						// ** Extract string which contains TDI generator list (and others outputs as phasemeters)
						TDIParamName = gXMLstring(param) ; // Type string
						loc_tdi_start = 0;
						do{
							char generatorname[64];
							vector<int> tmp_TDIPacks;
							vector<double> tmp_TDIPacksFact;
							loc_tdi_start = TDIParamName.find( ",", loc_tdi_start+1 );
							if(loc_tdi_start > 0){
								loc_tdi_end = TDIParamName.find( ",", loc_tdi_start+1 );
								if(loc_tdi_end<0) // Last generator
									loc_tdi_end = TDIParamName.length();
								TDIParamName.copy(generatorname,loc_tdi_end-loc_tdi_start-1,loc_tdi_start+1);
								generatorname[loc_tdi_end-loc_tdi_start-1] = '\0';
								//cout << loc_tdi_start << " -> " << loc_tdi_end << " : " << loc_tdi_end-loc_tdi_start-1 << " : " << generatorname << endl; 
								UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks, tmp_TDIPacksFact);
								cout << "   o TDI " << generatorname; 
								if(UnKnowTDI){
									if((strcmp(generatorname,"y231")==0)||(strcmp(generatorname,"y321")==0)||
									   (strcmp(generatorname,"z231")==0)||(strcmp(generatorname,"z321")==0)||
									   (strcmp(generatorname,"y312")==0)||(strcmp(generatorname,"y132")==0)||
									   (strcmp(generatorname,"z312")==0)||(strcmp(generatorname,"z132")==0)||
									   (strcmp(generatorname,"y123")==0)||(strcmp(generatorname,"y213")==0)||
									   (strcmp(generatorname,"z123")==0)||(strcmp(generatorname,"z213")==0))
										cout << " --> Phasemeter data are not manage here." << endl;
									else
										cout << " --> Sorry, it is not known !" << endl;
								}else{
									TDIsName.push_back(generatorname);
									TDIsPacks.push_back(tmp_TDIPacks);
									TDIsPacksFact.push_back(tmp_TDIPacksFact);
									cout << " --> OK " << endl;
								}
							} 
						}while(loc_tdi_start > 0);  
					}
					
					if(strcmp(ezxml_attr(param,"Name"),"OutputFile")==0){
						strcpy(XmlOutputFile,gXMLWord(param)) ;
					}
				}
				//throw invalid_argument("Section : Simulate : Stop") ;
			}
			
			// **********************
			// * Read LISACode Data *
			// **********************
			if(strcmp(type,"LISACode")==0){
				ezxml_t param;
				ezxml_t lisacode0;
				const char * lisacode0type;
				cout << "Section : LISACode " << endl ;
				for(param = ezxml_child(section,"Param"); param; param = param->next){
					if(strcmp(ezxml_attr(param,"Name"),"GlobalRandomSeed")==0){
						if(isdigit((gXMLstring(param))[0]))
							GlobalRandomSeed = gXMLint(param);
					}
					if(strcmp(ezxml_attr(param,"Name"),"StepPhysic")==0)
						tStepPhy = gXMLTime(param);
					if(strcmp(ezxml_attr(param,"Name"),"StepMeasure")==0)
						tStepMes = gXMLTime(param);
					if(strcmp(ezxml_attr(param,"Name"),"Max")==0)
						tMax = gXMLTime(param);
					if(strcmp(ezxml_attr(param,"Name"),"DeltaTDIDelay")==0)
						tDeltaTDIDelay = gXMLTime(param);
					if(strcmp(ezxml_attr(param,"Name"),"StepDisplay")==0)
						tDisplay = gXMLTime(param);
					
				}  // all parameters are read
				//throw invalid_argument("Section : LISACode : Stop") ;
				
				for (lisacode0 = ezxml_child(section, "XSIL"); lisacode0; lisacode0 = lisacode0->next) {
					lisacode0type = ezxml_attr(lisacode0,"Type");
					cout << "  Subsection : " << lisacode0type << endl;
					
					
					// **** Read OutputData **** //
					if((strcmp(lisacode0type,"OutputData")==0)||(strcmp(lisacode0type,"OutputFiles")==0)){
						for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SignalSC1")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNameSigSC1,gXMLWord(param));
									cout << "   x filename = " << FileNameSigSC1 ;
									FileEncodingSC1 = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingSC1 = 1 ;
									}
									cout << "   --> Encoding = " << FileEncodingSC1  << endl;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"SignalSC2")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNameSigSC2,gXMLWord(param));
									cout << "   x filename = " << FileNameSigSC2 ;
									FileEncodingSC2 = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingSC2 = 1;
									}
									cout << "   --> Encoding = " << FileEncodingSC2  << endl;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"SignalSC3")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNameSigSC3,gXMLWord(param));
									cout << "   x filename = " << FileNameSigSC3 ;
									FileEncodingSC3 = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingSC3 = 1 ;
									}
									cout << "   --> Encoding = " << FileEncodingSC3  << endl;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"TDI")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNameTDI,gXMLWord(param));
									cout << "   x filename = " << FileNameTDI ;
									FileEncodingTDI = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingTDI = 1 ;
									}
									cout << "   --> Encoding = " <<  FileEncodingTDI << endl;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"Delay")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNameDelays,gXMLWord(param));
									cout << "   x filename = " << FileNameDelays ;
									FileEncodingDelays = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingDelays = 1 ;
									}
									cout << "   --> Encoding = " << FileEncodingDelays << endl;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"Position")==0){
								if(strncmp(gXMLWord(param),"None",4) != 0){
									strcpy(FileNamePositions,gXMLWord(param));
									cout << "   x filename = " << FileNamePositions ;
									FileEncodingPos = 0;
									if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
										FileEncodingPos = 1 ;
									}
									cout << "   --> Encoding = " << FileEncodingPos  << endl;
								}
							}
						}
					}
					
					
					// **** Read Detector Data **** //
					if(strcmp(ezxml_attr(lisacode0,"Type"),"Detector")==0){
						for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"LaserPower")==0){
								LaserPower = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"Watt")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The lasers' power unit isn't known (only Watt) !");
								cout << "   x LaserPower = " << LaserPower << " W" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PhaMetFilter")==0){
								if(gXMLstring(param) == "On"){
									PhaMetFilterON = true;
								}else{ 
									if(gXMLstring(param) == "Off"){
										PhaMetFilterON = false;
									}else{
										throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the PhaMetFilterON  specification (ON or OFF) !");
									}
								}
								cout << "   x Phasemeter filter = " << PhaMetFilterON << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFAttenuation")==0){
								PhaMetFilterParam[0] = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The attenuation unit isn't known (only dB) !");
								cout << "   x PMFilter : Attenuation = " << PhaMetFilterParam[0] << " dB" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFOscillation")==0){
								PhaMetFilterParam[1] = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The oscillation unit isn't known (only dB) !");
								cout << "   x PMFilter : Oscillation = " << PhaMetFilterParam[1] << " dB" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFLowFreqFact")==0){
								PhaMetFilterParam[2] = atof(ezxml_txt(param));
								cout << "   x PMFilter : Low Frequency factor of sample frequency = " << PhaMetFilterParam[2] << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFHighFreqFact")==0){
								PhaMetFilterParam[3] = atof(ezxml_txt(param));
								cout << "   x PMFilter : High Frequency factor of sample frequency = " << PhaMetFilterParam[3] << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"InternalPhasemeters")==0){	
								if(gXMLstring(param)=="Always"){
									InternalPhasemeters = true;
								}else{ 
									if(gXMLstring(param)=="IfNoises"){
										InternalPhasemeters = false;
									}else{
										throw invalid_argument("ConfigSim::ReadXMLFile : Bad syntax for the internal phasemeter : 'Always' or 'IfNoises' !");
									}
								}
								cout << "   x Phasemeter filter = " << InternalPhasemeters << endl;
							}							
						}
					}
					// ***** Read USO Data *****
					if(strcmp(ezxml_attr(lisacode0,"Type"),"USO")==0){
						ezxml_t param;
						int iSC;
						double USOOffset(0.0), USODerivs(0.0), USONoise(0.0) ;
						for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SCIndex")==0){
								iSC = atoi(ezxml_txt(param));
								if((iSC<1)||(iSC>3))
									throw invalid_argument("ConfigSim::ReadXMLFile : The index of spacecraft can only be 1, 2 or 3 !");
								cout << "   x USO SC Index = " << iSC << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"Offset")==0)
								USOOffset = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"Derivs")==0)
								USODerivs = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"Noise")==0){
								double Fact(1.0);
								char * UnitType;
								UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
								USONoise = atof(ezxml_txt(param))*Fact;
								cout << "   x Noise = " <<USONoise << endl;
							}
						}
						if((iSC<1)||(iSC>3))
							throw invalid_argument("ConfigSim::ReadXMLFile : Bad USO's spacecraft number (only 1, 2 or 3) !");
						USOs[iSC-1].init(USOOffset, USODerivs, USONoise);
					}
					
					// **** Read Precision TDI Data ****
					if(strcmp(ezxml_attr(lisacode0,"Type"),"PrecisionTDI")==0){
						ezxml_t param;
						for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"InterpType")==0){
								if(gXMLstring(param) == "LAG")
									TDIInterp = LAG;
								cout << "   x InterpType = " << TDIInterp << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"InterpValue")==0){
								TDIInterpVal = atoi(ezxml_txt(param));
								cout << "   x InterpValue = " << TDIInterpVal << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"DelayApprox")==0){
								if(gXMLstring(param) == "On"){
									TDIDelayApprox = true;
								}else{ 
									if(gXMLstring(param) == "Off"){
										TDIDelayApprox = false;
									}else{
										throw invalid_argument("ConfigSim::ReadFile : Bad syntax for approximation of TDI Delay (ON or OFF) !");
									}
								}
								cout << "   x Approximation of TDI Delay = " << TDIDelayApprox << endl;
							}
						}
					}
					// ***** Read specific TDI generator ***** //
					if(strcmp(ezxml_attr(lisacode0,"Type"),"TDISpecificGenerator")==0){
						const char * generatorname;
						vector<int> tmp_TDIPacks;
						vector<double> tmp_TDIPacksFact;
						ezxml_t param;
						
						// ** Check if the generator don't already exist in LISABase
						bool UnKnowTDI(true);
						for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"GeneratorName")==0){
								generatorname = ezxml_txt(param);
								cout << "   x Name = " << generatorname << endl;
							}
						}
						UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks, tmp_TDIPacksFact);
						
						// Read unknow TDI generator
						if(UnKnowTDI){
							cout << "     x Pack = ";
							for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Pack")==0){
									int tmp_pack;
									int tmp_fact;
									int NbDelays(0);
									tmp_pack = atoi(ezxml_txt(param));
									tmp_fact = atoi(ezxml_attr(param,"Unit"));
									cout << " " << tmp_pack ;
									tmp_TDIPacks.push_back(tmp_pack);
									tmp_TDIPacksFact.push_back(tmp_fact);
									while(!((double)(tmp_pack/pow(10.0,NbDelays))<1.0)){
										NbDelays++;
									}
								}
							}
							cout << endl;
						}else{
							cout << "TDI generator " << generatorname << " is already defined therefore packs are read." << endl; 
						}
						
						TDIsName.push_back(generatorname);
						TDIsPacks.push_back(tmp_TDIPacks);
						TDIsPacksFact.push_back(tmp_TDIPacksFact);
					}
				}
			}
			
			// ******************
			// * Read LISA Data *
			// ******************
			
			if(strcmp(type,"LISAData")==0){
				ezxml_t lisadata;
				const char * lisadatatype;
				cout << "Section : LISAData " << endl ;
				for (lisadata = ezxml_child(section, "XSIL"); lisadata; lisadata = lisadata->next) {
					lisadatatype = ezxml_attr(lisadata,"Type");
					cout << "  Subsection : " << lisadatatype << endl;
					
					// ***********  Configure orbits *********** //
					
					if((strcmp(ezxml_attr(lisadata,"Type"),"LISACode_Orbits")==0)||(strcmp(ezxml_attr(lisadata,"Type"),"PseudoLISA")==0)||(strcmp(ezxml_attr(lisadata,"Type"),"ESA_Orbits")==0)){
						ezxml_t param;
						if(strcmp(ezxml_attr(lisadata,"Type"),"LISACode_Orbits")==0)
							OrbType = 0;
						if(strcmp(ezxml_attr(lisadata,"Type"),"PseudoLISA")==0)
							OrbType = 1;
						if(strcmp(ezxml_attr(lisadata,"Type"),"ESA_Orbits")==0)
							OrbType = 2;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"InitialRotation")==0)
								//cout << " MLDC PeusoLisa Orbits" << endl ;
								OrbInitRot = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialPosition")==0){
								double tmpInitPos(0.0);
								tmpInitPos = gXMLAngle(param);
								OrbStartTime = tmpInitPos*Yr_SI/(2.0*M_PI);
							}
							if(strcmp(ezxml_attr(param,"Name"),"Armlength")==0){
								if(strcmp(ezxml_attr(param,"Unit"),"Meter")==0){
									Armlength = atof(ezxml_txt(param));
								}else{
									double tmpArmTime(0.0);
									tmpArmTime = gXMLTime(param);
									Armlength = tmpArmTime*c_SI;
								}
								cout << "   x Armlength = " << Armlength << " m" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"OrderDelay")==0){
								OrbOrder = atoi(ezxml_txt(param));
								cout << "   x Order = " << OrbOrder << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"OrbitApproximation")==0){
								string tmpStr(gXMLstring(param));
								if(tmpStr == "Static")
									OrbApprox = 0;
								if(tmpStr == "Rigid")
									OrbApprox = 1;
								if(tmpStr == "Eccentric"){
									OrbApprox = 2;
									OrbOrder = -2;
								}
							}
						}
					}
				}
			}
			// * End of read LISA Data * //
			
			
			
			// ********************
			// * Read Source Data *
			// ********************
			if(strcmp(type,"SourceData")==0){
				ezxml_t source;
				const char *name;
				double Beta, Lambda, Psi;
				cout << "Section : SourceData" << endl ;
				for (source = ezxml_child(section, "XSIL"); source; source = source->next) {
					name = ezxml_attr(source,"Name");
					cout << "  Source name = " << name << endl;
					
					ezxml_t param;
					for(param = ezxml_child(source,"Param"); param; param = param->next){
						if(strcmp(ezxml_attr(param,"Name"),"EclipticLatitude")==0)
							Beta = gXMLAngle(param);
						if(strcmp(ezxml_attr(param,"Name"),"EclipticLongitude")==0)
							Lambda = gXMLAngle(param);
						if(strcmp(ezxml_attr(param,"Name"),"Polarization")==0)
							Psi = gXMLAngle(param);
					}
					cout << "   Type = " << ezxml_attr(source,"Type")<< endl ;
					
					
					// ***************  Read PlaneWave Source *************** //

					if(strcmp(ezxml_attr(source,"Type"),"PlaneWave")==0){
						ezxml_t param;
						string SourceType ;
						SourceType = "unknown" ;
						// **** Read only source type
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SourceType")==0){
								SourceType = gXMLstring(param);
							}
						}
						
						// *** Read Parameters for Monochromatic
						if(SourceType == "Monochromatic"){
							double f(0.0), hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Frequency")==0)
									f = gXMLFrequency(param);
								if(strcmp(ezxml_attr(param,"Name"),"AmplitudePlus")==0){
									double Fact(1.0);
									char * UnitType;
									UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
									hp = atof(ezxml_txt(param))*Fact;
									cout << "   x hp = " << hp << endl;
								}
								if(strcmp(ezxml_attr(param,"Name"),"AmplitudeCross")==0){
									double Fact(1.0);
									char * UnitType;
									UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
									hc = atof(ezxml_txt(param))*Fact;
									cout << "   x hc = " << hc << endl;
								}
								if(strcmp(ezxml_attr(param,"Name"),"InitialPhasePlus")==0)
									Phi0hp = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"InitialPhaseCross")==0)
									Phi0hc = gXMLAngle(param);
							}
							cout.precision(15);
							cout << "  + GWMono   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout.precision(15);
							cout << " f=" << f << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
							GWs.push_back(new GWMono(Beta, Lambda, Psi, f, hp, hc, Phi0hp, Phi0hc));
							NbSrc++;
						}
						
						// *** Read Parameters for Binary
						if(SourceType == "GalacticBinary"){
							double forb, inc, phi0;
							double M1, M2, r;
							double Amp, freq;
							bool SrcMLDC(false);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Inclination")==0)
									inc = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"InitialPhase")==0)
									phi0 = gXMLAngle(param);
								// * LISACode
								if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
									M1 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
									M2 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
									r = gXMLAstroDistance(param);
								if(strcmp(ezxml_attr(param,"Name"),"OrbitalFrequency")==0)
									forb = gXMLFrequency(param);
								// * MLDC
								if(strcmp(ezxml_attr(param,"Name"),"Amplitude")==0){
									Amp = atof(ezxml_txt(param)) * atof(ezxml_attr(param,"Unit"));
									SrcMLDC = true;
								}
								if(strcmp(ezxml_attr(param,"Name"),"Frequency")==0){
									freq = gXMLFrequency(param);
									SrcMLDC = true;
								}
								
							}
							cout.precision(15);
							if(SrcMLDC){
								// * MLDC
								double ci(cos(inc));
								cout << "  + GWMono : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
								cout << " Amplitude=" << Amp << " freq=" << freq << " inc=" << inc << " phi0=" << phi0 << endl;
								GWs.push_back(new GWMono(Beta, Lambda, Psi, freq, Amp*(1.0+ci*ci), 2*Amp*ci, phi0+3.0*M_PI/2.0, phi0));
							}else{
								// * LISACode
								cout << "  + GWBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
								cout << " M1=" << M1 << " M2=" << M2 << " forb=" << forb << " inc=" << inc << " phi0=" << phi0 << " r=" << r << endl;
								GWs.push_back(new GWBinary(Beta, Lambda, Psi, M1, M2, forb, inc, phi0, r));
							}
							NbSrc++;
						}
						
						
						// *** Read Parameters for PostNewtonBinary
						if(SourceType == "BlackHoleBinary"){
							int PNtype(2);
							double M1, M2, tcoal, inc, dist; // Commun
							double omega0(0.0), taud0(0.0), gw(1.0); // LISACode
							double phase(0.0), phase0(0.0); // LISACode or MLDC 
							double tTrunc(0.0), taper(7.0); // Ready to use ... when taper is included in waveform
							bool SrcMLDC(false);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
									M1 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
									M2 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"CoalescenceTime")==0)
									tcoal = gXMLTime(param);
								if(strcmp(ezxml_attr(param,"Name"),"Inclination")==0)
									inc = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
									dist = gXMLAstroDistance(param);
								if(strcmp(ezxml_attr(param,"Name"),"TruncationTime")==0)
									tTrunc = gXMLTime(param);
								if(strcmp(ezxml_attr(param,"Name"),"TaperApplied")==0)
									taper = atof(ezxml_txt(param));
								// * Specific for LISACode
								if(strcmp(ezxml_attr(param,"Name"),"CoalescencePhase")==0)
									phase = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"Type")==0)
									PNtype = atoi(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"Omega0")==0)
									omega0 = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"Taud0")==0)
									taud0 = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"gw")==0)
									gw = atof(ezxml_txt(param));
								// * Specific for MLDC
								if(strcmp(ezxml_attr(param,"Name"),"InitialAngularOrbitalPhase")==0){
									phase0 = gXMLAngle(param); // If it's possible it's better to use the phase
									SrcMLDC = true;
								}
							}
							cout.precision(15);
							cout << "  + GWPostNewtonBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " inc=" << inc << " phase=" << phase << " dist=" << dist;
							cout << " type=" << PNtype << " omega0=" << omega0 << " taud0=" << taud0 << " gw=" << gw << endl;
							GWs.push_back(new GWNewton2(Beta, Lambda, Psi, PNtype, M1, M2, tcoal, inc, phase, dist, taud0, omega0, gw));
							NbSrc++;
						}
						
						
						// *** Read Fast spin Black Hole binary
						if(SourceType == "FastSpinBlackHoleBinary"){
							double M1(1.0e6), M2(1.0e6), tcoal(600000.0), dist(1.0e6), phase(0.0);
							double S1(0.7), S2(0.3), polS1(1.7), polS2(2.0), azS1(3.2), azS2(2.5), polL(2.0), azL(4.7);  
							double AmplPNorder(0.0), TaperApplied(7.0);
							double GWTimeOffset(-2000.0);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
									M1 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
									M2 = gXMLAstroMass(param);
								if(strcmp(ezxml_attr(param,"Name"),"CoalescenceTime")==0)
									tcoal = gXMLTime(param);
								if(strcmp(ezxml_attr(param,"Name"),"PhaseAtCoalescence")==0)
									phase = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
									dist = gXMLAstroDistance(param);
								if(strcmp(ezxml_attr(param,"Name"),"PolarAngleOfSpin1")==0)
									polS1 = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"PolarAngleOfSpin2")==0)
									polS2 = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"AzimuthalAngleOfSpin1")==0)
									azS1 = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"AzimuthalAngleOfSpin2")==0)
									azS2 = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"InitialPolarAngleL")==0)
									polL = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"InitialAzimuthalAngleL")==0)
									azL = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"Spin1")==0)
									S1 = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"Spin2")==0)
									S2 = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"AmplPNorder")==0)
									AmplPNorder = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"TaperApplied")==0)
									TaperApplied = atof(ezxml_txt(param));
							}
							cout << "  + GWFastSpinBBH : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " phase=" << phase << " dist=" << dist << " AmpPN=" << AmplPNorder;
							cout << " L0=(theta=" << polL << ",phi=" << azL << ") S1=" << S1 << "(" << polS1 << "," << azS1 << ") S2=" << S2 << "(" << polS2 << "," << azS2 << ")" << endl;
							GWs.push_back(new GWFastSpinBBH(Beta, Lambda, M1, M2, tcoal, dist, S1, S2, polS1, polS2, azS1, azS2, phase, polL, azL, AmplPNorder, GWTimeOffset, tMax-GWTimeOffset, TaperApplied, 150.0, 6.0));
							NbSrc++;
						}
						
						// *** Read Parameters for Stochastic GW background
						if(SourceType == "Stochastic"){
							double val ;
							double hp_sto(0.0), hc_sto(0.0), Slope_sto(0.0), Fmin_sto(0.0), Fknee_sto(0.0);
							double tDurAdd(0.0), Tsample_sto(0.0);
							//double tFirst(40.0),  tLast(-100.0),  tder(-1000.0);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"PowerSpectralDensity")==0){
									//hp_sto = gXMLdouble(param)*1e-23/0.0333 ;
									hp_sto = gXMLdouble(param);
									hp_sto=pow(hp_sto,0.5);
									hc_sto = hp_sto ;
									//hc_sto = 0. ;
								}
								if(strcmp(ezxml_attr(param,"Name"),"SpectralSlope")==0){
									Slope_sto = gXMLdouble(param) ;
								}
								if(strcmp(ezxml_attr(param,"Name"),"Flow")==0){
									Fmin_sto = gXMLdouble(param) ;
								}
								if(strcmp(ezxml_attr(param,"Name"),"Fknee")==0){
									Fknee_sto = gXMLdouble(param) ;
								}
								if(strcmp(ezxml_attr(param,"Name"),"PseudoRandomseed")==0){
									val = gXMLdouble(param) ;
								}
								if(strcmp(ezxml_attr(param,"Name"),"InterpolationOrder")==0){
									val = gXMLdouble(param) ;
								}
							}
							// All parameters are read
							tDurAdd = tStepPhy ; 
							Tsample_sto = tStepPhy ;
							Beta = M_PI/2.-Beta ;  //  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! a verifier modif_eric !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
							//cout << "creating stochastic" <<endl ;
							cout << "  + GW Stochastic : " << NbSrc << " : Beta = " << Beta << " ,  Lambda = " << Lambda << " , dt = " << tStepPhy << " , hp = " << hp_sto<< " , hc = "<< hc_sto << " , Slope = " << Slope_sto << " , Fknee = " << Fknee_sto << " , Fmin = " << Fmin_sto << endl ;
							//GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));
							GWs.push_back(new GWSto(Beta,Lambda,tStepMes,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));						
							//GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,tDurAdd,tFirst,tLast,tder,Slope_sto,Tsample_sto,Fknee_sto,Fmin_sto,Nb_Ageing,hp_sto,hc_sto));
							NbSrc++;
							//throw ;
						}

						// **** Read Parameters for Cusp
						if(SourceType == "CosmicStringCusp"){
							double Amplitude(1.0e-10), CentralTime(tMax/2.0), MaximumFrequency(0.1), TPad (900.0);
							double ReqtDur(tDur), ReqtOffset(tOffset);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Amplitude")==0)
									Amplitude = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"CentralTime")==0){
									CentralTime = gXMLTime(param);
								}
								if(strcmp(ezxml_attr(param,"Name"),"MaximumFrequency")==0)
									MaximumFrequency = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"RequestDuration")==0)
									ReqtDur = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Name"),"RequestTimeOffset")==0)
									ReqtOffset = atof(ezxml_txt(param));
							}
							cout.precision(15);
							TPad -= gettOffset(); 
							cout << "  + GWCusp   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout << " Amplitude=" << Amplitude << " CentralTime=" << CentralTime << " MaximumFrequency=" << MaximumFrequency << " RequestDuration=" << ReqtDur << " RequestOffset=" << ReqtOffset << " Tpad(+tOffset)=" << TPad << endl;
							GWs.push_back(new GWCusp(Beta, Lambda, Psi, Amplitude, CentralTime, MaximumFrequency, tStepPhy, ReqtDur, TPad, ReqtOffset));
							NbSrc++;
						}	
						
						
						// **** Read Parameters for New (Template for adding a new GW type)
						if(SourceType == "New"){
							double f(0.0), hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
							for(param = ezxml_child(source,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"Frequency")==0)
									f = gXMLFrequency(param);
								if(strcmp(ezxml_attr(param,"Name"),"AmplitudePlus")==0){
									double Fact(1.0);
									char * UnitType;
									UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
									hp = atof(ezxml_txt(param))*Fact;
									cout << " - hp = " << hp << endl;
								}
								if(strcmp(ezxml_attr(param,"Name"),"AmplitudeCross")==0){
									double Fact(1.0);
									char * UnitType;
									UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
									hc = atof(ezxml_txt(param))*Fact;
									cout << " - hc = " << hc << endl;
								}
								if(strcmp(ezxml_attr(param,"Name"),"InitialPhasePlus")==0)
									Phi0hp = gXMLAngle(param);
								if(strcmp(ezxml_attr(param,"Name"),"InitialPhaseCross")==0)
									Phi0hc = gXMLAngle(param);
							}
							cout.precision(15);
							cout << "  + GWNew   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout.precision(15);
							cout << " f=" << f << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
							GWs.push_back(new GWNew(Beta, Lambda, Psi, f, hp, hc, Phi0hp, Phi0hc));
							NbSrc++;
						}
						
						// **** Read Parameters for Extreme Mass Ratio Inspiral
						if(SourceType == "ExtremeMassRatioInspiral"){
							cout << "  --> Sorry ! The EMRI are not computde actually by LISACode ! This work is in progress !" << endl;
						}
						
						/*
						// **** Read Parameters for "File" GW
						if(SourceType == "File"){
							ezxml_t series;
							const char *recordname;
							int typedata, encoding;
							char * filename;
							int length, records;
							series = ezxml_child(source,"XSIL");
							if(strcmp(ezxml_attr(series,"Type"),"TimeSeries")!=0)
								throw invalid_argument("ConfigSim::ReadXMLFile : The record type isn't known (only TimeSeries) !");
							recordname = ezxml_attr(series,"Name");
							cout << " Record " << recordname << " :" << endl;
							filename = gXMLTimeSeries(series,typedata,encoding,length,records);
							cout.precision(15);
							cout << "  + GWFile   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
							cout << " File=" << filename << endl;
							// WARNING : Impossible de reccuperer le nom de fichier qui devrait etre dans filename
							GWs.push_back(new GWFile(Beta, Lambda, Psi, filename));
							NbSrc++;
						}
						 */
						
						
					}
					
					
					// ***************  Read SampledPlaneWave Source (read in file) *************** //
					
					if((strcmp(ezxml_attr(source,"Type"),"SampledPlaneWave")==0)||(strcmp(ezxml_attr(source,"Name"),"hp,hc")==0)){
						ezxml_t filedata, param;
						string SourceType;
						char GWFileName[256];
						int GWFileEncoding(0);
						double GWTimeOffset(-1.0), GWTimeStep(-1.0), Duration(31457280.0);
						int GWLength(100), GWRecords(2);
						int typedata;
						for (filedata = ezxml_child(source, "XSIL"); filedata; filedata = filedata->next) {
							for(param = ezxml_child(filedata,"Param"); param; param = param->next){
								if(strcmp(ezxml_attr(param,"Name"),"SourceType")==0)
									SourceType = gXMLstring(param);
								if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0)
									GWTimeOffset = gXMLTime(param);
								if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0)
									GWTimeStep = gXMLTime(param);
								if(strcmp(ezxml_attr(param,"Name"),"Duration")==0)
									Duration = gXMLTime(param);
							}
							char * TmpFileName;
							TmpFileName = gXMLTimeSeries(filedata, typedata, GWFileEncoding, GWLength, GWRecords);
							strcpy(GWFileName,TmpFileName);
						}
						cout.precision(15);
						cout << "  + GWFile : Beta = " << Beta << " , Lambda = " << Lambda << " , Psi = " << Psi << " : " << GWFileName;
						if(GWFileEncoding){
							cout << ", BINARY , Offset = " << GWTimeOffset << " , TimeStep = " << GWTimeStep << " , GWLength = " << GWLength << " , GWRecords = " << GWRecords;
						}else{
							cout << ", ASCII,  Offset = " << GWTimeOffset << " , TimeStep = " << GWTimeStep;
						}
						cout << " : Load file ..."; fflush(stdout);
						GWs.push_back(new GWFile2(Beta, Lambda, Psi, GWFileName, GWFileEncoding, GWTimeOffset, GWTimeStep, GWLength, GWRecords));
						cout << " --> OK" << endl;
						NbSrc++;
					}
				}
			}
			
			
			// *******************
			// * Read Noise Data *
			// *******************
			if(strcmp(type,"NoiseData")==0){
				ezxml_t noisedata; 
				double val ;
				cout << "Section : NoiseData" << endl ;
				for (noisedata = ezxml_child(section, "XSIL"); noisedata; noisedata = noisedata->next) {
					ezxml_t param;
					const char * instrument;
					string str_xml,instrument_possible,SourceType,SpectralType;
					//eric_nul = " \n";
					int iSC(-1), IndDir(-1), InstrumentIndex ;
					double PSD(0.0), fknee(0.0) ;
					// Noise localisation in LISA : instrument and spacecraft
					instrument = ezxml_attr(noisedata,"Name");
					cout << "  Noisy Instrument = " << instrument << endl;
					// *** Read all the parameters
					for(param = ezxml_child(noisedata,"Param"); param; param = param->next){
						if(strcmp(ezxml_attr(param,"Name"),"PowerSpectralDensity")==0){
							PSD = gXMLdouble(param);
						}else{
							if(strcmp(ezxml_attr(param,"Name"),"Fknee")==0){
								fknee = gXMLdouble(param);
							}else{
								if(strcmp(ezxml_attr(param,"Name"),"GenerationTimeStep")==0){
									val = gXMLint(param);
								}else{
									if(strcmp(ezxml_attr(param,"Name"),"InterpolationOrder")==0){
										val = gXMLint(param);
									}else{
										if(strcmp(ezxml_attr(param,"Name"),"PseudoRandomSeed")==0){
											val = gXMLint(param);
										}else{
											if(strcmp(ezxml_attr(param,"Name"),"SourceType")==0){
												SourceType = gXMLstring(param);
											}else{
												if(strcmp(ezxml_attr(param,"Name"),"SpectralType")==0){
													SpectralType = gXMLstring(param);
												}else{
													cout << "string" << endl ;
													str_xml = gXMLstring(param) ;
													throw invalid_argument("ConfigSim::ReadXMLFile : Parameter not known for this noise  !");
												}
											}
										}
									}
								}
							}
						}
					}
					
					
					
					
					// *** Analyse for MLDC noise
					if(strncmp(instrument,"pm",2)==0){
						//cout << "instrument 11 :" << instrument << endl ;
						InstrumentIndex = 12; // a verifier
						if(strcmp(instrument,"pm1")==0){
							iSC = 1 ; IndDir = 0 ;
						}else{
							if(strcmp(instrument,"pm1s")==0){
								iSC = 1 ; IndDir = 1 ;
							}else{
								if(strcmp(instrument,"pm2")==0){
									iSC = 2 ; IndDir = 0 ;
								}else{
									if(strcmp(instrument,"pm2s")==0){
										iSC = 2 ; IndDir = 1 ;
									}else{
										if(strcmp(instrument,"pm3")==0){
											iSC = 3 ; IndDir = 0 ;
										}else{
											if(strcmp(instrument,"pm3s")==0){
												iSC = 3 ; IndDir = 1 ;
											}else{
												cout << "instrument 13 :" << instrument << endl ;
												iSC = 2 ; IndDir = 0 ; // bidon
											}
										}
									}
								}
							}
						}
					}
					if(strncmp(instrument,"pd",2)==0){
						//cout << "instrument 21 :" << instrument << endl ;
						InstrumentIndex = 6; // (shot noise) a verifier
						if(strcmp(instrument,"pd1")==0){
							iSC = 1 ; IndDir = 0 ;
							//cout << "case pd1 "<< endl ;
						}else{
							if(strcmp(instrument,"pd1s")==0){
								iSC = 1 ; IndDir = 1 ;
							}else{
								if(strcmp(instrument,"pd2")==0){
									iSC = 2 ; IndDir = 0 ;
								}else{
									if(strcmp(instrument,"pd2s")==0){
										iSC = 2 ; IndDir = 1 ;
									}else{
										if(strcmp(instrument,"pd3")==0){
											iSC = 3 ; IndDir = 0 ;
										}else{
											if(strcmp(instrument,"pd3s")==0){
												iSC = 3 ; IndDir = 1 ;
											}else{
												cout << "instrument 23 :" << instrument << endl ;
												iSC = 2 ; IndDir = 0 ; // bidon
											}
										}
									}
								}
							}
						}
					}
					if(strncmp(instrument,"c",1)==0){
						//cout << "instrument 31 :" << instrument << endl ;
						InstrumentIndex = 0; // (laser) a verifier
						if(strcmp(instrument,"c1")==0){
							iSC = 1 ; IndDir = 0 ;
							//cout << "case c1 "<< endl ;
						}else{
							if(strcmp(instrument,"c1s")==0){
								iSC = 1 ; IndDir = 1 ;
							}else{
								if(strcmp(instrument,"c2")==0){
									iSC = 2 ; IndDir = 0 ;
								}else{
									if(strcmp(instrument,"c2s")==0){
										iSC = 2 ; IndDir = 1 ;
									}else{
										if(strcmp(instrument,"c3")==0){
											iSC = 3 ; IndDir = 0 ;
										}else{
											if(strcmp(instrument,"c3s")==0){
												iSC = 3 ; IndDir = 1 ;
											}else{
												cout << "instrument 33 :" << instrument << endl ;
												iSC = 2 ; IndDir = 0 ; // bidon
											}
										}
									}
								}
							}
						}
					}
					// ** Test if instrument is known
					string nom_possible[] = {"pm1", "pm2", "pm3", "pm1s", "pm2s", "pm3s","pd1", "pd2", "pd3", "pd1s", "pd2s", "pd3s","c1", "c2", "c3", "c1s", "c2s", "c3s" } ;
					int test_possible ;
					instrument_possible = instrument ;
					test_possible = true ;
					for ( int j=0 ; j<18 ; j++){
						test_possible = test_possible*instrument_possible.compare(nom_possible[j]) ;
					}
					if(test_possible)
						throw invalid_argument("ConfigSim::ReadXMLFile : The instrument for noise isn't known  !");
					
					// *** Analyse for LISACode std noise
					if((iSC<1)&&(iSC>3))
						throw invalid_argument("ConfigSim::ReadXMLFile : The spacecraft for noise isn't known (only 1, 2 or 3) !");
					if((IndDir!=0)&&(IndDir!=1)&&(IndDir!=4))
						throw invalid_argument("ConfigSim::ReadXMLFile : The bench for noise isn't known (only 0 or 1) !");
					if(strcmp(instrument,"LaserNoise")==0)
						InstrumentIndex = 0;
					if(strcmp(instrument,"ShotNoise")==0)
						InstrumentIndex = 6;
					if(strcmp(instrument,"OOPNNoise")==0)
						InstrumentIndex = 6;
					if(strcmp(instrument,"MassNoise")==0)
						InstrumentIndex = 12;
					if(strcmp(instrument,"BenchNoise")==0)
						InstrumentIndex = 18;
					
					// Type of noise
					const char * noisetype;
					/*		  for(param = ezxml_child(noisedata,"Param"); param; param = param->next){
					 if(strcmp(ezxml_attr(param,"Name"),"NoiseType")==0)
					 //noisetype = ezxml_txt(param);
					 noisetype = "White"; //bidon
					 }*/
					noisetype = "unknown" ; // pourquoi ?
					if((strcmp(noisetype,"White")==0)||(strcmp(noisetype,"Filter_f")==0)||(strcmp(noisetype,"Filter_1of")==0)||(strcmp(noisetype,"Filter_fLosP")==0)||(SpectralType == "PinkAcceleration")||(SpectralType == "WhitePhase")||(SpectralType == "WhiteFrequency")){
						double level(0.0);
						NoiseSpec tmp_noise;
						/*for(param = ezxml_child(noisedata,"Param"); param; param = param->next){
						 if(strcmp(ezxml_attr(param,"Name"),noisetype)==0){
						 double Fact(1.0);
						 char * UnitType;
						 UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
						 level = atof(ezxml_txt(param))*Fact;
						 }
						 }*/
						//cout << endl << "noisetype = " << noisetype << endl ;
						//cout << "SpectralType = " << SpectralType << endl ;
						if(strcmp(noisetype,"White")==0){
							tmp_noise.NType = 1;
							tmp_noise.NVal0=level;
							cout << "  --> White : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(SpectralType == "WhitePhase"){
							tmp_noise.NType = 5; // a verifier (shot noise)
							tmp_noise.NVal0=pow(PSD,0.5);
							cout << "  --> WhitePhase : " << tmp_noise.NVal0 << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(SpectralType == "WhiteFrequency"){
							tmp_noise.NType = 1; // a verifier
							tmp_noise.NVal0=pow(PSD,0.5);
							cout << "  --> WhiteFrequency : " << tmp_noise.NVal0 << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(strcmp(noisetype,"Filter_1of")==0){
							tmp_noise.NType = 4;
							tmp_noise.NVal0=level;
							cout << "  --> Filter_1of : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(strcmp(noisetype,"Filter_f")==0){
							tmp_noise.NType = 5;
							tmp_noise.NVal0=level;
							cout << "  --> Filter_f : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(strcmp(noisetype,"Filter_fLosP")==0){
							tmp_noise.NType = 6;
							tmp_noise.NVal0=level;
							cout << "  --> Filter_fLosP : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
						}
						if(strcmp(noisetype,"Filter_MLDC_IM")==0){
							tmp_noise.NType = 7;
							tmp_noise.NVal0=level;
							cout << "  --> Filter_MLDC_IM : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
							throw invalid_argument("ConfigSim::ReadXMLFile : One parameter is missing in xml file for Filter_MLDC_IM !");	
						}	
						if(SpectralType == "PinkAcceleration"){
							tmp_noise.NType = 7;
							tmp_noise.NVal0 = pow(PSD,0.5);
							tmp_noise.NVal01 = fknee; // frequency of ankle
							//tmp_noise.NVal01=1e-8; // frequency of ankle
							cout << "  --> PinkAcceleration : ISC, IndDir, Level, fknee = " << iSC << " " << IndDir << " " << tmp_noise.NVal0  << " " << tmp_noise.NVal01 << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
							//throw invalid_argument("ConfigSim::ReadXMLFile : One parameter is missing in xml file for Filter_MLDC_IM !");		
						}
					}						
					if(strcmp(noisetype,"FilterCoef")==0){
						NoiseSpec tmp_noise;
						for(param = ezxml_child(noisedata,"Param"); param; param = param->next){
							tmp_noise.NType = 3;
							if(strcmp(ezxml_attr(param,"Name"),"Alpha")==0){
								double Fact(1.0);
								char * UnitType;
								UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
								(tmp_noise.NVal1).push_back(atof(ezxml_txt(param))*Fact);
								cout << "   - Alpha " << (tmp_noise.NVal1).size()-1 << " = " << (tmp_noise.NVal1)[(tmp_noise.NVal1).size()-1] << endl;
								
							}
							if(strcmp(ezxml_attr(param,"Name"),"Beta")==0){
								double Fact(1.0);
								char * UnitType;
								UnitType = gXMLUnit(ezxml_attr(param,"Unit"),Fact);
								(tmp_noise.NVal2).push_back(atof(ezxml_txt(param))*Fact);
								cout << "   - Beta " << (tmp_noise.NVal2).size()-1 << " = " << (tmp_noise.NVal2)[(tmp_noise.NVal2).size()-1] << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"Stabilization")==0){
								tmp_noise.NVal0 = gXMLTime(param);
							}
						}
					}
				}
			}
			
			// *******************
			// *  Read TDI Data  *
			// *******************
			/*
			 if(strcmp(type,"TDIData")==0){
			 ezxml_t tdidata;
			 for (tdidata = ezxml_child(section, "XSIL"); tdidata; tdidata = tdidata->next) {
			 const char * generatorname;
			 vector<int> tmp_TDIPacks;
			 vector<double> tmp_TDIPacksFact;
			 if(strcmp(ezxml_attr(tdidata,"Type"),"TDI")==0){
			 ezxml_t param;
			 bool UnKnowTDI(true);
			 for(param = ezxml_child(tdidata,"Param"); param; param = param->next){
			 if(strcmp(ezxml_attr(param,"Name"),"GeneratorName")==0){
			 generatorname = ezxml_txt(param);
			 cout << " TDI : Name = " << generatorname << endl;
			 }
			 }
			 // Search if TDI generator is predifined
			 UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks, tmp_TDIPacksFact);
			 
			 // Read unknow TDI generator
			 if(UnKnowTDI){
			 cout << "       Pack = ";
			 for(param = ezxml_child(tdidata,"Param"); param; param = param->next){
			 if(strcmp(ezxml_attr(param,"Name"),"Pack")==0){
			 int tmp_pack;
			 int NbDelays(0);
			 tmp_pack = atoi(ezxml_txt(param));
			 cout << " " << tmp_pack ;
			 tmp_TDIPacks.push_back(tmp_pack);
			 tmp_TDIPacksFact.push_back(1.0);
			 while(!((double)(tmp_pack/pow(10.0,NbDelays))<1.0)){
			 NbDelays++;
			 }
			 }
			 }
			 cout << endl;
			 }
			 TDIsName.push_back(generatorname);
			 TDIsPacks.push_back(tmp_TDIPacks);
			 TDIsPacksFact.push_back(tmp_TDIPacksFact);
			 cout << endl;
			 }
			 if(strcmp(ezxml_attr(tdidata,"Type"),"TDIObservable")==0){
			 ezxml_t param, series;
			 char genname[256];
			 char cfirstptr;
			 int iptr1, iptr2;
			 bool UnKnowTDI(true);
			 const char *genl;
			 //cout <<"TDI 1 encoding = "<<getFileEncodingTDI() << endl ;
			 
			 // const char *recordname;
			 // int typedata, encoding;
			 // const char *genl;
			 // char * filename;
			 // int length, records;
			 // double timeoffset(0.0), cadence(0.0), duration(0.0);
			 
			 // Read generator list
			 genl = ezxml_attr(tdidata,"Name");
			 
			 // Extract generator name
			 strcpy(genname," ");
			 iptr2 = 0;
			 cout << "-----------" << endl;
			 cout << "TDI generator :" << endl;
			 cout << "genl :"<<genl<< endl ;
			 do{
			 iptr1 = iptr2;
			 //cout << "# Read junk " << iptr1 ; fflush(stdout);
			 while((genl[iptr1]=='t')||(genl[iptr1]==',')||(genl[iptr1]==' ')){
			 iptr1++;
			 }
			 //cout << " --> " << iptr1 << endl; fflush(stdout);
			 iptr2 = iptr1;
			 //cout << "# Read word " << iptr2 ; fflush(stdout);
			 while((genl[iptr2]!=',')&&(genl[iptr2]!='\0')&&(genl[iptr2]!=' ')){
			 iptr2++;
			 }
			 //cout << " --> " << iptr2 << endl; fflush(stdout);
			 cfirstptr = genl[iptr1];
			 //cout << "# First char " << cfirstptr << endl; fflush(stdout);
			 strncpy(genname,strchr(genl,cfirstptr),(iptr2-iptr1));
			 genname[iptr2-iptr1] = '\0';
			 cout << "  -  " << genname ; fflush(stdout);
			 UnKnowTDI = FindTDIName(genname, tmp_TDIPacks, tmp_TDIPacksFact);
			 if(UnKnowTDI){
			 cout << " --> Sorry, it is not known !" << endl;
			 }else{
			 //cout << "genname ="<<genname<< endl;
			 TDIsName.push_back(genname);
			 TDIsPacks.push_back(tmp_TDIPacks);
			 TDIsPacksFact.push_back(tmp_TDIPacksFact);
			 cout << " --> OK !" << endl;
			 }
			 
			 }while(genl[iptr2]==',');
			 
			 // Read the filename
			 cout << "TDI filename ..." << endl;
			 series = ezxml_child(tdidata,"XSIL");
			 //cout << "series:"<<series<<endl ;
			 if(series !=0){
			 // Normally the following parameters are defined elsewhere
			 int typedata, encoding;
			 char * filename;
			 int length, records;
			 double timeoffset(0.0), cadence(0.0), duration(0.0);
			 if(strcmp(ezxml_attr(series,"Type"),"TimeSeries")!=0)
			 throw invalid_argument("ConfigSim::ReadXMLFile : The record type isn't known (only TimeSeries) !");
			 filename = gXMLTimeSeries(series,typedata,encoding,length,records);
			 
			 for(param = ezxml_child(series,"Param"); param; param = param->next){
			 if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0)
			 timeoffset = gXMLTime(param);
			 if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0)
			 cadence = gXMLTime(param); 
			 if(strcmp(ezxml_attr(param,"Name"),"Duration")==0)
			 duration = gXMLTime(param);
			 }
			 
			 
			 cout << "------- Read TDI Record Parameter ----- " << endl;
			 cout << "  - filename   = " << filename << endl;
			 cout << "  - typedata   = " << typedata << endl;
			 cout << "  - encoding   = " << encoding << endl;
			 cout << "  - length     = " << length << endl;
			 cout << "  - records    = " << records << endl;
			 cout << "  - timeoffset = " << timeoffset << endl;
			 cout << "  - cadence    = " << cadence << endl;
			 cout << "  - duration   = " << duration << endl;
			 cout << endl; 
			 strcpy(FileNameTDI,filename);
			 FileEncodingTDI  = encoding;
			 //cout << "test_eric encoding = " << getFileEncodingTDI() << endl ;
			 //tStepPhy = cadence;
			 tStepMes = cadence;
			 tMax = duration;
			 if(duration/1000<10000*cadence)
			 tDisplay = duration/1000;
			 else
			 tDisplay = 10000*cadence;
			 }
			 }
			 
			 }
			 }
			 */
			
		}
		
		ezxml_free(tree);
		
		
		// Creation of tMemNoiseFirst and tMemNoiseLast
		double OrderLagrangeNoise(7);
		tMemNoiseFirst = tStepPhy*ceil(2+OrderLagrangeNoise/2.0);
		tMemNoiseLast = -1.0*(tMaxDelay()+tStepPhy*ceil(2+OrderLagrangeNoise/2.0));
		
		// Noises creation
		NoisesCreation();
		
		// Display of TDI generators
		cout << "  ----------" << endl;
		for(int i=0; i<TDIsName.size(); i++){
			cout << "  " << TDIsName[i] << " :";
			for(int j=0; j<TDIsPacks[i].size(); j++){
				cout << " " << (TDIsPacks[i])[j];
				if(fabs((TDIsPacksFact[i])[j]-1.0) > PRECISION)
					cout << "*" << (TDIsPacksFact[i])[j];
			}
			cout << endl;
		}
				
		
		// ** Default value for display time step
		if(tDisplay<0.0)
			tDisplay = tDur/200.0;
		
		cout << "  ----------" << endl;
		cout << "  Sources number = " << NbSrc << endl;
		cout << "  tStepPhy       = " << tStepPhy << " s" << endl;
		cout << "  tStepMes       = " << tStepMes << " s" << endl;
		cout << "  tOffset        = " << tOffset << " s" << endl;
		cout << "  tMax           = " << tMax << " s" << endl;
		cout << "  tDuration      = " << tDur << " s" << endl;
		cout << "  tDeltaTDIDelay = " << tDeltaTDIDelay << " s" << endl;
		cout << "  tStepDisplay   = " << tDisplay << " s" << endl;
		cout << "  NbMaxDelays    = " << NbMaxDelays << endl;
		cout << "  InterpUtilVal  = " << TDIInterpVal << endl;
		cout << "  Orbits type    = " << OrbType << endl;
		cout << "  Orbits approx  = " << OrbApprox << endl;
		cout << "  Delay order    = " << OrbOrder << endl; 
		
		if(XmlOutputFile != "None"){
			cout << endl << " ==========  Create XML Outputfile ==========" << endl << endl;
			CreateXmlOutputFile();
		}
	}
	
	catch( exception & e ) {       
		cerr << "ConfigSim::ReadXMLFile : error : " << e.what() << endl;
		throw;
	}
	//cout << "fin eric"<< endl ;
	//throw;
}



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*!\brief
 * Ceates XML Output file for MLDC.
 *
 */
void ConfigSim::CreateXmlOutputFile()
{
	char *  OutputFileName;
	string xsil_beg("<XSIL>\n"),xsil_end("</XSIL>\n");
	string spc_5("     "),spc_9("         "),spc_3("    ");
	string ind1("     "), ind2(ind1+ind1), ind3(ind2+ind1), ind4(ind3+ind1), ind5(ind4+ind1), ind6(ind5+ind1);
	
	string ParamName("<Param Name=\""),Param_end("</Param>\n");
	string ArrayName("<Array Name=\""),Array_end("</Array>\n");
	string DimName("<Dim Name=\""),Dim_end("</Dim>\n");
	string StreamType("<Stream Type=\""),Stream_end("</Stream>\n");
	string ObsName;

	OutputFileName = getXmlOutputFile();
	FichXML.open(OutputFileName);
	cout << "Creating XML Output File :" << OutputFileName << endl ;
	// Header
	FichXML << "<?xml version=\"1.0\"?>" << endl;
	FichXML << "<!DOCTYPE XSIL SYSTEM \"http://www.vallis.org/lisa-xml.dtd\">" << endl ;
	FichXML << "<?xml-stylesheet type=\"text/xsl\" href=\"lisa-xml.xsl\"?>" << endl ;
	FichXML <<xsil_beg;
	
	FichXML << ind1 << ParamName << "Author\" >" << endl ;
	FichXML << ind2 << getAuthor() << endl;
	FichXML << ind1 << Param_end ;
	FichXML << ind1 << ParamName << "GenerationDate\" Type =\"ISO-8601\"> "<< endl ;
	FichXML << ind2 << MathUtils::TimeISO8601()  << endl;
	FichXML << ind1 << Param_end ;
	FichXML << ind1 << ParamName <<"Simulator\">" << endl ;
	FichXML << ind2 << "LISACode-v" << LCVersion << "-" << DateOfLastUpdate << endl;
	FichXML << ind1 << Param_end ;
	
	// *** Output data
	FichXML << ind1 << "<XSIL Type=\"TDIData\">"<<endl;
	
	// ** TDI data
	string TDIsGenName;
	TDIsGenName = "t";
	for(int iTDI=0; iTDI<TDIsName.size(); iTDI++){
		TDIsGenName += ",";
		TDIsGenName += TDIsName[iTDI];
	}
	AddTimeSeriesInXMLOutput(&FichXML, ind1, TDIsGenName, tOffset, 4, getFileEncodingTDI(), getFileNameTDI());
	
	// ** SC1 Phasemeter data
	if(strcmp(getFileNameSig(1),"None")!=0){
		if(UseInternalPhasemeter())
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y231,y321,z231,z321", tOffset+gettStartPhasemeters(), 5, getFileEncodingSig(1), getFileNameSig(1));
		else
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y231,y321", tOffset+gettStartPhasemeters(), 3, getFileEncodingSig(1), getFileNameSig(1));
	}
	
	// ** SC2 Phasemeter data
	if(strcmp(getFileNameSig(2),"None")!=0){
		if(UseInternalPhasemeter())
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y312,y132,z312,z132", tOffset+gettStartPhasemeters(), 5, getFileEncodingSig(2), getFileNameSig(2));
		else
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y312,y132", tOffset+gettStartPhasemeters(), 3, getFileEncodingSig(2), getFileNameSig(2));
	}
	
	// ** SC3 Phasemeter data
	if(strcmp(getFileNameSig(3),"None")!=0){
		if(UseInternalPhasemeter())
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y123,y213,z123,z213", tOffset+gettStartPhasemeters(), 5, getFileEncodingSig(3), getFileNameSig(3));
		else
			AddTimeSeriesInXMLOutput(&FichXML, ind1, "t,y123,y213", tOffset+gettStartPhasemeters(), 3, getFileEncodingSig(3), getFileNameSig(3));
	}
	
	FichXML << ind1 << xsil_end;
	FichXML << xsil_end;
	FichXML.close();
}

void ConfigSim::AddTimeSeriesInXMLOutput(ofstream * FichXML, string ind1, string ObsDescr, double tOffsetTS, int NRec, int DataFileEncoding, char * DataFileName)
{
	string ind2(ind1+ind1), ind3(ind2+ind1), ind4(ind3+ind1), ind5(ind4+ind1), ind6(ind5+ind1);
	string ParamName("<Param Name=\""),Param_end("</Param>\n");
	string ArrayName("<Array Name=\""),Array_end("</Array>\n");
	string DimName("<Dim Name=\""),Dim_end("</Dim>\n");
	string StreamType("<Stream Type=\""),Stream_end("</Stream>\n");
	string xsil_end("</XSIL>\n");
	
	(*FichXML).precision(12);
	
	(*FichXML) << ind2 << "<XSIL Name=\"" << ObsDescr << "\" Type=\"TDIObservable\">"<< endl;
	(*FichXML) << ind3 << ParamName <<"DataType\">"<<endl;
	(*FichXML) << ind4 <<"FractionalFrequency"<< endl;
	(*FichXML) << ind3 << Param_end;
	(*FichXML) << ind3 << "<XSIL Name=\"" << ObsDescr << "\" Type=\"TimeSeries\">"<< endl;
	(*FichXML) << ind4 << ParamName << "TimeOffset\" Unit=\"Second\">" << endl ;
	(*FichXML) << ind5 << tOffsetTS << endl;
	(*FichXML) << ind4 << Param_end;
	(*FichXML) << ind4 << ParamName << "Cadence\" Unit=\"Second\">" << endl ;
	(*FichXML) << ind5 << gettStepMes() << endl;
	(*FichXML) << ind4 << Param_end;
	(*FichXML) << ind4 << ParamName << "Duration\" Unit=\"Second\">" << endl ;
	(*FichXML) << ind5 << tDur << endl;
	(*FichXML) << ind4 << Param_end;
	(*FichXML) << ind4 << ArrayName << ObsDescr << "\" Type=\"double\" Unit=\"Word\">" << endl;
	(*FichXML) << ind5 << DimName <<"Length\">"<<endl;
	(*FichXML) << ind6 << int(gettDur()/gettStepMes()) << endl;
	(*FichXML) << ind5 << Dim_end;
	(*FichXML) << ind5 << DimName <<"Records\">" << endl;
	(*FichXML) << ind6 << NRec << endl;
	(*FichXML) << ind5 << Dim_end;
	if(DataFileEncoding==0)
		(*FichXML) << ind5 << StreamType<<"Remote\" Encoding=\"ASCII\">" <<endl;
	else
		(*FichXML) << ind5 << StreamType<<"Remote\" Encoding=\"Binary," << getSystemEncoding() << "\">" << endl;
	(*FichXML) << ind6 << DataFileName << endl;
	(*FichXML) << ind5 << Stream_end;
	(*FichXML) << ind4 << Array_end;
	(*FichXML) << ind3 << xsil_end;
	(*FichXML) << ind2 << xsil_end;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*!\brief
 * Character to double conversion.
 *
 * Fact double output is converted from In charcater pointer input.
 * This function returns end of string character pointer.
 */
char * ConfigSim::gXMLUnit(const char In[], double & Fact){
	char * EndPtr;
	double res;
	res = strtod(In,&EndPtr);
	if(EndPtr==In) res=1;
	Fact = res;
	return(EndPtr);
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*!\brief
 * Extracts time parameter from XML sctructure.
 *
 * Checks if unit is second.
 */

double ConfigSim::gXMLTime(ezxml_t param)
{
	double TimeParam(1.0);
	char * UnitType;
	UnitType = gXMLUnit(ezxml_attr(param,"Unit"), TimeParam);
	if((strcmp(UnitType,"Second")!=0)&&strcmp(UnitType,"second")!=0)
		throw invalid_argument("ConfigSim::gXMLTime : The time's unit isn't known (only Second) !");
	TimeParam *= atof(ezxml_txt(param));
	cout << "   x " << ezxml_attr(param,"Name") << " = " << TimeParam << " Second" << endl;
	return (TimeParam);
}

/*!\brief
 * Extracts angle parameter from XML sctructure.
 *
 * Checks if unit is degree.
 */
double ConfigSim::gXMLAngle(ezxml_t param)
{
	double AngleParam(1.0);
	char * UnitType;
	UnitType = gXMLUnit(ezxml_attr(param,"Unit"), AngleParam);
	AngleParam *= atof(ezxml_txt(param));
	if(strcmp(UnitType,"Degree")==0){
		AngleParam = MathUtils::deg2rad(AngleParam);
	}
	if((strcmp(UnitType,"Degree")!=0)&&(strcmp(UnitType,"Radian")!=0))
		throw invalid_argument("ConfigSim::gXMLAngle : The angle's unit isn't known (only Degree or Radian) !");
	cout << "   x " << ezxml_attr(param,"Name") << " = " << AngleParam << " Radian" << endl;
	return (AngleParam);
}
/*!\brief
 * Extracts frequency parameter from XML sctructure.
 *
 * Checks if unit is MilliHertz.
 */
double ConfigSim::gXMLFrequency(ezxml_t param)
{
	double FrequencyParam(1.0);
	char * UnitType;
	UnitType = gXMLUnit(ezxml_attr(param,"Unit"), FrequencyParam);
	FrequencyParam *= atof(ezxml_txt(param));
	if(strcmp(UnitType,"MilliHertz")==0){
		FrequencyParam *= 1.0e-3;
	}
	if((strcmp(UnitType,"MilliHertz")!=0)&&(strcmp(UnitType,"Hertz")!=0))
		throw invalid_argument("ConfigSim::gXMLFrequency : The frequency's unit isn't known (only MilliHertz or Hertz) !");
	cout << "   x " << ezxml_attr(param,"Name") << " = " << FrequencyParam << " Hertz" << endl;
	return (FrequencyParam);
}
/*!\brief
 * Extracts AstroMass parameter from XML sctructure.
 *
 * Checks if unit is SolarMass.
 */
double ConfigSim::gXMLAstroMass(ezxml_t param)
{
	double AstroMass(1.0);
	char * UnitType;
	UnitType = gXMLUnit(ezxml_attr(param,"Unit"), AstroMass);
	AstroMass *= atof(ezxml_txt(param));
	if(strcmp(UnitType,"SolarMass")!=0)
		throw invalid_argument("ConfigSim::gXMLAstroMass : The astronomic mass's unit isn't known (only SolarMass) !");
	cout << "   x " << ezxml_attr(param,"Name") << " = " << AstroMass << " SolarMass" << endl;
	return (AstroMass);
}
/*!\brief
 * Extracts AstroDistance parameter from XML sctructure.
 *
 * Checks if unit is Parsec.
 */
double ConfigSim::gXMLAstroDistance(ezxml_t param)
{
	double AstroDistance;
	char * UnitType;
	UnitType = gXMLUnit(ezxml_attr(param,"Unit"), AstroDistance);
	AstroDistance *= atof(ezxml_txt(param));
	if(strcmp(ezxml_attr(param,"Unit"),"Parsec")==0){
		AstroDistance *= 1.0e-3;
	}
	if((strcmp(UnitType,"Parsec")!=0)&&(strcmp(UnitType,"KiloParsec")!=0))
		throw invalid_argument("ConfigSim::gXMLAstroDistance : The astronomic distance's unit isn't known (only Parsec or KiloParsec) !");
	cout << "   x " << ezxml_attr(param,"Name") << " = " << AstroDistance << " KiloParsec" << endl;
	return (AstroDistance);
}



char * ConfigSim::gXMLWord(ezxml_t param)
{
	static char theWord[256];
	char * ptr; 
	char cfirstptr;
	int iptr1, iptr2;
	
	ptr = (*param).txt;
	iptr1 = 0;
	while((ptr[iptr1]==' ')||(ptr[iptr1]=='\n')||(ptr[iptr1]=='\t')){
		iptr1++;
	}
	iptr2 = iptr1;
	//while((ptr[iptr2]!=' ')&&(ptr[iptr2]!='\n')&&(ptr[iptr2]!='\t'))
	while((ptr[iptr2]!=' ')&&(ptr[iptr2]!='\n')&&(ptr[iptr2]!='\t')){
		iptr2++;
	}
	ptr[iptr2]='\0';
	cfirstptr = ptr[iptr1];
	strcpy(theWord,strchr(ptr,cfirstptr));
	
	cout << "   x Word = " << theWord << endl;
	
	return(theWord);
}



/*!\brief
 * Extracts a value (double) parameter from XML sctructure.
 *
 * NO units
 */
double ConfigSim::gXMLdouble(ezxml_t param)
{
	double result(1.0);
	result = atof(ezxml_txt(param));
	cout << "   x " << ezxml_attr(param,"Name") << " = " << result << endl;
	return (result);
}


/*!\brief
 * Extracts a string from XML structure.
 *
 * NO units
 */
string  ConfigSim::gXMLstring(ezxml_t param)
{
	string result,str_find,str_nul,str_end,str_space,str_tab;
	char * tmpW;
	str_nul = " \n";
	str_end = "\n";
	tmpW = stripcopy((*param).txt);
	str_find = tmpW;
	str_find = str_find.substr(str_find.find_first_not_of(str_nul)) ;
	str_find = str_find.substr(0,str_find.find_first_of(str_end)) ;
	result = str_find;
	cout << "   x " << ezxml_attr(param,"Name") << " = " << result << endl;
	return (result);
}


/*!\brief
 * Extracts a value (double) parameter from XML sctructure.
 *
 * NO units
 */
int ConfigSim::gXMLint(ezxml_t param)
{
	int result(0);
	result = atoi(ezxml_txt(param));
	cout << "   x " << ezxml_attr(param,"Name") << " = " << result << endl;
	return (result);
}


/*!\brief
 * Extracts TimeSeries parameters from XML sctructure.
 *
 * Sets parameters :
 * \arg typedata get the type of record data : 0 for ASCII, 1 for Binary,LittleEndian and 2 for Binary,BigEndian (extracted from "Stream" child of series input)
 * \arg encoding (extracted from "Stream" child of series input)
 * \arg length and records (extracted from "Array" child of series input)
 * Returns filenename (extracted from "Array" child of series input).
 */
char * ConfigSim::gXMLTimeSeries(ezxml_t series, int & typedata, int & encoding, int & length, int & records)
{
	ezxml_t  array,dim, stream;
	static char filename[256];
	char * ptr; 
	char cfirstptr;
	int iptr1, iptr2;
	
	array= ezxml_child(series,"Array");
	
	// Read dimensions
	for(dim = ezxml_child(array,"Dim"); dim; dim = dim->next){
		if(strcmp(ezxml_attr(dim,"Name"),"Length")==0){
			length = atoi(ezxml_txt(dim));
		}
		if(strcmp(ezxml_attr(dim,"Name"),"Records")==0){
			records = atoi(ezxml_txt(dim));
		}
	}
	
	// Read file parameters
	stream = ezxml_child(array,"Stream");
	//filename = ezxml_txt(stream);
	ptr = (*stream).txt;
	iptr1 = 0;
	while((ptr[iptr1]==' ')||(ptr[iptr1]=='\n')||(ptr[iptr1]=='\t')){
		iptr1++;
	}
	iptr2 = iptr1;
	//while((ptr[iptr2]!=' ')&&(ptr[iptr2]!='\n')&&(ptr[iptr2]!='\t'))
	while((ptr[iptr2]!='\n')&&(ptr[iptr2]!='\t')){
		iptr2++;
	}
	ptr[iptr2]='\0';
	cfirstptr = ptr[iptr1];
	strcpy(filename,strchr(ptr,cfirstptr));
	
	if(strcmp(uppercase(ezxml_attr(stream,"Encoding")),"ASCII")==0)
		encoding = 0;
	else{
		encoding = 1;
	}
	
	cout << "   x FileName = " << filename << endl;
	
	cout << "   x Type     = " ;
	typedata = -1;
	if(strcmp(ezxml_attr(array,"Type"),"double")==0){
		typedata = 8;
		cout << "double" << endl;
	}else{
		cout << " Unknow type data !" << endl;
	}
	
	cout << "   x Encoding = " << encoding;
	switch(encoding){
		case 0 :
			cout << " : ASCII" << endl;
			break;
		case 1 :
			cout << " : Binary" << endl;
			break;
		default:
			cout << " : Unknown" << endl;
	}
	
	cout << "   x Length   = " << length << endl;
	cout << "   x Records  = " << records << endl;
	
	return(filename);
}


/*!\brief
 * Adds a noise specification structure.
 *
 * A noise specification structure NoiseSpec containing tm_noise input information is pushed.
 * The place where it is pushed is defined by inputs: 
 * \arg iSC : spacecraft number
 * \arg IndDir : directional index
 * \arg InstrumentIndex : instrument index
 *
 */
void ConfigSim::NoisePlace(NoiseSpec tmp_noise, int iSC, int IndDir, int InstrumentIndex)
{
	//cout << "--> Place noise " << iSC << " " << IndDir << " " << InstrumentIndex << endl;
	if(iSC!=4){
		if(IndDir!=4){
			NoisesData[iSC-1+3*IndDir+InstrumentIndex].push_back(tmp_noise);
		}else{
			NoisesData[iSC-1+InstrumentIndex].push_back(tmp_noise);
			NoisesData[iSC-1+3+InstrumentIndex].push_back(tmp_noise);
		}
	}else{
		for(int liSC=1; liSC<=3; liSC++){
			if(IndDir!=4){
				NoisesData[liSC-1+3*IndDir+InstrumentIndex].push_back(tmp_noise);
			}else{
				//cout << " --------------> Add noise at " << liSC-1+InstrumentIndex << " and " << liSC-1+3+InstrumentIndex << endl;
				NoisesData[liSC-1+InstrumentIndex].push_back(tmp_noise);
				NoisesData[liSC-1+3+InstrumentIndex].push_back(tmp_noise);
			}
		}
	}
	
}
/*!\brief
 * Creation of noises.
 *
 * Number of created noises is given by size of NoisesData.
 */
void ConfigSim::NoisesCreation()
{
	//cout << " Creation of " << NoisesData.size() << " noises." << endl;
	int noise_TwoFilter;
	for(int Index=0; Index<Noises.size(); Index++){
		//cout << "  - Noise " << Index << " : " << (NoisesData[Index]).size() << " data" << endl;
		if ((NoisesData[Index]).size() != 0){
			Noise * iNoise;
			vector< vector<double> > alpha(0), beta(0);
			int NbDataStab(0);
			for(int sNoise=0; sNoise<(NoisesData[Index]).size(); sNoise++){
				NoiseSpec tmp_n;
				tmp_n =  (NoisesData[Index])[sNoise];
				noise_TwoFilter = 0;
				cout << "  - Noise " << Index << " (" << (NoisesData[Index]).size() << " spec): "; fflush(stdout);
				// ** White noise
				if (tmp_n.NType == 1){
					cout << "White " << tmp_n.NVal0;
					iNoise = new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, tmp_n.NVal0);
				}
				// ** Noise read in file
				if (tmp_n.NType == 2){
					cout << "File " << tmp_n.NStr << " :  "; fflush(stdout);
					iNoise = new NoiseFile(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, tmp_n.NStr,  tmp_n.NVal0);
				}
				// ** Filter noise with explicit specification of coefficients
				if (tmp_n.NType == 3){
					cout << "Filter : ";
					alpha.push_back(tmp_n.NVal1);
					beta.push_back(tmp_n.NVal2);
					cout << "alpha =";
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " , beta =";
					for(int i=0; i<(beta[0]).size(); i++){
						(beta[0])[i]*=tStepPhy;
						cout << " " << (beta[0])[i];
					}
					cout << " , Stabilization = " << tmp_n.NVal0;
					NbDataStab = int(tmp_n.NVal0+0.1);
				}
				// ** Filter noise in f^-1 : NVal0 -> factor
				if (tmp_n.NType == 4){
					double tmpVal(0.0);
					if(alpha.size() == 0){
						alpha.resize(1);
						beta.resize(1);
						(alpha[0]).resize(1);
						(beta[0]).resize(2);
						(alpha[0])[0] = 1.0;
						(beta[0])[0] = 0.0;
						(beta[0])[1] = 0.0;
					}else{
						if(((alpha[0]).size()!=1)||((beta[0]).size()!=2)){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
						if(fabs((alpha[0])[0]-(1.0))>PRECISION){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
					}
					cout << "Filter f-1 : ";
					cout << "Level = " << tmp_n.NVal0 << " : ";
					tmpVal = tmp_n.NVal0*M_PI*tStepPhy;
					(beta[0])[0] = sqrt(((beta[0])[0])*((beta[0])[0]) + tmpVal*tmpVal);
					(beta[0])[1] = (beta[0])[0];
					cout << "alpha =";
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " , beta =";
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
				}
				// ** Filter noise in f : NVal0 -> factor
				if (tmp_n.NType == 5){
					double tmpVal(0.0);
					if(alpha.size() == 0){
						alpha.resize(1);
						beta.resize(1);
						(alpha[0]).resize(1);
						(beta[0]).resize(2);
						(alpha[0])[0] = -1.0;
						(beta[0])[0] = 0.0;
						(beta[0])[1] = 0.0;
					}else{
						if(((alpha[0]).size()!=1)||((beta[0]).size()!=2)){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
						if(fabs((alpha[0])[0]-(-1.0))>PRECISION){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
					}
					cout << "Filter f : ";
					cout << "Level = " << tmp_n.NVal0 << " : ";
					tmpVal = tmp_n.NVal0/(M_PI*tStepPhy);
					(beta[0])[0] = sqrt(((beta[0])[0])*((beta[0])[0]) + tmpVal*tmpVal); 
					(beta[0])[1] = -1.0*(beta[0])[0];				
					cout << "alpha =";
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " , beta =";
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
				}
				// ** Filter noise in f*L/sqrt(P) : NVal0 -> factor
				if (tmp_n.NType == 6){
					double tmpVal(0.0);
					if(alpha.size() == 0){
						alpha.resize(1);
						beta.resize(1);
						(alpha[0]).resize(1);
						(beta[0]).resize(2);
						(alpha[0])[0] = -1.0;
						(beta[0])[0] = 0.0;
						(beta[0])[1] = 0.0;
					}else{
						if(((alpha[0]).size()!=1)||((beta[0]).size()!=2)){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
						if(fabs((alpha[0])[0]-(-1.0))>PRECISION){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
					}
					cout << "Filter f*L/sqrt(P) : "; fflush(stdout);
					cout << "Level = " << tmp_n.NVal0 << " : "; fflush(stdout);
					tmpVal = tmp_n.NVal0/(M_PI*tStepPhy)*(Armlength/L0_m_default)*sqrt(LaserPower_W_default/LaserPower);
					(beta[0])[0] = sqrt(((beta[0])[0])*((beta[0])[0]) + tmpVal*tmpVal); 
					(beta[0])[1] = -1.0*(beta[0])[0];
					cout << "alpha ="; fflush(stdout);
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i]; fflush(stdout);
					}
					cout << " , beta ="; fflush(stdout);
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i]; fflush(stdout);
					}
				}
				// modif e.p. Octobre 2007 (puts the MLDC noise for IM)
				// ** Filter MLDC_IM
				if (tmp_n.NType == 7){
					double tmpVal(0.0);
					double coef_alpha_ep;
					coef_alpha_ep=1-1.0e-14; // correction to saturate at low frequencies
					//	coef_alpha_ep=1.;
					if(alpha.size() == 0){
						alpha.resize(1);
						beta.resize(1);
						(alpha[0]).resize(2);
						(beta[0]).resize(3);
						(alpha[0])[0] = 2.0*coef_alpha_ep;
						(alpha[0])[1] = -1.0*coef_alpha_ep;
						(beta[0])[0] = 0.0;
						(beta[0])[1] = 0.0;
						(beta[0])[2] = 0.0;
					}else{
						if(((alpha[0]).size()!=2)||((beta[0]).size()!=3)){
							throw invalid_argument("ConfigSim::ReadFile : Specification's noises of multi-noises are not the same !");
						}
						
					}
					// 1/f≤ Noise
					tmpVal = tmp_n.NVal0*tmp_n.NVal01*M_PI*M_PI*tStepPhy*tStepPhy;
					cout << endl << "     Filter MLDC_IM (f2) : "; fflush(stdout);
					cout << "Level = " <<tmp_n.NVal01  << " : "; fflush(stdout);
					(beta[0])[0] = tmpVal;
					(beta[0])[1] = 2*(beta[0])[0];
					(beta[0])[2] = (beta[0])[0]; 
					cout << "alpha =";
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " ,   beta ="; fflush(stdout);
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
					NbDataStab = 100000;
					cout << "  ; NbDataStab   "<< NbDataStab; fflush(stdout);
					Filter NF_MLDC_IM_f2(alpha, beta, NbDataStab);
					
					// 1/f Noise
					cout << endl << "     Filter MLDC_IM (f1) : ";
					cout << "Level = " << tmp_n.NVal0 << " : ";
					(alpha[0]).resize(1);
					(beta[0]).resize(2);
					(alpha[0])[0] = 1.0*coef_alpha_ep;
					(beta[0])[0] = tmp_n.NVal0*M_PI*tStepPhy;
					(beta[0])[1] = (beta[0])[0];
					cout << "alpha ="; fflush(stdout);
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " ,  beta ="; fflush(stdout);
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
					cout << "  ; NbDataStab   "<< NbDataStab; fflush(stdout);
					Filter NF_MLDC_IM_f1(alpha, beta, NbDataStab);
					iNoise = new NoiseTwoFilter(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, NF_MLDC_IM_f1, NF_MLDC_IM_f2);
					noise_TwoFilter = 1;
					
				}
				// end of modif e.p.
				cout << endl;
			}
			// ** For all filtred noises
			if((alpha.size()!=0)&&(beta.size()!=0)&&(noise_TwoFilter != 1)){
				Filter NF_tmp(alpha, beta, NbDataStab);
				iNoise = new NoiseFilter(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, NF_tmp);
			}
			if(Index > Noises.size()-1)
				Noises.resize(Index+1);
			Noises[Index] = iNoise;
		}
	}
}
/*!\brief
 * Computes maximal time travel for one delay.
 *
 * Maximal time travel for one delay is
 * \f$ tMaxDelay= tStepMes  \cdot ceil (\frac{6 \cdot Armlength }{5 \cdot tStepMes \cdot C}) \f$ .
 *
 */
double ConfigSim::tMaxDelay()
{
	return(tStepMes*ceil((6.0/5.0)*Armlength/(tStepMes*c_SI)));
}
/*!\brief Computes minimal time travel for one delay.
 * 
 * Minimal time travel for one delay is 
 * \f$ tMinDelay = tStepMes  \cdot ceil \frac{4 \cdot Armlength }{5 \cdot tStepMes \cdot C} \f$ .
 */
double ConfigSim::tMinDelay()
{
	return(tStepMes*ceil((4.0/5.0)*Armlength/(tStepMes*c_SI)));
}
/*!\brief
 * Computes memory time during which data must be saved for apply TDI interpolation.
 *
 * Memory time during which data must be saved for apply TDI interpolation is :
 * \f[ tMemNecInterpTDI =  \left\{ \begin{array}{ll}
 \big(2 + ceil(\frac{TDIInterpVal}{2}) \big) \cdot tStepMes & \textrm{ if (TDIInterp = LAG)} \\
 2 \cdot tStepMes & else  \end{array} \right. \f]
 *
 */
double ConfigSim::tMemNecInterpTDI()
{
	double tMemOtStep(2.0);
	if(TDIInterp == LAG)
		tMemOtStep += ceil(TDIInterpVal/2.0);
	return(tMemOtStep*tStepMes);
}


/*!\brief
 * Checks if there are noises or not.
 *
 * Returns FALSE if all noises in Noises are NULL, else TRUE.
 */
bool ConfigSim::getNoNoise()
{
	bool NNoise(true);
	for(int i=0; i<Noises.size(); i++){
		if(Noises[i] != NULL)
			NNoise = false;
	}
	//NNoise = false ; // always calculate the tau (internal) phasemeters
	return(NNoise);
}


bool ConfigSim::UseInternalPhasemeter()
{
	if(InternalPhasemeters){
		return(true);
	}else{
		if(getNoNoise())
			return(false);
		else
			return(true);
	}
}


int ConfigSim::testbyteorder()
{
	short int word = 0x0001;
	char *byte = (char *) &word;
	int  BIGENDIAN = 0;
	int  LITTLEENDIAN = 1;
	return(byte[0] ? LITTLEENDIAN : BIGENDIAN);
}


char * ConfigSim::uppercase(const char * mot)
{
	int longueur,i ;
	longueur = strlen(mot) ;
	static char MOT1[256] ;
	strcpy(MOT1,mot) ; 
	for (i=0 ; i<longueur ; i++){
		majuscule(MOT1[i]);
	}
	return(MOT1);
}

void ConfigSim::majuscule(char &lettre)
{
	if ((lettre>='a')&&(lettre<='z')) lettre=lettre-32;
}

string ConfigSim::upS(string Str)
{
	int longueur,i;
	string rStr(Str);
	char cS;
	longueur = Str.length();
	for (i=0 ; i<longueur ; i++){
		cS = rStr.c_str()[i];
		majuscule(cS);
		rStr[i] = cS;
	}
	//cout << Str << " ==>> " << rStr << endl;
	return(rStr);
}

// ** Compare lowercase word : Return true if the two words are identical
bool ConfigSim::scmp(const string str1, const string str2)
{
	if(upS(str1) == upS(str2))
		return(true);
	else
		return(false);
}


/*! Makes a copy of the string "orig", stripping all whitespace-type characters;
 * the returned string is a private copy that must be deallocated with free().
 */
char * ConfigSim::stripcopy(const char *orig) 
{
    int pos = 0, len = 0;
    char *ret;
    
    // Strip space-like characters at the beginning
    while(orig[pos] == ' ' || orig[pos] == '\n' || orig[pos] == '\r'|| orig[pos] == '\t')
        pos++;
	
    // Walk until the end of the string  
    while(orig[pos+len] != 0)
        len++;
	
    // Strip space-like characters at the end
    len--;
    while(orig[pos+len] == ' ' || orig[pos+len] == '\n' || orig[pos+len] == '\r' || orig[pos+len] == '\t')
		len--;
    len++;
	
    // Copy the string
    ret = (char*)malloc( (len+1) * sizeof(char) );
    for(int i=0;i<len;i++) 
		ret[i] = orig[pos+i];
    ret[len] = '\0';
    
    return ret;
}


bool ConfigSim::FindTDIName(const char * generatorname, vector<int> & tmp_TDIPacks, vector<double> & tmp_TDIPacksFact)
{
	bool UnKnowTDI(true);
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
		// Packs of X * sqrt(2)/3
		tmp_TDIPacks.push_back(1);
		tmp_TDIPacks.push_back(35);
		tmp_TDIPacks.push_back(364);
		tmp_TDIPacks.push_back(3653);
		tmp_TDIPacks.push_back(-4);
		tmp_TDIPacks.push_back(-53);
		tmp_TDIPacks.push_back(-521);
		tmp_TDIPacks.push_back(-5235);
		tmp_TDIPacksFact.resize(8,sqrt(2.0)/3.0);
		// Packs of Y * sqrt(2)/3
		tmp_TDIPacks.push_back(2);
		tmp_TDIPacks.push_back(16);
		tmp_TDIPacks.push_back(145);
		tmp_TDIPacks.push_back(1461);
		tmp_TDIPacks.push_back(-5);
		tmp_TDIPacks.push_back(-61);
		tmp_TDIPacks.push_back(-632);
		tmp_TDIPacks.push_back(-6316);
		tmp_TDIPacksFact.resize(16,sqrt(2.0)/3.0);
		// Pack of Z * sqrt(2)/3
		tmp_TDIPacks.push_back(3);
		tmp_TDIPacks.push_back(24);
		tmp_TDIPacks.push_back(256);
		tmp_TDIPacks.push_back(2542);
		tmp_TDIPacks.push_back(-6);
		tmp_TDIPacks.push_back(-42);
		tmp_TDIPacks.push_back(-413);
		tmp_TDIPacks.push_back(-4124);
		tmp_TDIPacksFact.resize(24,sqrt(2.0)/3.0);
		if(3>NbMaxDelays)
			NbMaxDelays = 3;
	}
	
	
	return(UnKnowTDI);
}


//end of LISACODE-ConfigSim.cpp

