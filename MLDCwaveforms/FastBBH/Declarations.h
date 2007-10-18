/*******************************  STRUCTURES  *******************************/


 /*  BH parameters  */

typedef struct
{
  double Mass1;                
  double Mass2;                 
  double CoalescenceTime;
  double Distance;
  double EclipticLatitude;
  double EclipticLongitude;
  double Spin1;
  double Spin2;
  double PolarAngleOfSpin1;
  double PolarAngleOfSpin2;    
  double AzimuthalAngleOfSpin1;
  double AzimuthalAngleOfSpin2;
  double PhaseAtCoalescence;
  double InitialPolarAngleL;
  double InitialAzimuthalAngleL;
  double TaperApplied;
  double AmplPNorder;
} SBH_structure;

void SBH_Barycenter(SBH_structure SBH, double *hp, double *hc);

double Freq(double t, double Mtot, double Mchirp, double eta, double beta, double sigma, double tc);

void spline(double *x, double *y, int n, double yp1, double ypn, double *y2);

void rkckstep(double outputvals[], double fourthorderoutputvals[], double *outputthomas, double h, double currentvals[], double currentthomas, double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc);

void update(int j, double A, double h, double intvals[], double t, double m1, double m2, double Mtot, double Mchirp, double mu, double eta, double chi1, double chi2, double N[], double tc, double **k, double kthomas[]);

void calcderivvals(double derivvals[], double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S1xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S1ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S1zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S2xdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S2ydot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double S2zdot(double inputs[], double r, double m1, double m2, double Mtot, double mu, double chi1, double chi2);

double calcLdotS1(double inputs[]);

double calcLdotS2(double inputs[]);

double calcSdotS(double inputs[]);

double calcLdotN(double inputs[], double nvec[]);

void calcLcrossN(double output[], double inputs[], double nvec[]);

void KILL(char*);

