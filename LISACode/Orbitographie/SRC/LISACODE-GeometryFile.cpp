// $Id: $
/*
 *  LISACODE-GeometryFile.cpp
 *  V 1.4.1
 *  LISACode
 *
 *  Created on 21/12/07 by E.Plagnol
 *  Last modification on 13/05/08 by A.Petiteau
 *
 */

#include "LISACODE-GeometryFile.h"


/* Constructor */

/*! \brief Constructs an instance and initializes it with default values.
*
* #init is called with the following arguments :
* \arg	t0_n = 0.0
* \arg	rot0_n = 0.0
* \arg	L0m_n = #L0_m_default
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryFile::GeometryFile()
{
	init(0.0, 0.0, L0_m_default, -1, 1, 10.0,"Orbit_ESA.input.txt");
	
}

/*! \brief Constructs an instance and initializes it with default values and t0_n and rot0_n inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = #L0_m_default
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryFile::GeometryFile(double t0_n, double rot0_n)
{
	init(t0_n, rot0_n, L0_m_default, -1, 1, 10.0,"Orbit_ESA.input.txt");
}


/*! \brief Constructs an instance and initializes it with default values and t0_n, rot0_n and L0m_n inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = -1
* \arg	move_n = 1
* \arg	tStep = 10.0
*/
GeometryFile::GeometryFile(double t0_n, double rot0_n, double L0m_n)
{
	init(t0_n, rot0_n, L0m_n, -1, 1, 10.0, "Orbit_ESA.input.txt");
}


/*! \brief Constructs an instance and initializes it with t0_n, rot0_n, L0m_n,  order_default_n, move_n and tStep inputs.
* The orbits are readed in file 
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = order_default_n input
* \arg	move_n = move_n input
* \arg	tStep = tStep input
*/
GeometryFile::GeometryFile(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep, "Orbit_ESA.input.txt");
}

/*! \brief Constructs an instance and initializes it with t0_n, rot0_n, L0m_n,  order_default_n, move_n and tStep inputs.
*
* #init is called with the following arguments :
* \arg	t0_n = t0_n input
* \arg	rot0_n = rot0_n input
* \arg	L0m_n = L0m_n input
* \arg	order_default_n = order_default_n input
* \arg	move_n = move_n input
* \arg	tStep = tStep input
*/
GeometryFile::GeometryFile(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep, char * InputFileName_n)
{
	init(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep, InputFileName_n);
}



/* Destructor */
/*! \brief Destructor */
GeometryFile::~GeometryFile()
{
	
}


/* Access methods */
/*! \brief Initialization
*
*/
void GeometryFile::init(double t0_n, double rot0_n, double L0m_n, int order_default_n, int move_n, double tStep, char * InputFileName_n)
{
	double junk;
	double day_sec(3600*24.);
	int nb, nl;
	ifstream InputFile;
	
	initGlobal(t0_n, rot0_n, L0m_n, order_default_n, move_n, tStep);

	cout << "Create orbits : Orbits in file ... " << endl ;
	cout << "    Reading Input file ... " << endl;

	InputFileName = InputFileName_n ;
	cout << "    Loading of the input file :" << InputFileName << " ..." << endl;
	
	// Open file
	InputFile.open(InputFileName);
	if (!InputFile){
		cerr << "Error : Problem for open the file  " << InputFileName << endl;
		exit(1);
	}
	while((InputFile.peek() == '#')&&(!InputFile.eof())){
		InputFile.ignore(256,'\n'); 
	};
	NbData = 0 ;
	InputFile >> NbData ; // number of lines of data
	cout << " Number of data = " << NbData << endl ;
	InputFile.ignore(256,'\n'); 
	
	XYZ = (double***)malloc(3*sizeof(double**));
	for(int ip=0; ip<3; ip++){
		XYZ[ip] = (double**)malloc(3*sizeof(double*));
		for(int it=0; it<3; it++)
			XYZ[ip][it] = (double*)malloc(NbData*sizeof(double));
	}
	Time_sec = (double*)malloc(NbData*sizeof(double));
	
	for(nl=0; nl<NbData; nl++){
		//cout << " nl =" << nl << endl;
		InputFile >> junk ;
		InputFile >> Time_sec[nl] ;
		for( nb=0 ; nb<3 ; nb++){
			for( int ixyz=0 ; ixyz<3 ; ixyz++){
				//cout << "nl,nb,ixyz =" << nl << "  " << nb << "  " << ixyz << endl;
				InputFile >>  XYZ[nb][ixyz][nl];
				XYZ[nb][ixyz][nl] = 1000*XYZ[nb][ixyz][nl] ; // from km to m
			}
		}
		Time_sec[nl] = day_sec*Time_sec[nl]; // from days to seconds
		/*
		 if((nl < 10)||((nl > 180) && (nl < 190))){
			 for(nb=0 ; nb<3; nb++){
				 cout << " sat,line,Sec,x,y,z " << nb << "  " << nl << "  " << Time_sec[nl] << "  " << 
				 XYZ[nb][0][nl] << "  " << XYZ[nb][1][nl] << "  " << XYZ[nb][2][nl] << endl ;
			 }
		 }
		 if(nl == 11){
			 cout << " ... ... .. " << endl;
			 cout << " ... ... .. " << endl;
			 cout << " ... ... .. " << endl;
		 }
		 */
	}
	InputFile.close();
	InputFile.clear();
	
	cout << "End of loading of the input file : " << InputFileName << endl << endl;
	//throw invalid_argument("Geometry::init: ESA Orbits : Voluntary stop!");
	
	for(int i=1; i<4; i++){
		SCposStore[i-1] = position(i,0.0);
	}
	
	for(int i=1; i<4; i++){
		DelayStore[i-1] = tdelay(i,(i+1)%3+1,2,0.0);
		//cout << " L" << i << (i+1)%3+1<< endl;
		DelayStore[i+2] = tdelay(i,i%3+1,2,0.0);
		//cout << " L" << i << i%3+1 << endl;
	}
	
	cout << " --> OK" << endl;
}


void GeometryFile::DispInfo()
{
	DispGeneralInfo();
	cout << "    - Type             = File" << endl;
	cout << "--------------------" << endl;
}


/* Other methods */

/*! \brief Returns the position of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
* 
*
*/

Vect GeometryFile::position(int nb, double t)
{
	Vect r;
	int order(7);
	double time(t0+move*t);
	int nb0 = nb-1;
	double tStep = Time_sec[1];
	double xbin(time/tStep);
	int bin((int)floor(xbin+100.0*1e-6));
	double Pk(0.0), Pkv(0.0);
	int ordermin((int)(floor(double(order+1)/2.0)));
	int kmin(bin-ordermin+1);
	int kmax(bin+(order+1-ordermin));
	
	//cout << "NbData, nb,Pos(X,Y,Z) = "<< NbData << "  " << nb << "  " << XYZ[nb-1][0][0]
	//     << "  " << XYZ[nb-1][1][0] << "  " << XYZ[nb-1][2][0] << endl ; 
	//throw invalid_argument("Geometry::Position : ESA Orbits : Voluntary stop!");

	//cout << "  kmin = " << kmin << " , ordermin= " << ordermin << endl;
	if (kmin < 0){
	    kmin=0 ; 
	}
	r.p[0]=0. ;
	r.p[1]=0. ;
	r.p[2]=0. ;
	for(int k=kmin; k<=kmax; k++){ // Lagrange Interpolation
	    Pk = 1.0; Pkv = 1.0;
	    for(int j=kmin; j<=kmax; j++){
			if(j!=k){
				//cout << " k,j,tj,tk =" <<k << "  " << j << "  "<<tFirst+j*tStep << "  " << tFirst+k*tStep << endl ;
				//Pk *= (time-tFirst+j*tStep)/((j-k)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
				Pk *= (time-Time_sec[j])/((k-j)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
				Pkv *= (time-Time_sec[j]+1)/((k-j)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
			}
	    }
	    for(int j=0 ; j<3 ; j++){
			r.p[j] +=XYZ[nb0][j][k]*Pk ;
			//v.p[j] +=XYZ[nb0][j][k]*Pkv ;
	    }
	    
	    //cout << " k,Y1[k],Pk,Pkv,Pos[1],Vel[1] = " << k << "  " << Y1[k]<< "  " << Pk << "  " << Pkv << "  " << Pos[1] << "  " << Vel[1] << endl;
	}
	
	return r;
}



/*! \brief Returns the velocity of the spacecraft in the barycentric frame, depending on time ts (s) and spacecraft number nb=[1,3].
*
*
*/

Vect GeometryFile::velocity(int nb, double t)
{
	Vect v;
	Vect r;
		  //cout << "NbData, nb,Pos(X,Y,Z) = "<< NbData << "  " << nb << "  " << XYZ[nb-1][0][0]
		  //     << "  " << XYZ[nb-1][1][0] << "  " << XYZ[nb-1][2][0] << endl ; 
		  //throw invalid_argument("Geometry::Position : ESA Orbits : Voluntary stop!");
	int order(7);
	double time(t0+move*t);
	int nb0 = nb-1;
	double tStep = Time_sec[1];
	double xbin(time/tStep);
	int bin((int)floor(xbin+100.0*1e-6));
	double Pk(0.0), Pkv(0.0);
	int ordermin((int)(floor(double(order+1)/2.0)));
	int kmin(bin-ordermin+1);
	int kmax(bin+(order+1-ordermin));
	
	if (kmin < 0){
	    kmin=0 ; }
	//cout << "  bin = " << bin << "  kmin = " << kmin << "  ordermin = " << ordermin << "  tStep = " << tStep << endl;
	
	
	//cout << " kmin,kmax = " << kmin << "  " << kmax << endl ;
	r.p[0]=0. ;
	r.p[1]=0. ;
	r.p[2]=0. ;
	v.p[0]=0. ;
	v.p[1]=0. ;
	v.p[2]=0. ;
	for(int k=kmin; k<=kmax; k++){ // Lagrange Interpolation
	    Pk = 1.0; Pkv = 1.0;
	    for(int j=kmin; j<=kmax; j++){
			if(j!=k){
				//cout << " k,j,tj,tk =" <<k << "  " << j << "  "<<tFirst+j*tStep << "  " << tFirst+k*tStep << endl ;
				//Pk *= (time-tFirst+j*tStep)/((j-k)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
				Pk *= (time-Time_sec[j])/((k-j)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
				Pkv *= (time-Time_sec[j]+1)/((k-j)*tStep); //Pk *= (t-tj)/(tk-tj) with ti=tFirst-i*tStep
			}
	    }
	    for(int j=0 ; j<3 ; j++){
			r.p[j] +=XYZ[nb0][j][k]*Pk ;
			v.p[j] +=XYZ[nb0][j][k]*Pkv ;
	    }
	}
	for(int j=0 ; j<3 ; j++){
	    v.p[j] = v.p[j]-r.p[j]; // difference between two positions at 1 sec interval
								//cout << " j,r,v = " << j << "  " << r.p[j] << "  " << v.p[j] << endl ;
	}
	
	/*	cout << "Vel : t,nb,x,y,z =" << t << "  "<<nb<<"  "<<v.p[0]<<"  "<<v.p[1]<<"  "<<v.p[2]<< endl;
	if (t > 4.){
		throw invalid_argument("GeometryAnalytic::velocity : On arrete !");
	}*/	
		
	return v;
}



//end of LISACODE-GeometryFile.cpp


