         /* ----------------  DETECTOR CONSTANTS  ---------------- */               

/* Data constants:  Observation time (seconds),  Number of data points*/ 

 /* one "mldc year" at 15 s sampling */
//#define T 31457280.0 
//#define NFFT 2097152 

 /* two "mldc years" at 15 s sampling */
//#define T 62914560.0 
//#define NFFT 4194304 

 /* four "mldc years" at 15 s sampling */
#define T 125829120.0 
#define NFFT 8388608  

 /* sampling rate determined by T, NFFT */
#define dt (T/NFFT)


/* Instrument constants:  Noise levels, Size of constellation  */ 
 
 /* Mean arm length of the LISA detector (meters) */ 
#define L 5.000000e+09

 /* Photon shot noise power */ 
#define Sps 3.240000e-22

 /* Acceleration noise power */ 
#define Sacc 9.000000e-30

 /* Transfer frequency = c/(2*pi*L) */ 
#define fstar 0.0095426903

