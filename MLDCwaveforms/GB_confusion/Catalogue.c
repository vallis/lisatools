/* gcc -O2 -o Catalogue Catalogue.c -lm */
/* 04/28/2011 -- convert GC to EC -- TBL */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define pi 3.141592653589793
#define pc 3.0856775807e16
#define clight 299792458.
#define Msun 1.9889e30
#define G 6.67259e-11


double ran2(long *idum);

int main(int argc,char **argv)
{
	int i;
	long seed = -987654876;
	double junk;
	
	//Nelemns parameters
	double m1,m2,Porb,Porb_dot,m2_dot,l,b,DL;
	
	//Conversion parameters
	double Mchirp;
	
	//LISA parameters
	double f,fdot,theta,phi,Amp,iota,psi,phi0;
	
	double deg2rad = pi/180.0;
	double gc = 8500.0;
	double kpc2pc =  1000.0;
	double x,y,z;
	double xgc,ygc,zgc;
	double xec,yec,zec,rec;
	
	FILE *Infile;	
	FILE *Outfile; 
	
	int n,NS;

	Outfile = fopen("SourceCatalogue.txt","w");

	Infile = fopen("dwd_GWR_NewLISA.dat","r");
	NS = 26084393;
	for(n=0; n<NS; n++)
	{
		if(n%1000000==0)printf("Converting DWD: %i/%i\n",n,NS);
		
		/*Nelemens files*/
		fscanf(Infile, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%i\n", &m1, &m2, &Porb, &Porb_dot, &m2_dot, &l, &b, &DL, &junk, &junk, &junk, &i);
		
		//convert galactic coordinates to radians
		l = l*deg2rad;
		b = (90.0 - b)*deg2rad;
		
		//convert distance to pc
		DL *= kpc2pc;
		
		//change to ecliptic coordinates
		x = DL*sin(b)*cos(l);
		y = DL*sin(b)*sin(l);
		z = DL*cos(b);
		
		xgc = x-gc;
		ygc = y;
		zgc = z;
		
		xec = -0.05487556043*x + 0.4941094278*y - 0.86766614920*z;
		yec = -0.99382137890*x - 0.1109907351*y - 0.00035159077*z;
		zec = -0.09647662818*x + 0.8622858751*y + 0.49714719180*z;
		
		rec = sqrt(xec*xec + yec*yec + zec*zec);
		
		theta = pi/2.0 - acos(zec/rec);
		phi   = atan2(yec,xec);
		while(phi<0.0) phi += 2.0*pi;
		
		//convert orbital parameters to GW parameters
		f    =  2.0/Porb;
		fdot = -2.0*Porb_dot/Porb/Porb;
		
		//convert mass+distance to amplitude
		m1 *= Msun;
		m2 *= Msun;
		Amp    = 2.0 * G*G*m1*m2 * pow(pi*pi*f*f/(G*(m1+m2)),1./3.) / (DL*pc) /(clight*clight*clight*clight); 		
		
		//randomly assign orientation parameters
		iota = acos(-1.0 + 2.0*ran2(&seed));
		psi  = ran2(&seed)*2.0*pi;
		phi0 = ran2(&seed)*2.0*pi; 
		
		fprintf(Outfile, "%.16g %.10g %.6g %.6g %.6g %.6g %.6g %.6g\n",f,fdot,theta,phi,Amp,iota,psi,phi0);
	}
	fclose(Infile);
	
	
	Infile = fopen("AMCVn_GWR_MLDC_bulgefix_opt.dat","r");		
	NS = 34223502;
	for(n=0; n<NS; n++)
	{
		if(n%1000000==0)printf("Converting AM Cvn: %i/%i\n",n,NS);
		/*Nelemens files*/
		fscanf(Infile, "%lf%lf%lf%lf%lf%lf%lf%lf\n", &m1, &m2, &Porb, &Porb_dot, &m2_dot, &l, &b, &DL);

		//convert galactic coordinates to radians
		l = l*deg2rad;
		b = (90.0 - b)*deg2rad;
		
		//convert distance to pc
		DL *= kpc2pc;
		
		//change to ecliptic coordinates
		x = DL*sin(b)*cos(l);
		y = DL*sin(b)*sin(l);
		z = DL*cos(b);
		
		xgc = x-gc;
		ygc = y;
		zgc = z;
		
		xec = -0.05487556043*x + 0.4941094278*y - 0.86766614920*z;
		yec = -0.99382137890*x - 0.1109907351*y - 0.00035159077*z;
		zec = -0.09647662818*x + 0.8622858751*y + 0.49714719180*z;
		
		rec = sqrt(xec*xec + yec*yec + zec*zec);
		
		theta = pi/2.0 - acos(zec/rec);
		phi   = atan2(yec,xec);
		while(phi<0.0) phi += 2.0*pi;
		
		//convert orbital parameters to GW parameters
		f    =  2.0/Porb;
		fdot = -2.0*Porb_dot/Porb/Porb;
		
		//convert mass+distance to amplitude
		m1 *= Msun;
		m2 *= Msun;
		Amp    = 2.0 * G*G*m1*m2 * pow(pi*pi*f*f/(G*(m1+m2)),1./3.) / (DL*pc) /(clight*clight*clight*clight); 		
		
		//randomly assign orientation parameters
		iota = acos(-1.0 + 2.0*ran2(&seed));
		psi  = ran2(&seed)*2.0*pi;
		phi0 = ran2(&seed)*2.0*pi; 
		
		fprintf(Outfile, "%.16g %.10g %.6g %.6g %.6g %.6g %.6g %.6g\n",f,fdot,theta,phi,Amp,iota,psi,phi0);
	}
	fclose(Infile);
	fclose(Outfile);
	
	Outfile = fopen("VerificationBinaries.txt","w");
	Infile = fopen("Verification_binaries.dat","r");		
	NS = 35;
	for(n=0; n<NS; n++)
	{
		if(n%10==0)printf("Converting Verification Binaries: %i/%i\n",n,NS);
		/*Nelemens files*/
		fscanf(Infile, "%lf%lf%lf%lf%lf%i%lf%lf%lf%i\n", &m1, &m2, &Porb, &Porb_dot, &m2_dot, &i, &l, &b, &DL,&i);
		
		//convert galactic coordinates to radians
		l = l*deg2rad;
		b = (90.0 - b)*deg2rad;
		
		//convert distance to pc
		DL *= kpc2pc;
		
		//change to ecliptic coordinates
		x = DL*sin(b)*cos(l);
		y = DL*sin(b)*sin(l);
		z = DL*cos(b);
		
		xgc = x-gc;
		ygc = y;
		zgc = z;
		
		xec = -0.05487556043*x + 0.4941094278*y - 0.86766614920*z;
		yec = -0.99382137890*x - 0.1109907351*y - 0.00035159077*z;
		zec = -0.09647662818*x + 0.8622858751*y + 0.49714719180*z;
		
		rec = sqrt(xec*xec + yec*yec + zec*zec);
		
		theta = pi/2.0 - acos(zec/rec);
		phi   = atan2(yec,xec);
		while(phi<0.0) phi += 2.0*pi;
		
		//convert orbital parameters to GW parameters
		f    =  2.0/Porb;
		fdot = -2.0*Porb_dot/Porb/Porb;
		
		//convert mass+distance to amplitude
		m1 *= Msun;
		m2 *= Msun;
		Amp    = 2.0 * G*G*m1*m2 * pow(pi*pi*f*f/(G*(m1+m2)),1./3.) / (DL*pc) /(clight*clight*clight*clight); 		
		
		//randomly assign orientation parameters
		iota = acos(-1.0 + 2.0*ran2(&seed));
		psi  = ran2(&seed)*2.0*pi;
		phi0 = ran2(&seed)*2.0*pi; 
		
		fprintf(Outfile, "%.16g %.10g %.6g %.6g %.6g %.6g %.6g %.6g\n",f,fdot,theta,phi,Amp,iota,psi,phi0);
	}
	fclose(Infile);
	fclose(Outfile);
	
	
    return 0;
	
}

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define eps 1.2e-7
#define RNMX (1.0-eps)

double ran2(long *idum)
{
    int j;
    long k;
    static long idum2=123456789;
    static long iy=0;
    static long iv[NTAB];
    double temp;
	
    if (*idum <= 0) {
        if (-(*idum) < 1) *idum=1;
        else *idum = -(*idum);
        idum2=(*idum);
        for (j=NTAB+7;j>=0;j--) {
            k=(*idum)/IQ1;
            *idum=IA1*(*idum-k*IQ1)-k*IR1;
            if (*idum < 0) *idum += IM1;
            if (j < NTAB) iv[j] = *idum;
        }
        iy=iv[0];
    }
    k=(*idum)/IQ1;
    *idum=IA1*(*idum-k*IQ1)-k*IR1;
    if (*idum < 0) *idum += IM1;
    k=idum2/IQ2;
    idum2=IA2*(idum2-k*IQ2)-k*IR2;
    if (idum2 < 0) idum2 += IM2;
    j=iy/NDIV;
    iy=iv[j]-idum2;
    iv[j] = *idum;
    if (iy < 1) iy += IMM1;
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
}

