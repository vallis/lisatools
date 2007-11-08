#define NR_END 1
#define FREE_ARG char*

 /*  Cusp parameters  */

typedef struct
{    
  double Amplitude;
  double CentralTime;
  double Polarization;       // rotation done inside this code
  double EclipticLatitude;
  double EclipticLongitude;
  double MaximumFrequency;   // fine if above the LISA band, but some should be inside 1e-4--10 Hz, uniformly in log
  int Samples;
  double Cadence;
  double Tpad;               // both before and after...
} Cusp_structure;

void Cusp_Barycenter(Cusp_structure Cusp, double *hp, double *hc, int n);
void KILL(char*); 
double *dvector(long nl, long nh);
void free_dvector(double *v, long nl, long nh);
