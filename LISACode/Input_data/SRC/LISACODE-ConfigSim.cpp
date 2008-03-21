/*! \todo Create a variable for the default configuration file name ("ConfigBase").*/

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
 * \arg TDIInterpUtilVal = 20
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
    tMax = 65736.0;
    tStepMes = 1.0;
    tMemNoiseFirst = 5.0;
	tMemNoiseLast = -30.0;
    tMemSig = 100.0;
	tDisplay = 1000.0;
	tDeltaTDIDelay = 0.0;
	TDIInterp = LAG;
	TDIInterpUtilVal = 20;
	Armlength = L0_m_default;
	OrbStartTime = 0.0;
	OrbInitRot = 0.0;
	OrbMove = 1;
	OrbOrder = 2;
	TDIDelayApprox = false;
	LaserPower = LaserPower_W_default;
	PhaMetFilterON = true;
	PhaMetFilterParam.resize(4);
	PhaMetFilterParam[0] = 180.0;
	PhaMetFilterParam[1] = 0.1;
	PhaMetFilterParam[2] = 0.1;
	PhaMetFilterParam[3] = 0.3;
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
       ptr=ConfigFileName+strlen(ConfigFileName);
       //cout << " filename = " << ConfigFileName << endl;
       //cout << strncmp(ptr-4,".xml",4) << endl;
       Endian = testbyteorder();       
       if(Endian == 0) {
           BigEndian = true;
           SystemEncoding = "BigEndian";
       } else {
           BigEndian = false;
           SystemEncoding = "LittleEndian";
       }
       cout << endl << "System Encoding for Binary files is  " <<  getSystemEncoding()<< endl << endl ;
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
	cout << "  Read ASCII configuration file : " << ConfigFileName << endl;
	
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
		if(Buf == "Time"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(Buf == "StepPhysic"){
				ConfigFile >> tStepPhy;
				//cout << " Pas en temps = " << tStepPhy << endl;
			}
			if(Buf == "StepMeasure"){
				ConfigFile >> tStepMes;
				//cout << " Duree d'une mesure = " << tStepMes << endl;
			}
			if(Buf == "Max"){
				ConfigFile >> tMax;
				//cout << " Temps max = " << tMax << endl;
			}
			if(Buf == "NoiseStoreFirst"){
				//ConfigFile >> tMemNoiseFirst;
				//cout << " Temps relatif de debut de memorisation du bruit = " << tMemNoiseFirst << endl;
				ConfigFile >> Buf;
				cout << " ConfigSim::ReadFile : Warning : NoiseStoreFirst is not use in this new version !" << endl;
			}
			if(Buf == "NoiseStoreLast"){
				//ConfigFile >> tMemNoiseLast;
				//cout << " Temps relatif de fin de memorisation du bruit = " << tMemNoiseLast << endl;
				ConfigFile >> Buf;
				cout << " ConfigSim::ReadFile : Warning : NoiseStoreLast is not use in this new version !" << endl;
			}			
			if(Buf == "SignalStore"){
				ConfigFile >> tMemSig;
				//cout << " Duree de memorisation du signal = " << tMemSig << endl;
			}
			if(Buf == "StepDisplay"){
				ConfigFile >> tDisplay;
				//cout << " Pas d'affichage = " << tDisplay<< endl;
			}
			if(Buf == "DeltaTDIDelay"){
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
		if(Buf == "Interpolation"){
			TDIInterpUtilVal = 0.0;
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(Buf == "LAG"){
				ConfigFile >> TDIInterpUtilVal;
				TDIInterp = LAG;
			}
			ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
		}
		/* Informations la precision 
			---------------------------*/
		if(Buf == "TDIDelayApprox"){
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if((Buf == "ON")||(Buf == "On")||(Buf == "on")){
				TDIDelayApprox = true;
			}else{
				if((Buf == "OFF")||(Buf == "Off")||(Buf == "off")){
					TDIDelayApprox = false;
				}else{
					throw invalid_argument("ConfigSim::ReadFile : Bad syntax for approximation of TDI Delay (ON or OFF) !");
				}
			}
			ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
			if(Buf == ";"){
				Readable = true;
			}
		}
		
		
		/* Informations sur les orbites 
			------------------------------*/
		if(Buf == "Orbits"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture de l'element bruite
			if(Buf == "Armlength"){
				ConfigFile >> Armlength;
				ConfigFile >> Buf ;
			}
			if(Buf == "StartTime"){
				ConfigFile >> OrbStartTime;
				ConfigFile >> Buf ;
			}
			if(Buf == "InitialRotation"){
				ConfigFile >> OrbInitRot;
				ConfigFile >> Buf ;
			}
			if(Buf == "Move"){
				ConfigFile >> Buf;
				if((Buf == "ON")||(Buf == "On")||(Buf == "on")){
					OrbMove = 1;
				}else{
					if((Buf == "OFF")||(Buf == "Off")||(Buf == "off")){
						OrbMove = 0;
					}else{
						if(Buf == "MLDC_ON"){
							OrbMove = 1+2;
						}else{
							if(Buf == "MLDC_OFF"){
								OrbMove = 0+2;
							}else{
								throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the orbit move specification (ON,OFF, MLDC_ON or MLDC_OFF) !");
							}
						}
					}
				}
				cout << " OrbMove =  " << OrbMove << endl ;
				cout << " - Approximation of TDI Delay = " << TDIDelayApprox << endl;
				ConfigFile >> Buf ;
			}
			if(Buf == "Order"){
				ConfigFile >> OrbOrder;
				ConfigFile >> Buf ;
			}
			if(Buf == ";"){
				Readable = true;
			}
		}		
		
		/* Informations sur le detecteur 
			------------------------------*/
		if(Buf == "Detector"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture du parametre du detecteur
			if(Buf == "LaserPower"){
				ConfigFile >> LaserPower;
				ConfigFile >> Buf ;
			}
			if(Buf == "PhaMetFilter"){
				ConfigFile >> Buf;
				if((Buf == "ON")||(Buf == "On")||(Buf == "on")){
					PhaMetFilterON = true;
				}else{
					if((Buf == "OFF")||(Buf == "Off")||(Buf == "off")){
						PhaMetFilterON = false;
					}else{
						throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the phasemeter filter specification (ON or OFF) !");
					}
				}
				ConfigFile >> Buf ;
			}
			if(Buf == "PhaMetFilterParameters"){
				ConfigFile >> Buf;  // Lecture de :
				ConfigFile >> Buf;
				do{
					if(Buf == "attenuation"){
						ConfigFile >> PhaMetFilterParam[0];
						//cout << " Frequency = " << freq << endl;
					}
					if(Buf == "oscillation"){
						ConfigFile >> PhaMetFilterParam[1];
						//cout << " h+ = " << hp << endl;
					}
					if(Buf == "FactFmesForHighFreq"){
						ConfigFile >> PhaMetFilterParam[2];
						//cout << " hx = " << hc << endl;
					}
					if(Buf == "FactFmesForLowFreq"){
						ConfigFile >> PhaMetFilterParam[3];
						//cout << " Phi0hp = " << Phi0hp << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}			
			if(Buf == ";"){
				Readable = true;
			}
		}
		
		
		/* Informations sur le Fond d'ondes gravitationnelles 
			-------------------------------------------------*/
		if(Buf == "GWB"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture du parametre du detecteur
			if(Buf == "File"){
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
				cout << " Loading gravitationnal wave background signal from " << FileName << " ... " << endl;
				GWB = new BackgroundGalactic(FileName, Factor);
				cout << " Loading gravitationnal wave background signal --> OK !" << endl;
			}
		}
		
		
		
		/* Informations sur l'enregistrement
			------------------------------------*/
		if(Buf == "Record"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(Buf == "SignalSC"){
				int iSC;
				ConfigFile >> iSC;
				switch (iSC){
				case 1 :
				  ConfigFile >> FileNameSigSC1 >> Buf;
				  FileEncodingSC1 = 0 ;
				  if(Buf == "BINARY")
				    FileEncodingSC1 = 1 ;
				  break;
				case 2 :
				  ConfigFile >> FileNameSigSC2  >> Buf;
				  FileEncodingSC2 = 0 ;
				  if(Buf == "BINARY")
				    FileEncodingSC2 = 1;
				  break;
				case 3 :
				  ConfigFile >> FileNameSigSC3  >> Buf;
				  FileEncodingSC3 = 0 ;
				  if(Buf == "BINARY")
				    FileEncodingSC3 = 1 ;
				  break;
				default :
				  throw invalid_argument("ConfigSim::ReadFile : The spacecraft number must be 1, 2 or 3 !");
				};
			}
			if(Buf == "TDI"){
				ConfigFile >> FileNameTDI >> Buf;
				FileEncodingTDI = 0;
				if(Buf == "BINARY")
					FileEncodingTDI = 1;
				//cout << " Fichier d'enregistrement de TDI = " << FileNameTDI << endl;
			}
			if(Buf == "Delay"){
				ConfigFile >> FileNameDelays >> Buf;
				FileEncodingDelays = 0;
				if(Buf == "BINARY")
					FileEncodingDelays = 1;
				//cout << " Fichier d'enregistrement des retards = " << FileNamedelays << endl;
			}
			if(Buf == "Position"){
				ConfigFile >> FileNamePositions >> Buf;
				FileEncodingPos = 0;
				if(Buf == "BINARY")
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
		if(Buf == "GW"){
			double Beta(0.0), Lambda(0.0), Psi(0.0);
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			// Lecture de Beta, Lambda et Psi
			do{
				if(Buf == "Bet"){
					double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Beta = MathUtils::deg2rad(tmpAng);
					//cout << " Beta = " << Beta << endl;
				}
				if(Buf == "Lam"){double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Lambda = MathUtils::deg2rad(tmpAng);
					//cout << " Lambda = " << Lambda << endl;
				}
				if(Buf == "Psi"){
					double tmpAng(0.0);
					ConfigFile >> tmpAng;
					Psi = MathUtils::deg2rad(tmpAng);
					//cout << " Psi = " << Psi << endl;
				}
				ConfigFile >> Buf;
			}while((Buf != ":")&&(!ConfigFile.eof()));
			// Lecture du type d'onde
			ConfigFile >> Buf;
			// Monochromatic gravitational wave
			if(Buf == "Mono"){
				Readable = true;
				//cout << "OG Monochromatique : " << endl; 
				ConfigFile >> Buf;
				double freq(0.0), hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
				do{
					if(Buf == "f"){
						ConfigFile >> freq;
						//cout << " Frequency = " << freq << endl;
					}
					if(Buf == "hp"){
						ConfigFile >> hp;
						//cout << " h+ = " << hp << endl;
					}
					if(Buf == "hc"){
						ConfigFile >> hc;
						//cout << " hx = " << hc << endl;
					}
					if(Buf == "Phi0hp"){
						ConfigFile >> Phi0hp;
						//cout << " Phi0hp = " << Phi0hp << endl;
					}
					if(Buf == "Phi0hc"){
						ConfigFile >> Phi0hc;
						//cout << " Phi0hc = " << Phi0hc << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				//cout.precision(15);
				//cout << "GWMono   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				//cout.precision(15);
				//cout << " f=" << freq << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
				GWs.push_back(new GWMono(Beta, Lambda, Psi, freq, hp, hc, Phi0hp, Phi0hc));
				NbSrc++;
			}
			// Stochastic gravitational wave
			if(Buf == "Stochastic"){
				Readable = true;
				cout << "============   OG Stochastic : " << endl; 
				ConfigFile >> Buf;
				double hp_sto(0.0), hc_sto(0.0), Slope_sto(0.0), Fmin_sto(0.0), Fknee_sto(0.0);
				// tFirst = tStepPhy ;
				// tDurAdd = tStepPhy ; 
				// Tsample_sto = tStepPhy ;
				do{
					if(Buf == "hp"){
						ConfigFile >> hp_sto;
						//cout << " h+ = " << hp << endl;
					}
					if(Buf == "hc"){
						ConfigFile >> hc_sto;
						//cout << " hx = " << hc << endl;
					}
					if(Buf == "Slope"){
						ConfigFile >> Slope_sto;
						//cout << " Slope_sto = " << Slope_sto << endl;
					}
					if(Buf == "Fmin"){
						ConfigFile >> Fmin_sto;
						//cout << " Fmin_sto = " << Fmin_sto << endl;
					}
					if(Buf == "Fknee"){
						ConfigFile >> Fknee_sto;
						//cout << " Fknee_sto = " << Fknee_sto << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				cout << "  +++ GW Stochastic : Beta,Lambda,hp,hc,Slope_sto,Fknee_sto,Fmin_sto = "<<endl <<NbSrc<<"   "<<MathUtils::rad2deg(Beta)<< "  " <<MathUtils::rad2deg(Lambda)<< "  " << hp_sto<< "  "<< hc_sto<< "  " <<Slope_sto<< "  " <<Fknee_sto<< "  " <<Fmin_sto<< endl ;
				GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));
				NbSrc++;
			}
			if(Buf == "PeriGate"){
				Readable = true;
				//cout << "OG Porte periodique : " << endl; 
				ConfigFile >> Buf;
				double freq(0.0), hp(0.0), hc(0.0) ;
				do{
					if(Buf == "f"){
						ConfigFile >> freq;
						//cout << " Frequency = " << freq << endl;
					}
					if(Buf == "hp"){
						ConfigFile >> hp;
						//cout << " h+ = " << hp << endl;
					}
					if(Buf == "hc"){
						ConfigFile >> hc;
						//cout << " hx = " << hc << endl;
					}
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				GWs.push_back(new GWPeriGate(Beta, Lambda, Psi, freq, hp, hc));
				NbSrc++;
			}			
			// Binary gravitational wave
			if(Buf == "Binary"){ 
				Readable = true;
				//cout << "OG Binaire : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				double M1(0.5), M2(0.033), forb(1./1028.76), inc(1.53921), phi0(0.0), r(0.1); 
				do{
					if(Buf == "M1"){
						ConfigFile >> M1;
						//cout << " Masse 1 = " << M1 << endl;
					}
					if(Buf == "M2"){
						ConfigFile >> M2;
						//cout << " Masse 2 = " << M2 << endl;
					}
					if(Buf == "forb"){
						ConfigFile >> forb;
						//cout << " Frequence orbital = " << forb << endl;
					}
					if(Buf == "inc"){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						inc = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(Buf == "phi0"){
						ConfigFile >> phi0;
						//cout << " Phase initial = " << phi0 << endl;
					}
					if(Buf == "r"){
						ConfigFile >> r;
						//cout << " Distance = " << r << endl;
					}			
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
				//cout.precision(15);
				//cout << "GWBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				//cout << " M1=" << M1 << " M2=" << M2 << " forb=" << forb << " inc=" << inc << " phi0=" << phi0 << " r=" << r << endl;
				GWs.push_back(new GWBinary(Beta, Lambda, Psi, M1, M2, forb, inc, phi0, r));
				NbSrc++;
			}
			// Gravitational wave from a binary system compute at 1 or 2.5 PN
			if(Buf == "PostNewtonBinary"){ 
				Readable = true;
				//cout << "OG Binaire : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				int PNtype(1);
				double M1(1.0e5), M2(1.0e5), tcoal(600000.0), inc(M_PI/2.0), phase(0.0), dist(1.0e6);
				double omega0(0.), taud0(0.), gw(1.);
				do{
					ConfigFile >> Buf;
					if(Buf == "M1"){
						ConfigFile >> M1;
						//cout << " Masse 1 = " << M1 << endl;
					}
					if(Buf == "M2"){
						ConfigFile >> M2;
						//cout << " Masse 2 = " << M2 << endl;
					}
					if(Buf == "tcoal"){
						ConfigFile >> tcoal;
						//cout << " Frequence orbital = " << forb << endl;
					}
					if(Buf == "inc"){
						double tmpAng(0.0);
						ConfigFile >> tmpAng;
						inc = MathUtils::deg2rad(tmpAng);
						//cout << " Inclinaison = " << inc << endl;
					}
					if(Buf == "phase"){
						ConfigFile >> phase;
						//cout << " Phase initial = " << phi0 << endl;
					}
					if(Buf == "r"){
						ConfigFile >> dist;
						//cout << " Distance = " << r << endl;
					}
					if(Buf == "type"){
						ConfigFile >> PNtype;
						//cout << " type = " << PNtype << endl;
						do{
							ConfigFile >> Buf;
							if(Buf == "omega0"){
								ConfigFile >> omega0;
							}
							if(Buf == "taud0"){
								ConfigFile >> taud0;
							}
							if(Buf == "gw"){
								ConfigFile >> gw;
							}
						}while((Buf != ";")&&(!ConfigFile.eof()));
					}
				}while((Buf != ";")&&(!ConfigFile.eof()));
				//cout.precision(15);
				//cout << "GWPostNewtonBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				//cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " inc=" << inc << " phase=" << phase << " dist=" << dist;
				//cout << " type=" << PNtype << " omega0=" << omega0 << " taud0=" << taud0 << " gw=" << gw << endl;
				GWs.push_back(new GWNewton2(Beta, Lambda, Psi, PNtype, M1, M2, tcoal, inc, phase, dist, taud0, omega0, gw));
				NbSrc++;
			}
			// Source dans un fichier
			if(Buf == "File"){
				char FileName[256];
				//cout << "OG Fichier : " << endl; 
				ConfigFile >> Buf; // Lecture de :
				ConfigFile >> FileName;
				//cout << " Nom du fichier contenant l'onde = " << FileName << endl;
				ConfigFile >> Buf; // Verifie si la syntaxe est bonne c-a-d le ;
				if(Buf == ";"){
					Readable = true;
				}
				cout.precision(15);
				//cout << "GWFile   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
				//cout << " File=" << FileName << endl;
				GWs.push_back(new GWFile(Beta, Lambda, Psi, FileName));
				NbSrc++;				
			}
		}
		
		
		
		/* Informations sur les bruits 
			------------------------------*/
		if(Buf == "Noise"){ 
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			int Index(0);
			NoiseSpec tmp_noise;
			// Lecture de l'element bruite
			if(Buf == "Laser"){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir;
			}
			if(Buf == "Shot"){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+6;
			}
			if(Buf == "OOPN"){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+6;
			}
			if(Buf == "Mass"){
				int iSC(0), IndDir(0);
				//cout << " Laser de ";
				ConfigFile >> iSC;
				//cout << " SC " << iSC;
				ConfigFile >> IndDir;
				//cout << " Sens " << IndDir << " : ";
				Index = iSC-1+3*IndDir+12;
			}
			if(Buf == "Bench"){
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
			if(Buf == "White"){
				double SqPSD(0.0);
                //cout << "Bruit blanc ";
                ConfigFile >> SqPSD;
                //cout << " de SqPSD = " << SqPSD << endl;
				//tmp_noise = new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, SqPSD);
				tmp_noise.NType = 1;
				tmp_noise.NVal0=SqPSD;
				ConfigFile >> Buf;
			}
			if(Buf == "File"){
				tmp_noise.NType = 2;
				ConfigFile >> tmp_noise.NStr;
				//cout << "NoiseInFile File Name = " << FileNoiseName << endl;
				//tmp_noise = new NoiseFile(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, FileNoiseName);
				ConfigFile >> Buf;
			}
			if(Buf == "FilterCoef"){
				tmp_noise.NType = 3;
                //cout << "Bruit filtrer ";
                ConfigFile >> Buf; // Lecture de :
				ConfigFile >> Buf; 
				do{
					if(Buf == "alpha"){
						do{
							double tmp_coef(0.0);
							ConfigFile >> tmp_coef;
							(tmp_noise.NVal1).push_back(tmp_coef);
							ConfigFile >> Buf;
						}while(Buf == ",");
					}
					if(Buf == "beta"){
						do{
							double tmp_coef(0.0);
							ConfigFile >> tmp_coef;
							(tmp_noise.NVal2).push_back(tmp_coef);
							//(beta).push_back(tmp_coef*tStepPhy);
							ConfigFile >> Buf;
						}while(Buf == ",");
					}
					if(Buf == "stablization"){
						ConfigFile >> tmp_noise.NVal0;
						ConfigFile >> Buf;
					}
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}
			
			
			if(Buf == "Filter_1of"){
				tmp_noise.NType = 4;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(Buf == "Filter_f"){
				tmp_noise.NType = 5;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(Buf == "Filter_fLosP"){
				tmp_noise.NType = 6;
				ConfigFile >> tmp_noise.NVal0; //Level
				ConfigFile >> Buf;
			}
			if(Buf == "Filter_MLDC_IM"){
				tmp_noise.NType = 7;  // new number
				ConfigFile >> tmp_noise.NVal0; // Level of 1/f
				ConfigFile >> tmp_noise.NVal01; // Frequency at which 1/f becomes 1/f^2
				ConfigFile >> Buf;
			}
			/*
			if(Buf == "Filter_Sto"){
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
		if(Buf == "USO"){
			int iSC(0);
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> Buf;
			if(Buf == "SC")
				ConfigFile >> iSC;
			cout << " Make USO " << iSC << " :" << endl;
			if((iSC<1)||(iSC>3))
				throw invalid_argument("ConfigSim::ReadFile : The spacecraft number must be 1, 2 or 3 !");
			Readable = true;
			ConfigFile >> Buf;
			double USOOffset(0.0), USODerivs(0.0), USONoise(0.0) ;
			do{
				if(Buf == "offset"){
					ConfigFile >> USOOffset;
					//cout << " offset = " << USOOffset << endl;
				}
				if(Buf == "derivs"){
					ConfigFile >> USODerivs;
					//cout << " derivs = " << USODerivs << endl;
				}
				if(Buf == "noise"){
					ConfigFile >> USONoise;
					//cout << " USONoise = " << USONoise << endl;
				}
				ConfigFile >> Buf;
			}while((Buf != ";")&&(!ConfigFile.eof()));
			USOs[iSC-1].init(USOOffset, USODerivs, USONoise);
		}
		
		
		/* Informations about TDI generator
			---------------------------------------*/
		if(Buf == "TDI"){
			char generatorname[256];
			vector<int> tmp_TDIPacks;
			ConfigFile >> Buf; // Lecture de :
			ConfigFile >> generatorname;
			bool UnKnowTDI(true);
			
			// Search if TDI generator is predifined
			UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks);
			
			// Read unknow TDI generator
			if(UnKnowTDI){
				cout << " TDI " << generatorname << " : Read package ... " ;
				do{
					int tmp_pack;
					ConfigFile >> tmp_pack;
					//cout << " Pack " << tmp_pack;
					tmp_TDIPacks.push_back(tmp_pack);
					ConfigFile >> Buf;
					int NbDelays(0);
					while(!((double)(tmp_pack/pow(10.0,NbDelays))<1.0)){
						//cout << "tmp_pack = " << tmp_pack << " et pow(10,NbDelays) = " << pow(10,NbDelays) << endl;
						NbDelays++;
					}
					//cout << "NbMaxDelays = " << NbMaxDelays << endl;
					if(NbDelays-1>NbMaxDelays)
						NbMaxDelays = NbDelays-1;
				}while(Buf == ",");
				cout << " --> OK !" <<  endl;
			}else{
				cout << " TDI " << generatorname << " is predefined generator : Packs isn't readed." << endl;
				do{
					ConfigFile >> Buf;
				}while((Buf != ";")&&(!ConfigFile.eof()));
			}
			if(Buf == ";"){
				Readable = true;
			}
			
			TDIsName.push_back(generatorname);
			TDIsPacks.push_back(tmp_TDIPacks);
			
			
		}
		
		
		/* Read end of configuration's file
			---------------------------------------*/
		if(Buf == "END"){
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
	for(int i=0; i<TDIsName.size(); i++){
		cout << TDIsName[i] << " :";
		for(int j=0; j<TDIsPacks[i].size(); j++)
			cout << " " << (TDIsPacks[i])[j];
		cout << endl;
	}
	cout << endl << " ----------" << endl;
	cout << " Sources number = " << NbSrc << endl;
	cout << " tStepPhy       = " << tStepPhy << " s" << endl;
	cout << " tStepMes       = " << tStepMes << " s" << endl;
	cout << " tDeltaTDIDelay = " << tDeltaTDIDelay << " s" << endl;
	cout << " NbMaxDelays    = " << NbMaxDelays << endl;
	cout << " InterpUtilVal  = " << TDIInterpUtilVal << endl;
	
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
		cout << "  Read XML configuration file : " << ConfigFileName << endl;
		
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
		    cout << " - Type = "<< GenerationType << endl ;
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
			/******************/
			/* Read Simulate Data */
			/******************/
			if(strcmp(type,"Simulate")==0){
				ezxml_t simulate,param;
				//vector<int> tmp_TDIPacks;
				string str_xml;
				double val ;
				int loc_tdi;
				bool UnKnowTDI(true);
				cout << "Section : "<< type  << endl ;
				for(param = ezxml_child(section,"Param"); param; param = param->next){
					if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0){
						TimeOffset = gXMLTime(param);
					}
					if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0){
						tStepMes = gXMLTime(param);
					}
					if(strcmp(ezxml_attr(param,"Name"),"Duration")==0){
						tMax = gXMLTime(param);
					}
					if(strcmp(ezxml_attr(param,"Name"),"Observables")==0){
						TDIParamName = gXMLstring(param) ; // Type ?? TDINameType
						loc_tdi = TDIParamName.find( "Xf,", 0 );
						if(loc_tdi > 0){
						  vector<int> tmp_TDIPacks;
						  //UnKnowTDI = FindTDIName("Xf", tmp_TDIPacks);
						  UnKnowTDI = FindTDIName("Xf", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Xf");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Xf is calculated" <<endl;
						  }
						}
						loc_tdi = TDIParamName.find( "Yf,", 0 );
						if(loc_tdi > 0){
						vector<int> tmp_TDIPacks;
						  UnKnowTDI = FindTDIName("Yf", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Yf");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Yf is calculated" <<endl;
						  }
						}
						loc_tdi = TDIParamName.find( "Zf,", 0 );
						if(loc_tdi > 0){
						  vector<int> tmp_TDIPacks;
						  UnKnowTDI = FindTDIName("Zf", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Zf");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Zf is calculated" <<endl;
						  }
						}
						loc_tdi = TDIParamName.find( "Xf2", 0 );
						if(loc_tdi > 0){
						  vector<int> tmp_TDIPacks;
						  UnKnowTDI = FindTDIName("Xf2", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Xf2");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Xf2 is calculated" <<endl;
						  }
						}
						loc_tdi = TDIParamName.find( "Yf2", 0 );
						if(loc_tdi > 0){
						  vector<int> tmp_TDIPacks;
						  UnKnowTDI = FindTDIName("Yf2", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Yf2");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Yf2 is calculated" <<endl;
						  }
						}
						loc_tdi = TDIParamName.find( "Zf2", 0 );
						if(loc_tdi > 0){
						  vector<int> tmp_TDIPacks;
						  UnKnowTDI = FindTDIName("Zf2", tmp_TDIPacks);
						  if(UnKnowTDI){
						    cout << " --> Sorry, it is not known !" << endl;
						  }else{
						    //cout << "genname ="<<genname<< endl;
						    TDIsName.push_back("Zf2");
						    TDIsPacks.push_back(tmp_TDIPacks);
						  cout << " TDI Zf2 is calculated" <<endl;
						  }
						}
						//throw ;
					}
					if(strcmp(ezxml_attr(param,"Name"),"OutputFile")==0){
					  strcpy(XmlOutputFile,gXMLWord(param)) ;
					}
				}
				//throw invalid_argument("Section : Simulate : Stop") ;
			}
			
			/******************/
			/* Read LISACode Data */
			/******************/
			if(strcmp(type,"LISACode")==0){
			  ezxml_t param;
			  ezxml_t lisacode0;
			  const char * lisacode0type;
			  int Seed_MLDC;
			  cout << "Section : LISACode " << endl ;
			  for(param = ezxml_child(section,"Param"); param; param = param->next){
			    if(strcmp(ezxml_attr(param,"Name"),"GlobalRandomSeed")==0)
			      GlobalRandomSeed = gXMLint(param);
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
			    // Read OutputData  (modif E.P.)
			    //if((strcmp(lisacode0type,"OutputData"))||(strcmp(lisacode0type,"OutputFiles"))==0){
			    if((strcmp(lisacode0type,"OutputData")==0)||(strcmp(lisacode0type,"OutputFiles")==0)){
			      //ezxml_t param;
			      for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
				if(strcmp(ezxml_attr(param,"Name"),"SignalSC1")==0){
				  strcpy(FileNameSigSC1,gXMLWord(param));
				  cout << "   - filename = " << FileNameSigSC1 ;
				  //throw ;
				  FileEncodingSC1 = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingSC1 = 1 ;}
				  cout << "   - Encoding = " << FileEncodingSC1  << endl;
				}
				//}
				if(strcmp(ezxml_attr(param,"Name"),"SignalSC2")==0){
				  strcpy(FileNameSigSC2,gXMLWord(param));
				  cout << "   - filename = " << FileNameSigSC2 ;
				  //throw ;
				  FileEncodingSC2 = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingSC2 = 1;}
				  cout << "   - Encoding = " << FileEncodingSC2  << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"SignalSC3")==0){
				  strcpy(FileNameSigSC3,gXMLWord(param));
				  cout << "   - filename = " << FileNameSigSC3 ;
				  //throw ;
				  FileEncodingSC3 = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingSC3 = 1 ;}
				  cout << "   - Encoding = " << FileEncodingSC3  << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"TDI")==0){
				  strcpy(FileNameTDI,gXMLWord(param));
				  cout << "   - filename = " << FileNameTDI ;
				  //throw ;
				  FileEncodingTDI = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingTDI = 1 ;}
				  cout << "   - Encoding = " <<  FileEncodingTDI << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"Delay")==0){
				  strcpy(FileNameDelays,gXMLWord(param));
				  cout << "   - filename = " << FileNameDelays ;
				  //throw ;
				  FileEncodingDelays = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingDelays = 1 ;}
				  cout << "   - Encoding = " << FileEncodingDelays << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"Position")==0){
				  strcpy(FileNamePositions,gXMLWord(param));
				  cout << "   - filename = " << FileNamePositions ;
				  //throw ;
				  FileEncodingPos = 0;
				  if(strcmp(uppercase(ezxml_attr(param,"Encoding")),"ASCII")!=0){
				    FileEncodingPos = 1 ;}
				  cout << "   - Encoding = " << FileEncodingPos  << endl;
				}
			      }
			    }// End of Read OutputData  (modif E.P.)
			    // Read Detector Data
			    if(strcmp(ezxml_attr(lisacode0,"Type"),"Detector")==0){
			      //ezxml_t param;
			      for(param = ezxml_child(lisacode0,"Param"); param; param = param->next){
				if(strcmp(ezxml_attr(param,"Name"),"LaserPower")==0){
				  LaserPower = atof(ezxml_txt(param));
				  //throw ;
				  if(strcmp(ezxml_attr(param,"Unit"),"Watt")!=0)
				    throw invalid_argument("ConfigSim::ReadXMLFile : The lasers' power unit isn't known (only Watt) !");
				  cout << " - LaserPower = " << LaserPower << " W" << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"PhaMetFilter")==0){										  //if(strcmp(ezxml_txt(param),"On")==0){
				  if(gXMLstring(param) == "On"){
				    PhaMetFilterON = true;
				  }else{ 
				    if(gXMLstring(param) == "Off"){
				      PhaMetFilterON = false;
				    }else{
				      throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the PhaMetFilterON  specification (ON or OFF) !");
				    }
				  }
				  cout << " - Phasemeter filter = " << PhaMetFilterON << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"PMFAttenuation")==0){
				  PhaMetFilterParam[0] = atof(ezxml_txt(param));
				  if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
				    throw invalid_argument("ConfigSim::ReadXMLFile : The attenuation unit isn't known (only dB) !");
				  cout << " - PMFilter : Attenuation = " << PhaMetFilterParam[0] << " dB" << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"PMFOscillation")==0){
				  PhaMetFilterParam[1] = atof(ezxml_txt(param));
				  if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
				    throw invalid_argument("ConfigSim::ReadXMLFile : The oscillation unit isn't known (only dB) !");
				  cout << " - PMFilter : Oscillation = " << PhaMetFilterParam[1] << " dB" << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"PMFLowFreqFact")==0){
				  PhaMetFilterParam[2] = atof(ezxml_txt(param));
				  cout << " - PMFilter : Low Frequency factor of sample frequency = " << PhaMetFilterParam[2] << endl;
				}
				if(strcmp(ezxml_attr(param,"Name"),"PMFHighFreqFact")==0){
				  PhaMetFilterParam[3] = atof(ezxml_txt(param));
				  cout << " - PMFilter : High Frequency factor of sample frequency = " << PhaMetFilterParam[3] << endl;
				}
			      }
			    }
			  }
			  //throw;
			}
			/******************/
			/* Read LISA Data */
			/******************/
			if(strcmp(type,"LISAData")==0){
				ezxml_t lisadata;
				const char * lisadatatype; 
				for (lisadata = ezxml_child(section, "XSIL"); lisadata; lisadata = lisadata->next) {
					lisadatatype = ezxml_attr(lisadata,"Type");
					cout << "LISAData : " << lisadatatype << endl;
					
					/*************** Read LISACode XML File ***************/
					
					// Read Time Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"Time")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
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
						}
					}
					
					// Read Interpolation Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"Interpolation")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"InterpType")==0){
								if(strcmp(ezxml_txt(param),"LAG")){
									TDIInterp = LAG;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"InterpValue")==0){
								TDIInterpUtilVal = atoi(ezxml_txt(param));
							}
						}
						cout << " - Type = " << TDIInterp << endl;
						cout << " - Value = " << TDIInterpUtilVal << endl;
					}
					// Read Precision TDI Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"PrecisionTDI")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"InterpType")==0){
								if(strcmp(ezxml_txt(param),"LAG")){
									TDIInterp = LAG;
								}
							}
							if(strcmp(ezxml_attr(param,"Name"),"InterpValue")==0){
								TDIInterpUtilVal = atoi(ezxml_txt(param));
							}
							if(strcmp(ezxml_attr(param,"Name"),"DelayApprox")==0){
								if(strcmp(ezxml_txt(param),"On")==0){
									TDIDelayApprox = true;
								}else{ 
									if(strcmp(ezxml_txt(param),"Off")==0){
										TDIDelayApprox = false;
									}else{
										throw invalid_argument("ConfigSim::ReadFile : Bad syntax for approximation of TDI Delay (ON or OFF) !");
									}
								}
								cout << " - Approximation of TDI Delay = " << TDIDelayApprox << endl;
							}
						}
						cout << " - Type = " << TDIInterp << endl;
						cout << " - Value = " << TDIInterpUtilVal << endl;
					}
					
					// Read Orbits Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"Orbits")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"Armlength")==0){
								Armlength = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"Meter")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The armlenght's unit isn't known (only Meter) !");
								cout << " - Armlength = " << Armlength << " m" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"StartTime")==0)
								OrbStartTime = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialRotation")==0)
								OrbInitRot = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"Move")==0){								
								if(strcmp(ezxml_txt(param),"On")==0){
									OrbMove = 1;
								}else{ 
									if(strcmp(ezxml_txt(param),"Off")==0){
										OrbMove = 0;
										
									}else{ 
										if(strcmp(ezxml_txt(param),"MLDC_ON")==0){
											OrbMove = 1+2;
											
										}else{ 
											if(strcmp(ezxml_txt(param),"MLDC_OFF")==0){
												OrbMove = 0+2;
												
											}else{
												throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the orbit move specification (ON or OFF) !");
											}
										}
									}
								}
								cout << " - Move = " << OrbMove << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"Order")==0){
								OrbOrder = atoi(ezxml_txt(param));
								cout << " - Order = " << OrbOrder << endl;
							}
						}
					}
					
					// Read Detector Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"Detector")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"LaserPower")==0){
								LaserPower = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"Watt")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The lasers' power unit isn't known (only Watt) !");
								cout << " - LaserPower = " << LaserPower << " W" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PhaMetFilter")==0){								
								if(strcmp(ezxml_txt(param),"On")==0){
									PhaMetFilterON = true;
								}else{ 
									if(strcmp(ezxml_txt(param),"Off")==0){
										PhaMetFilterON = false;
									}else{
										throw invalid_argument("ConfigSim::ReadFile : Bad syntax for the orbit move specification (ON or OFF) !");
									}
								}
								cout << " - Phasemeter filter = " << PhaMetFilterON << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFAttenuation")==0){
								PhaMetFilterParam[0] = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The attenuation unit isn't known (only dB) !");
								cout << " - PMFilter : Attenuation = " << PhaMetFilterParam[0] << " dB" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFOscillation")==0){
								PhaMetFilterParam[1] = atof(ezxml_txt(param));
								if(strcmp(ezxml_attr(param,"Unit"),"dB")!=0)
									throw invalid_argument("ConfigSim::ReadXMLFile : The oscillation unit isn't known (only dB) !");
								cout << " - PMFilter : Oscillation = " << PhaMetFilterParam[1] << " dB" << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFLowFreqFact")==0){
								PhaMetFilterParam[2] = atof(ezxml_txt(param));
								cout << " - PMFilter : Low Frequency factor of sample frequency = " << PhaMetFilterParam[2] << endl;
							}
							if(strcmp(ezxml_attr(param,"Name"),"PMFHighFreqFact")==0){
								PhaMetFilterParam[3] = atof(ezxml_txt(param));
								cout << " - PMFilter : High Frequency factor of sample frequency = " << PhaMetFilterParam[3] << endl;
							}
						}
					}
					
					// Read USO Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"USO")==0){
						ezxml_t param;
						int iSC;
						double USOOffset(0.0), USODerivs(0.0), USONoise(0.0) ;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SCNumber")==0){
								bool UnKnowSCNumber(true);
								if(strcmp(ezxml_txt(param),"SC1")==0){
									iSC = 1;
									UnKnowSCNumber = false;
								}
								if(strcmp(ezxml_txt(param),"SC2")==0){
									iSC = 1;
									UnKnowSCNumber = false;
								}
								if(strcmp(ezxml_txt(param),"SC3")==0){
									iSC = 1;
									UnKnowSCNumber = false;
								}
								if(UnKnowSCNumber)
									throw invalid_argument("ConfigSim::ReadXMLFile : The name type isn't known (only SignalSC1, SignalSC2, SignalSC3, Delay, Position or TDI) !");
								cout << " - USO SC Number = " << iSC << endl;
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
								cout << " - Noise = " <<USONoise << endl;
							}
						}
						if((iSC<1)||(iSC>3))
							throw invalid_argument("ConfigSim::ReadFile : Bad USO's spacecraft number (only 1, 2 or 3) !");
						USOs[iSC-1].init(USOOffset, USODerivs, USONoise);
						cout << endl;
					}
					
					// Read Record Data
					if(strcmp(ezxml_attr(lisadata,"Type"),"Records")==0){
						ezxml_t param, series;
						const char *recordname;
						int typedata, encoding;
						char * filename;
						int length, records;
						int iSC(0);
						bool UnKnowRecords(true);
						// Read the filename
						series = ezxml_child(lisadata,"XSIL");
						if(strcmp(ezxml_attr(series,"Type"),"TimeSeries")!=0)
							throw invalid_argument("ConfigSim::ReadXMLFile : The record type isn't known (only TimeSeries) !");
						filename = gXMLTimeSeries(series,typedata,encoding,length,records);
						// Find the name of records
						if(strcmp(ezxml_attr(lisadata,"Name"),"SignalSC1")==0){
							strcpy(FileNameSigSC1,filename);
							cout << " - FileNameSigSC1 = " << FileNameSigSC1 ;
							FileEncodingSC1 = encoding ;
							cout << "   Encoding =" << encoding << endl;
							UnKnowRecords = false;
						}
						if(strcmp(ezxml_attr(lisadata,"Name"),"SignalSC2")==0){
						  strcpy(FileNameSigSC2,filename); ;
						  cout << " - FileNameSigSC2 = " << FileNameSigSC2 ;
							FileEncodingSC2 = encoding ;
							cout << "   Encoding =" << encoding << endl;
							UnKnowRecords = false;
						}
						if(strcmp(ezxml_attr(lisadata,"Name"),"SignalSC3")==0){
							strcpy(FileNameSigSC3,filename);
							cout << " - FileNameSigSC3 = " << FileNameSigSC3 ;
							FileEncodingSC3 = encoding ;
							cout << "   Encoding =" << encoding << endl;
							UnKnowRecords = false;
						}
						if(strcmp(ezxml_attr(lisadata,"Name"),"Delay")==0){
							strcpy(FileNameDelays,filename);
							cout << " - Delay = " << FileNameDelays ;
							FileEncodingDelays= encoding ;
							cout << "   Encoding =" << encoding << endl;
							UnKnowRecords = false;
						}
						if(strcmp(ezxml_attr(lisadata,"Name"),"Position")==0){
							strcpy(FileNamePositions,filename);
							cout << " - Position = " << FileNamePositions ;
							FileEncodingPos = encoding ;
							cout << "   Encoding =" << encoding << endl;
							UnKnowRecords = false;
						}
						if(strcmp(ezxml_attr(lisadata,"Name"),"TDI")==0){
							strcpy(FileNameTDI,filename);
							cout << " - TDI = " << FileNameTDI ;
							FileEncodingTDI = encoding ;
							cout << "   Encoding =" <<encoding << endl;
							UnKnowRecords = false;
						}
						if(UnKnowRecords)
							throw invalid_argument("ConfigSim::ReadXMLFile : The name type isn't known (only SignalSC1, SignalSC2, SignalSC3, Delay, Position or TDI) !");
					}
					
					
					
					
					/***************  Read Mock LISA Data Challenge XML File ***************/
					
					if(strcmp(ezxml_attr(lisadata,"Type"),"PseudoLISA")==0){
						ezxml_t param;
						for(param = ezxml_child(lisadata,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"InitialRotation")==0)
							  //cout << " MLDC PeusoLisa Orbits" << endl ;
								OrbInitRot = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialPosition")==0){
								double tmpInitPos(0.0);
								tmpInitPos = gXMLAngle(param);
								OrbStartTime = tmpInitPos*Yr_SI/(2.0*M_PI);
								OrbMove = 3 ;  // MLDC
								OrbOrder = 2;
								//OrbMove = 1 ;  // a verifier
								//OrbOrder = 2;  // a verifier
								//cout << "OrbMove " << OrbMove << endl ;
								//throw ;
							}
							if(strcmp(ezxml_attr(param,"Name"),"Armlength")==0){
								if(strcmp(ezxml_attr(param,"Unit"),"Meter")==0){
									Armlength = atof(ezxml_txt(param));
								}else{
									double tmpArmTime(0.0);
									tmpArmTime = gXMLTime(param);
									Armlength = tmpArmTime*c_SI;
								}
								cout << " - Armlength = " << Armlength << " m" << endl;
							}
						}
					}
					
				}  // end for
			} // end if
			/* End of read LISA Data */			
			
			
			/********************/
			/* Read Source Data */
			/********************/
			if(strcmp(type,"SourceData")==0){
				ezxml_t source;
				const char *name;
				double Beta, Lambda, Psi;
				cout << "Section : SourceData" << endl ;
				for (source = ezxml_child(section, "XSIL"); source; source = source->next) {
					name = ezxml_attr(source,"Name");
					cout << endl << "Source : Name = " << name << endl;
					
					/*************** Read LISACode XML File ***************/
					// pourquoi lire cela ici : modif_eric .. a deplacer ?
					ezxml_t param;
					for(param = ezxml_child(source,"Param"); param; param = param->next){
						if(strcmp(ezxml_attr(param,"Name"),"EclipticLatitude")==0)
							Beta = gXMLAngle(param);
						if(strcmp(ezxml_attr(param,"Name"),"EclipticLongitude")==0)
							Lambda = gXMLAngle(param);
						if(strcmp(ezxml_attr(param,"Name"),"Polarization")==0)
							Psi = gXMLAngle(param);
					}
					cout << " Source : type = " << ezxml_attr(source,"Type")<< endl ;
					//throw invalid_argument(" eric 999 ");
					// Read Parameters for Mono
					if(strcmp(ezxml_attr(source,"Type"),"Mono")==0){
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
						cout << "GWMono   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
						cout.precision(15);
						cout << " f=" << f << " hp=" << hp << " hc=" << hc << " Phi0hp=" << Phi0hp << " Phi0hc=" << Phi0hc << endl;
						GWs.push_back(new GWMono(Beta, Lambda, Psi, f, hp, hc, Phi0hp, Phi0hc));
						NbSrc++;
					}	
					// Read Parameters for Binary
					if(strcmp(ezxml_attr(source,"Type"),"Binary")==0){
						double M1, M2, forb, inc, phi0, r;
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
								M1 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
								M2 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"OrbitalFrequency")==0)
								forb = gXMLFrequency(param);
							if(strcmp(ezxml_attr(param,"Name"),"Inclination")==0)
								inc = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialPhase")==0)
								phi0 = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
								r = gXMLAstroDistance(param);
						}
						cout.precision(15);
						cout << "GWBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
						cout << " M1=" << M1 << " M2=" << M2 << " forb=" << forb << " inc=" << inc << " phi0=" << phi0 << " r=" << r << endl;
						GWs.push_back(new GWBinary(Beta, Lambda, Psi, M1, M2, forb, inc, phi0, r));
						NbSrc++;
					}
					// Read Parameters for PostNewtonBinary
					if(strcmp(ezxml_attr(source,"Type"),"PostNewtonBinary")==0){
						int PNtype;
						double M1, M2, tcoal, inc, phase, dist, omega0(0.0), taud0(0.0), gw(1.0);
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
								M1 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
								M2 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"CoalescenceTime")==0)
								tcoal = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"Inclination")==0)
								inc = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"CoalescencePhase")==0)
								phase = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
								dist = gXMLAstroDistance(param);
							if(strcmp(ezxml_attr(param,"Name"),"Type")==0)
								PNtype = atoi(ezxml_txt(param));
							if(strcmp(ezxml_attr(param,"Name"),"Omega0")==0)
								omega0 = atof(ezxml_txt(param));
							if(strcmp(ezxml_attr(param,"Name"),"Taud0")==0)
								taud0 = atof(ezxml_txt(param));
							if(strcmp(ezxml_attr(param,"Name"),"gw")==0)
								gw = atof(ezxml_txt(param));
						}
						cout.precision(15);
						cout << "GWPostNewtonBinary : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
						cout << " M1=" << M1 << " M2=" << M2 << " tcoal=" << tcoal << " inc=" << inc << " phase=" << phase << " dist=" << dist;
						cout << " type=" << PNtype << " omega0=" << omega0 << " taud0=" << taud0 << " gw=" << gw << endl;
						GWs.push_back(new GWNewton2(Beta, Lambda, Psi, PNtype, M1, M2, tcoal, inc, phase, dist, taud0, omega0, gw));
						NbSrc++;
						
					}
					// Read Parameters for "File" GW
					if(strcmp(ezxml_attr(source,"Type"),"File")==0){
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
						cout << "GWFile   : Beta=" << Beta << " Lambda=" << Lambda << " Psi=" << Psi;
						cout << " File=" << filename << endl;
						// WARNING : Impossible de reccuperer le nom de fichier qui devrait etre dans filename
						GWs.push_back(new GWFile(Beta, Lambda, Psi, filename));
						NbSrc++;
					}	
					
					
					/***************  Read Mock LISA Data Challenge XML File ***************/
					// Adapted for stochastic background MLDC round 3
					// Read direction and polarisation of GW
					string SourceType_s ;
					SourceType_s = "unknown" ;
					if((strcmp(ezxml_attr(source,"Type"),"PlaneWave")==0)||(strcmp(ezxml_attr(source,"Type"),"SampledPlaneWave")==0)){
						ezxml_t param;
						double val ;
						double hp(0.0), hc(0.0), Phi0hp(0.0), Phi0hc(0.0);
						double hp_sto(0.0), hc_sto(0.0), Slope_sto(0.0), Fmin_sto(0.0), Fknee_sto(0.0);
						double tDurAdd(0.0),  tFirst(40.0),  tLast(-100.0),  tder(-1000.0), Tsample_sto(0.0);
						int Nb_Ageing(100000);
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SourceType")==0){
								SourceType_s = gXMLstring(param);
							}
							// Read Parameters for Stochastic GW background
							if(SourceType_s == "Stochastic"){
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
						}  // All parameters are read
						tDurAdd = tStepPhy ; 
						Tsample_sto = tStepPhy ;
						Beta = M_PI/2.-Beta ;  //  a verifier modif_eric
						cout << "creating stochastic" <<endl ;
						cout << " GW Stochastic : nb_src,Beta,Lambda,hp,hc,tStepMes,tDurAdd,tFirst,tLast,tder,Slope_sto,Fknee_sto,Fmin_sto = "<<endl <<NbSrc<<"   "<<MathUtils::rad2deg(Beta)<< "  " <<MathUtils::rad2deg(Lambda)<< "  " << hp_sto<< "  "<< hc_sto<< "  " <<tStepPhy<< "  " <<tDurAdd<< "  " <<tFirst<< "  " <<tLast<< "  " <<tder<< "  " <<Slope_sto<< "  " <<Fknee_sto<< "  " <<Fmin_sto<< endl ;
						//GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));
						GWs.push_back(new GWSto(Beta,Lambda,tStepMes,Slope_sto,Fknee_sto,Fmin_sto,hp_sto,hc_sto));						//GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,tDurAdd,tFirst,tLast,tder,Slope_sto,Tsample_sto,Fknee_sto,Fmin_sto,Nb_Ageing,hp_sto,hc_sto));
						NbSrc++;
						//throw ; 
					}
					// Read specific parameters for plane wave
					if((strcmp(ezxml_attr(source,"Type"),"PlaneWave")==0) && (SourceType_s == "unknown")){
						ezxml_t param;
						char * SourceType;
						double Amplitude(0.0), Inclination(0.0), Frequency(0.0), Initialphase(0.0);
						double M1(0.0), M2(0.0), Distance(0.0), CoalTime(0.0);
						double ci;
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"SourceType")==0){
								//SourceType = ezxml_txt(param);
								SourceType = gXMLWord(param);
							}
							if(strcmp(ezxml_attr(param,"Name"),"Amplitude")==0){
								Amplitude = atof(ezxml_txt(param))*atof(ezxml_attr(param,"Unit"));
							}
							if(strcmp(ezxml_attr(param,"Name"),"Inclination")==0)
								Inclination = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"Frequency")==0)
								Frequency = gXMLFrequency(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialPhase")==0)
								Initialphase = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"InitialAngularOrbitalPhase")==0)
								Initialphase = gXMLAngle(param);
							if(strcmp(ezxml_attr(param,"Name"),"Mass1")==0)
								M1 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"Mass2")==0)
								M2 = gXMLAstroMass(param);
							if(strcmp(ezxml_attr(param,"Name"),"Distance")==0)
								Distance = gXMLAstroDistance(param);
							if(strcmp(ezxml_attr(param,"Name"),"CoalescenceTime")==0)
								CoalTime = gXMLTime(param);
						}
						cout << " New Compute GW (PlaneWave) : " << SourceType << endl;
						cout << "   - Beta         = " << Beta << " rad" << endl;
						cout << "   - Lambda       = " << Lambda << " rad" << endl;
						cout << "   - Psi          = " << Psi << " rad" << endl;
						cout << "   - Amplitude    = " << Amplitude  << endl;
						cout << "   - Inclination  = " << Inclination << " rad" << endl;
						cout << "   - Frequency    = " << Frequency << " Hz" << endl;
						cout << "   - InitialPhase = " << Initialphase << " rad" << endl;
						cout << "   - Mass 1       = " << M1 << " MS" << endl;
						cout << "   - Mass 2       = " << M2 << " MS" << endl;
						cout << "   - Distance     = " << Distance << " pc" << endl;
						cout << "   - Coal. time   = " << CoalTime << " s" << endl;
						
						if(strcmp(SourceType,"GalacticBinary")==0){
							ci = cos(Inclination);
							GWs.push_back(new GWMono(Beta, Lambda, Psi, Frequency, Amplitude*(1.0+ci*ci), 2*Amplitude*ci, Initialphase+3.0*M_PI/2.0, Initialphase));
							NbSrc++;
						}else{ 
							if(strcmp(SourceType,"BlackHoleBinary")==0){
								GWs.push_back(new GWNewton2(Beta, Lambda, Psi, 2, M1, M2, CoalTime, Inclination, Initialphase, Distance, 1.0, 10.0, 1.0));
								NbSrc++;
							}else{
								if(SourceType_s == "Stochastic"){
									cout << "creating stochastic" << endl ;
									//GWs.push_back(new GWSto(Beta,Lambda,tStepPhy,tDurAdd,tFirst,tLast,tder,Slope_sto,Tsample_sto,Fknee_sto,Fmin_sto,Nb_Ageing,hp_sto,hc_sto));
									NbSrc++;
								}else{
									if(strcmp(SourceType,"ExtremeMassRatioInspiral")==0){
										cout << "  --> Sorry ! The EMRI are not computde actually by LISACode ! This work is in progress !" << endl;
									}else{
										throw invalid_argument("ConfigSim::ReadXMLFile : The gravitational wave SourceType isn't known (only GalacticBinary or BlackHoleBinary) !");
									}
								}
							}
						}
					}
					// Read specific parameters for sampled plane wave (File with just hp and hc)
					if((strcmp(ezxml_attr(source,"Type"),"SampledPlaneWave")==0)||(strcmp(ezxml_attr(source,"Name"),"hp,hc")==0)){
						ezxml_t param;
						const char * SourceType;
						double TimeOffset(0.0), Cadence(0.0), Duration(0.0); 
						//double ci;
						for(param = ezxml_child(source,"Param"); param; param = param->next){
							if(strcmp(ezxml_attr(param,"Name"),"TimeOffset")==0)
								TimeOffset = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"Cadence")==0)
								Cadence = gXMLTime(param);
							if(strcmp(ezxml_attr(param,"Name"),"Duration")==0)
								Duration = gXMLTime(param);
						}
						cout << " New Read GW (SampledPlaneWave) : " << SourceType << endl;
						cout << "   - Beta         = " << Beta << " rad" << endl;
						cout << "   - Lambda       = " << Lambda << " rad" << endl;
						cout << "   - Psi          = " << Psi << " rad" << endl;
						cout << "   - TimeOffset   = " << TimeOffset  << " s" << endl;
						cout << "   - Cadence      = " << Cadence  << " s" << endl;
						cout << "   - Duration     = " << Duration  << " s" << endl;
					}
					// Read specific parameters for plane wave table ???
					if(strcmp(ezxml_attr(source,"Type"),"PlaneWaveTable")==0){
						cout << "  --> Sorry ! The GW type PlaneWaveTable is not included actually by LISACode ! This work is in progress !" << endl;
					}
				}
			}
			
			
			/*******************/
			/* Read Noise Data */
			/*******************/
			if(strcmp(type,"NoiseData")==0){
				ezxml_t noisedata; 
				double val ;
				for (noisedata = ezxml_child(section, "XSIL"); noisedata; noisedata = noisedata->next) {
					ezxml_t param;
					const char * instrument;
					char * localise;
					string str_xml,instrument_eric,SourceType,SpectralType;
					//eric_nul = " \n";
					int SCem, iSC(-1), IndDir(-1), InstrumentIndex ;
					double PSD ;
					// Noise localisation in LISA : instrument and spacecraft
					instrument = ezxml_attr(noisedata,"Name");
					cout << endl << "Noise : Instrument = " << instrument << endl;
					for(param = ezxml_child(noisedata,"Param"); param; param = param->next){
						if(strcmp(ezxml_attr(param,"Name"),"PowerSpectralDensity")==0){
							PSD = gXMLdouble(param);
						}else{
							if(strcmp(ezxml_attr(param,"Name"),"Fknee")==0){
								val = gXMLdouble(param);
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
					} // all parameters are read
					if(SpectralType == "PinkAcceleration"){
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
					if(SpectralType == "WhitePhase"){
						//cout << "instrument 21 :" << instrument << endl ;
						InstrumentIndex = 6; // (shot noise) a verifier
						if(strcmp(instrument,"pd1")==0){
							iSC = 1 ; IndDir = 0 ;
							cout << "case pd1 "<< endl ;
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
					if(SpectralType == "WhiteFrequency"){
						//cout << "instrument 31 :" << instrument << endl ;
						InstrumentIndex = 0; // (laser) a verifier
						if(strcmp(instrument,"c1")==0){
							iSC = 1 ; IndDir = 0 ;
							cout << "case c1 "<< endl ;
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
					//throw ;
					//cout << " - Instrument = " << instrument << endl;
					string nom_eric[] = {"pm1", "pm2", "pm3", "pm1s", "pm2s", "pm3s","pd1", "pd2", "pd3", "pd1s", "pd2s", "pd3s","c1", "c2", "c3", "c1s", "c2s", "c3s" } ;
					int test_eric ;
					instrument_eric = instrument ;
					//instrument_eric = instrument_eric.substr(0,2);
					test_eric=1 ;
					for ( int j=0 ; j<18 ; j++){
						test_eric=test_eric*instrument_eric.compare(nom_eric[j]) ;
						//cout << "<" << instrument_eric<< "<" << nom_eric[j] << "<" << test_eric <<endl ;
					}
					if(test_eric != 0)
						throw invalid_argument("ConfigSim::ReadXMLFile : The instrument for noise isn't known  !");
					
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
						// modif e.p.
						if(strcmp(noisetype,"Filter_MLDC_IM")==0){
							tmp_noise.NType = 7;
							tmp_noise.NVal0=level;
							cout << "  --> Filter_MLDC_IM : " << level << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
							throw invalid_argument("ConfigSim::ReadXMLFile : One parameter is missing in xml file for Filter_MLDC_IM !");	
						}	
						// end of modif e.p.
						// modif e.p.
						if(SpectralType == "PinkAcceleration"){
							tmp_noise.NType = 7;
							tmp_noise.NVal0=pow(PSD,0.5);
							tmp_noise.NVal01=1e-4; // frequency of ankle
												   //tmp_noise.NVal01=1e-8; // frequency of ankle
							cout << "  --> PinkAcceleration : ISC, IndDir, Level = " << iSC << " " << IndDir << "  " <<tmp_noise.NVal0 << endl;
							NoisePlace(tmp_noise, iSC, IndDir, InstrumentIndex);
							//throw invalid_argument("ConfigSim::ReadXMLFile : One parameter is missing in xml file for Filter_MLDC_IM !");		
						}
						// end of modif e.p.
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
				cout << endl ;
			}
			
			/*******************/
			/* Read TDI Data */
			/*******************/
			if(strcmp(type,"TDIData")==0){
				ezxml_t tdidata;
				for (tdidata = ezxml_child(section, "XSIL"); tdidata; tdidata = tdidata->next) {
					const char * generatorname;
					vector<int> tmp_TDIPacks;
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
						UnKnowTDI = FindTDIName(generatorname, tmp_TDIPacks);
						
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
									while(!((double)(tmp_pack/pow(10.0,NbDelays))<1.0)){
										NbDelays++;
									}
								}
							}
							cout << endl;
						}
						TDIsName.push_back(generatorname);
						TDIsPacks.push_back(tmp_TDIPacks);
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
						/*
						  const char *recordname;
						int typedata, encoding;
						const char *genl;
						char * filename;
						int length, records;
						double timeoffset(0.0), cadence(0.0), duration(0.0);
						*/
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
							UnKnowTDI = FindTDIName(genname, tmp_TDIPacks);
							if(UnKnowTDI){
								cout << " --> Sorry, it is not known !" << endl;
							}else{
							  //cout << "genname ="<<genname<< endl;
								TDIsName.push_back(genname);
								TDIsPacks.push_back(tmp_TDIPacks);
								cout << " --> OK !" << endl;
							}
							
						}while(genl[iptr2]==',');
						
						// Read the filename
						cout << "TDI filename ..." << endl;
						series = ezxml_child(tdidata,"XSIL");
						cout << "series:"<<series<<endl ;
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

		}
		cout << endl << "   ==================  Create XML Outputfile here" << endl << endl;
		if(XmlOutputFile != "None"){
			CreateXmlOutputFile();
			//throw ;
		}
		ezxml_free(tree);
		
		// Creation of tMemNoiseFirst and tMemNoiseLast
		double OrderLagrangeNoise(7);
		tMemNoiseFirst = tStepPhy*ceil(2+OrderLagrangeNoise/2.0);
		tMemNoiseLast = -1.0*(tMaxDelay()+tStepPhy*ceil(2+OrderLagrangeNoise/2.0));
		
		// Noises creation
		NoisesCreation();
		
		// Display of TDI generators
		
		for(int i=0; i<TDIsName.size(); i++){
			cout << TDIsName[i] << " :";
			for(int j=0; j<TDIsPacks[i].size(); j++)
				cout << " " << (TDIsPacks[i])[j];
			cout << endl;
		}
		
		cout << endl << " ----------" << endl;
		cout << " Sources number = " << NbSrc << endl;
		cout << " tStepPhy       = " << tStepPhy << " s" << endl;
		cout << " tStepMes       = " << tStepMes << " s" << endl;
		cout << " tDeltaTDIDelay = " << tDeltaTDIDelay << " s" << endl;
		cout << " NbMaxDelays    = " << NbMaxDelays << endl;
		cout << " InterpUtilVal  = " << TDIInterpUtilVal << endl;
		
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
	string spc_5("     "),spc_9("         "),spc_3("   ");
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
	
	FichXML << spc_3 <<ParamName<<"Author\" >" << endl ;
	FichXML << spc_9 << getAuthor() << endl << spc_3 << Param_end ;
	FichXML << spc_3 <<ParamName<<"GenerationDate\" Type =\"" << getGenerationType() << "\"> "<< endl ;
	FichXML << spc_9 << getGenerationDate()  << endl << spc_3 << Param_end ;
	FichXML << spc_3 <<ParamName<<"Simulator\">" << endl ;
	FichXML << spc_9 << getSimulator() <<endl << spc_3 << Param_end ;
	
	// TDI data
	FichXML <<"<XSIL Type=\"TDIData\">"<<endl;
	
	FichXML <<"   <XSIL Name=\""<<getTDIParamName()<<"\" Type=\"TDIObservable\">"<< endl;
	FichXML << spc_5 <<ParamName <<"DataType\">"<<endl <<spc_5<<"FractionalFrequency"<< endl<<spc_5 << Param_end;
	FichXML <<"   <XSIL Name=\""<<getTDIParamName()<<"\" Type=\"TimeSeries\">"<< endl;
	FichXML << spc_5 <<ParamName<<"TimeOffset\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << getTimeOffset() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Cadence\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << gettStepMes() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Duration\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << int(gettMax()) << endl << spc_5 <<Param_end;
	FichXML << spc_5 << ArrayName << getTDIParamName()<<"\" Type=\"double\" Unit=\"Word\">" << endl;
	FichXML << spc_5 << DimName <<"Length\">"<<endl <<spc_9<< int(gettMax()/gettStepMes()+1)<<endl<<spc_5<<Dim_end;
	FichXML << spc_5 << DimName <<"Records\">"<<endl <<spc_9<< "4"<<endl<<spc_5 <<Dim_end;
	if(getFileEncodingTDI()==0)
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"ASCII\">" <<endl;
	else{
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"Binary,"<<getSystemEncoding()<<"\">" <<endl;}
	FichXML << spc_9 << getFileNameTDI() << endl <<spc_5<<Stream_end;
	FichXML << spc_5 << Array_end;
	FichXML << spc_3 << xsil_end;
	FichXML << xsil_end;
	
	// SC1 Phasemeter data
	ObsName="t,y231,y321,z231,z321";
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TDIObservable\">"<< endl;
	FichXML << spc_5 <<ParamName <<"DataType\">"<<endl <<spc_5<<"TimeSeries"<<endl<< spc_5 << Param_end;
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TimeSeries\">"<< endl;
	FichXML << spc_5 <<ParamName<<"TimeOffset\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << getTimeOffset() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Cadence\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << gettStepMes() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Duration\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << int(gettMax()) << endl << spc_5 <<Param_end;
	FichXML << spc_5 << ArrayName << ObsName<<"\" Type=\"double\" Unit=\"Word\">" << endl;
	FichXML << spc_5 << DimName <<"Length\">"<<endl <<spc_9<< int(gettMax()/gettStepMes()+1)<<endl<<spc_5<<Dim_end;
	FichXML << spc_5 << DimName <<"Records\">"<<endl <<spc_9<< "5"<<endl<<spc_5 <<Dim_end;
	if(getFileEncodingSig(1)==0)
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"ASCII\">" <<endl;
	else{
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"Binary,"<<getSystemEncoding()<<"\">" <<endl;}
	FichXML << spc_9 << getFileNameSigSC1() << endl <<spc_5<<Stream_end;
	FichXML << spc_5 << Array_end;
	FichXML << spc_3 << xsil_end;
	FichXML << xsil_end;
	
	// SC2 Phasemeter data
	ObsName="t,y312,y132,z312,z132";
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TDIObservable\">"<< endl;
	FichXML << spc_5 <<ParamName <<"DataType\">"<<endl <<spc_5<<"TimeSeries"<< spc_5<<endl<<Param_end;
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TimeSeries\">"<< endl;
	FichXML << spc_5 <<ParamName<<"TimeOffset\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << getTimeOffset() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Cadence\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << gettStepMes() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Duration\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << int(gettMax()) << endl << spc_5 <<Param_end;
	FichXML << spc_5 << ArrayName << ObsName<<"\" Type=\"double\" Unit=\"Word\">" << endl;
	FichXML << spc_5 << DimName <<"Length\">"<<endl <<spc_9<< int(gettMax()/gettStepMes()+1)<<endl<<spc_5<<Dim_end;
	FichXML << spc_5 << DimName <<"Records\">"<<endl <<spc_9<< "5"<<endl<<spc_5 <<Dim_end;
	if(getFileEncodingSig(2)==0)
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"ASCII\">" <<endl;
	else{
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"Binary,"<<getSystemEncoding()<<"\">" <<endl;}
	FichXML << spc_9 << getFileNameSigSC2() << endl <<spc_5<<Stream_end;
	FichXML << spc_5 << Array_end;
	FichXML << spc_3 << xsil_end;
	FichXML << xsil_end;
	
	// SC3 Phasemeter data
	ObsName="t,y123,y213,z123,z213";
	
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TDIObservable\">"<< endl;
	FichXML << spc_5 <<ParamName <<"DataType\">"<<endl <<spc_5<<"TimeSeries"<< spc_5<<endl<<Param_end;
	FichXML <<"   <XSIL Name=\""<<ObsName<<"\" Type=\"TimeSeries\">"<< endl;
	FichXML << spc_5 <<ParamName<<"TimeOffset\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << getTimeOffset() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Cadence\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << gettStepMes() << endl << spc_5 <<Param_end;
	FichXML << spc_5 <<ParamName<<"Duration\" Unit=\"Second\">" << endl ;
	FichXML << spc_9 << int(gettMax()) << endl << spc_5 <<Param_end;
	FichXML << spc_5 << ArrayName << ObsName<<"\" Type=\"double\" Unit=\"Word\">" << endl;
	FichXML << spc_5 << DimName <<"Length\">"<<endl <<spc_9<< int(gettMax()/gettStepMes()+1)<<endl<<spc_5<<Dim_end;
	FichXML << spc_5 << DimName <<"Records\">"<<endl <<spc_9<< "5"<<endl<<spc_5 <<Dim_end;
	if(getFileEncodingSig(3)==0)
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"ASCII\">" <<endl;
	else{
	  FichXML << spc_5 << StreamType<<"Remote\" Encoding=\"Binary,"<<getSystemEncoding()<<"\">" <<endl;}
	FichXML << spc_9 << getFileNameSigSC3() << endl <<spc_5<<Stream_end;
	FichXML << spc_5 << Array_end;
	FichXML << spc_3 << xsil_end;
	FichXML << xsil_end;
	
	
	FichXML << xsil_end;
	FichXML << xsil_end;
	FichXML.close();
	//throw;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << TimeParam << " Second" << endl;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << AngleParam << " Radian" << endl;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << FrequencyParam << " Hertz" << endl;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << AstroMass << " SolarMass" << endl;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << AstroDistance << " KiloParsec" << endl;
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

	cout << " - Word = " << theWord << endl;
	
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << result << endl;
	return (result);
}


/*!\brief
* Extracts a string from XML structure.
*
* NO units
*/
string  ConfigSim::gXMLstring(ezxml_t param)
{
	string result,str_find,str_nul,str_end ;
	str_nul = " \n";
	str_end = "\n";
	str_find = (*param).txt ;
	str_find = str_find.substr(str_find.find_first_not_of(str_nul)) ;
	str_find = str_find.substr(0,str_find.find_first_of(str_end)) ;
	result = str_find;
	cout << " - " << ezxml_attr(param,"Name") << " = " << result << endl;
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
	cout << " - " << ezxml_attr(param,"Name") << " = " << result << endl;
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
	typedata = -1;
	/*
	if(strcmp(ezxml_attr(stream,"Type"),"XXXXX")==0)
		typedata = 0;
	else{
		cout << " Unknow type data !" << endl;
	}
	encoding = -1;
	 */
	if(strcmp(uppercase(ezxml_attr(stream,"Encoding")),"ASCII")==0)
		encoding = 0;
	else{
	  encoding = 1;
	}
	
	cout << "   - FileName = " << filename << endl;
	
	cout << "   - Type     = " << typedata ;
	switch(typedata){
		default:
			cout << " : Unknown" << endl;
	}
	cout << "   - Encoding = " << encoding;
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
		
	cout << "   - Length   = " << length << endl;
	cout << "   - Records  = " << records << endl;
	
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
				cout << " Noise " << Index << " (" << (NoisesData[Index]).size() << " spec): ";
				if (tmp_n.NType == 1){
					cout << "White " << tmp_n.NVal0;
					iNoise = new NoiseWhite(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, tmp_n.NVal0);
				}
				if (tmp_n.NType == 2){
					cout << "File " << tmp_n.NStr;
					iNoise = new NoiseFile(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, tmp_n.NStr);
				}
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
					cout << "Filter f*L/sqrt(P) : ";
					cout << "Level = " << tmp_n.NVal0 << " : ";
					tmpVal = tmp_n.NVal0/(M_PI*tStepPhy)*(Armlength/L0_m_default)*sqrt(LaserPower_W_default/LaserPower);
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
				// modif e.p. Octobre 2007 (puts the MLDC noise for IM)
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
					// 1/f² Noise
					tmpVal = tmp_n.NVal0*tmp_n.NVal01*M_PI*M_PI*tStepPhy*tStepPhy;
					cout << endl << "Filter MLDC_IM (f¯2) : ";
					cout << "Level = " <<tmp_n.NVal01  << " : ";
					(beta[0])[0] = tmpVal;
					(beta[0])[1] = 2*(beta[0])[0];
					(beta[0])[2] = (beta[0])[0]; 
					cout << "alpha =";
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " , beta =";
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
					NbDataStab = 100000;
					cout << "  ; NbDataStab   "<< NbDataStab;
					Filter NF_MLDC_IM_f2(alpha, beta, NbDataStab);
					
					// 1/f Noise
					cout << endl << "Filter MLDC_IM (f¯1) : ";
					cout << "Level = " << tmp_n.NVal0 << " : ";
					(alpha[0]).resize(1);
					(beta[0]).resize(2);
					(alpha[0])[0] = 1.0*coef_alpha_ep;
					(beta[0])[0] = tmp_n.NVal0*M_PI*tStepPhy;
					(beta[0])[1] = (beta[0])[0];
					cout << "alpha ="; 
					for(int i=0; i<(alpha[0]).size(); i++){
						cout << " " << (alpha[0])[i];
					}
					cout << " , beta =";
					for(int i=0; i<(beta[0]).size(); i++){
						cout << " " << (beta[0])[i];
					}
					cout << "  ; NbDataStab   "<< NbDataStab;
					Filter NF_MLDC_IM_f1(alpha, beta, NbDataStab);
					iNoise = new NoiseTwoFilter(tStepPhy, tStepMes, tMemNoiseFirst, tMemNoiseLast, NF_MLDC_IM_f1, NF_MLDC_IM_f2);
					noise_TwoFilter = 1;
					
				}
				// end of modif e.p.
				cout << endl;
			}
			//For all filtred noises
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
\big(2 + ceil(\frac{TDIInterpUtilVal}{2}) \big) \cdot tStepMes & \textrm{ if (TDIInterp = LAG)} \\
 2 \cdot tStepMes & else  \end{array} \right. \f]
 *
 */
double ConfigSim::tMemNecInterpTDI()
{
	double tMemOtStep(2.0);
	if(TDIInterp == LAG)
		tMemOtStep += ceil(TDIInterpUtilVal/2.0);
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
	NNoise = false ; // always calculate the tau (internal) phasemeters
	return(NNoise);
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

bool ConfigSim::FindTDIName(const char * generatorname, vector<int> & tmp_TDIPacks)
{
	bool UnKnowTDI(true);
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
	
	return(UnKnowTDI);
}


//end of LISACODE-ConfigSim.cpp

