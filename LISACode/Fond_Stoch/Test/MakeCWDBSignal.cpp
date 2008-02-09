// $Id: MakeCWDBSignal.cpp,v 1.2 2007/02/19 10:17:04 ruiloba Exp $
/*
 *  MakeCWDBSignal.cpp
 *
 *
 *
 *  Created on 01/06/06 by Antoine PETITEAU (APC)
 *  Last modification on 01/06/06 by Antoine PETITEAU (APC)
 *
 */

// Complation : g++ MakeCWDBSignal.cpp -lm -o MakeCWDBSignal
// Execution  : ./MakeCWDBSignal SC1File SC2File SC3File OutputFile


#include <iostream.h>
#include <fstream.h>
#include <string>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector.h>

using namespace std;


int main (int argc, char * const argv[])
{
	
	cout << " ****************************************" << endl;
	cout << " *                                      *" << endl;
	cout << " *  Creation of Phasemeters' File for   *" << endl;
	cout << " *  ---------------------------------   *" << endl;
	cout << " *  Confusion Whites Dwarfs Background  *" << endl;
	cout << " *  ----------------------------------  *" << endl;
	cout << " *                                      *" << endl;
	cout << " ****************************************" << endl << endl;
	
	char * SC1FileName;
	char * SC2FileName;
	char * SC3FileName;
	char * OutputFileName;
	ifstream SC1File;
	ifstream SC2File;
	ifstream SC3File;
	ofstream OutputFile;
	int nl(0), NbLine(0);	
	double t0(0.0), t1(0.0), tn(0.0);
	double tLastInit(0.0), tLastMax(0.0), dtLast(0.0);
	//double tNewInit(0.0), tNewMax(0.0), dtNew(0.0);
	double tmpread;
	
	
	
	//cout << argc << " arguments recus." << endl;
	if(argc >= 2){
		SC1FileName = argv[1];
	}else{
		SC1FileName = new char [30];
		cout << " Entrer le nom du fichier du satellite 1 : ";
		cin >> SC1FileName;
	}
	if(argc >= 3){
		SC2FileName = argv[2];
	}else{
		SC2FileName = new char [30];
		cout << " Entrer le nom du fichier du satellite 2 : ";
		cin >> SC2FileName;
	}
	if(argc >= 4){
		SC3FileName = argv[3];
	}else{
		SC3FileName = new char [30];
		cout << " Entrer le nom du fichier du satellite 3 : ";
		cin >> SC3FileName;
	}
	if(argc >= 5){
		OutputFileName = argv[4];
	}else{
		OutputFileName = new char [30];
		cout << " Entrer le nom du fichier de sortie global : ";
		cin >> OutputFileName;
	}
	
	/****************************/
	/*** Lecture des fichiers ***/
	/****************************/
	
	cout << "Lecture de " << SC1FileName << " ..." << endl;
	SC1File.open(SC1FileName);
	if (!SC1File){
		cerr << "Erreur : Error to open the file  " << SC1FileName << "." << endl;
		exit(1);
	}
	// Lit le header
	while(SC1File.peek() == '#'){
		SC1File.ignore(256,'\n'); 
	};
	// Compte le nombre de lignes
	nl = 0;
	while(!SC1File.eof()){
		//Lecture du temps
		switch (nl){
			case 0 :
				SC1File >> t0;
				break;
			case 1 :
				SC1File >> t1;
				break;
			default :
				SC1File >> tn;
		}
		// Lecture du reste de la ligne
		SC1File.ignore(256,'\n');
		if(!SC1File.eof()){	
			nl++;
		}
		//cout << nl << endl;
	};
	SC1File.close();
	SC1File.clear();
	
	tLastInit = t0;
	dtLast = fabs(t0-t1);
	tLastMax = tn;
	NbLine = nl;
	
	
	cout << "Lecture de " << SC2FileName << " ..." << endl;
	SC2File.open(SC2FileName);
	if (!SC2File){
		cerr << "Erreur : Error to open the file  " << SC2FileName << "." << endl;
		exit(1);
	}
	// Lit le header
	while(SC2File.peek() == '#'){
		SC2File.ignore(256,'\n'); 
	};
	// Compte le nombre de lignes
	nl = 0;
	while(!SC2File.eof()){
		//Lecture du temps
		switch (nl){
			case 0 :
				SC2File >> t0;
				break;
			case 1 :
				SC2File >> t1;
				break;
			default :
				SC2File >> tn;
		}
		// Lecture du reste de la ligne
		SC2File.ignore(256,'\n');
		if(!SC2File.eof()){	
			nl++;
		}
		//cout << nl << endl;
	};
	SC2File.close();
	SC2File.clear();
	
	//cout << " fabs(tInit-t0) = " << fabs(tInit-t0) << endl;
	//cout << " fabs(dt-fabs(t0-t1))/dt = " << fabs(dt-fabs(t0-t1))/dt << endl;
	//cout << " fabs(tMax-tn)/tMax = " << fabs(tMax-tn)/tMax << endl;
	//cout << " NbLine!=nl = " << (NbLine!=nl) << endl;
	if((fabs(tLastInit-t0)>1.0e-6)||(fabs(dtLast-fabs(t0-t1))/dtLast>1.0e-6)||(fabs(tLastMax-tn)/tLastMax>1.0e-6)||(NbLine!=nl)){
		cerr << " Erreur : Les parametres temporelles ne sont pas les memes dans tous les fichiers." << endl;
		exit(1);
	}
	
	
	
	cout << "Lecture de " << SC3FileName << " ..." << endl;
	SC3File.open(SC3FileName);
	if (!SC3File){
		cerr << "Erreur : Error to open the file  " << SC3FileName << "." << endl;
		exit(1);
	}
	// Lit le header
	while(SC3File.peek() == '#'){
		SC3File.ignore(256,'\n'); 
	};
	// Compte le nombre de lignes
	nl = 0;
	while(!SC3File.eof()){
		//Lecture du temps
		switch (nl){
			case 0 :
				SC3File >> t0;
				break;
			case 1 :
				SC3File >> t1;
				break;
			default :
				SC3File >> tn;
		}
		// Lecture du reste de la ligne
		SC3File.ignore(256,'\n');
		if(!SC3File.eof()){	
			nl++;
		}
		//cout << nl << endl;
	};
	SC3File.close();
	SC3File.clear();
	
	//cout << " fabs(tInit-t0) = " << fabs(tInit-t0) << endl;
	//cout << " fabs(dt-fabs(t0-t1))/dt = " << fabs(dt-fabs(t0-t1))/dt << endl;
	//cout << " fabs(tMax-tn)/tMax = " << fabs(tMax-tn)/tMax << endl;
	//cout << " NbLine!=nl = " << (NbLine!=nl) << endl;
	
	if((fabs(tLastInit-t0)>1.0e-6)||(fabs(dtLast-fabs(t0-t1))/dtLast>1.0e-6)||(fabs(tLastMax-tn)/tLastMax>1.0e-6)||(NbLine!=nl)){
		cerr << " Erreur : Les parametres temporelles ne sont pas les memes dans tous les fichiers." << endl;
		exit(1);
	}
	
	cout << "  Nombre de mesures = " << NbLine << endl;
	cout << "  Temps initial = " << tLastInit << endl;
	cout << "  Temps final  = " << tLastMax << endl;
	cout << "  Pas de temps  = " << dtLast << endl;

	
	/******************************************/
	/*** Ecriture dans le fichier de sortie ***/
	/******************************************/
	
	cout << "Ecriture du fichier de sortie " << OutputFileName << " ..." << endl;
	SC1File.open(SC1FileName);
	SC2File.open(SC2FileName);
	SC3File.open(SC3FileName);
	OutputFile.open(OutputFileName);
	
	// Lit le header
	while(SC1File.peek() == '#'){
		SC1File.ignore(256,'\n'); 
	};
	while(SC2File.peek() == '#'){
		SC2File.ignore(256,'\n'); 
	};
	while(SC3File.peek() == '#'){
		SC3File.ignore(256,'\n'); 
	};
	OutputFile << "#Time s1 s2 s3 sp1 sp2 sp3" << endl;
	OutputFile.precision(15);
	for(int i=0; i<NbLine; i++){
		SC1File >> tmpread; // time
		OutputFile << tmpread << " ";
		SC1File >> tmpread; // s1
		OutputFile << tmpread << " ";
		SC2File >> tmpread;
		SC2File >> tmpread; // s2
		OutputFile << tmpread << " ";
		SC3File >> tmpread;
		SC3File >> tmpread; // s3
		OutputFile << tmpread << " ";
		SC1File >> tmpread; // sp1
		OutputFile << tmpread << " ";
		SC2File >> tmpread; // sp2
		OutputFile << tmpread << " ";
		SC3File >> tmpread; // sp3
		OutputFile << tmpread << endl;
		SC1File >> tmpread >> tmpread;
		SC2File >> tmpread >> tmpread;
		SC3File >> tmpread >> tmpread;
	}
	OutputFile.close();
	
	
/*	// *** Version fichier binaire ***
	
	OutputFile.open(OutputFileName,ios::binary);
	// Lit le header
	while(SC1File.peek() == '#'){
		SC1File.ignore(256,'\n'); 
	};
	while(SC2File.peek() == '#'){
		SC2File.ignore(256,'\n'); 
	};
	while(SC3File.peek() == '#'){
		SC3File.ignore(256,'\n'); 
	};
	for(int i=0; i<NbLine; i++){
		SC1File >> tmpread; // time
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC1File >> tmpread; // s1
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC2File >> tmpread;
		SC2File >> tmpread; // s2
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC3File >> tmpread;
		SC3File >> tmpread; // s3
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC1File >> tmpread; // sp1
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC2File >> tmpread; // sp2
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC3File >> tmpread; // sp3
		OutputFile.write((char*) &tmpread, sizeof(double));
		SC1File >> tmpread >> tmpread;
		SC2File >> tmpread >> tmpread;
		SC3File >> tmpread >> tmpread;
	}
	OutputFile.close();
	*/
	
	
	SC1File.close();
	SC2File.close();
	SC3File.close();
	
	
	cout << endl << "End" << endl;
	
    return 0;
}


