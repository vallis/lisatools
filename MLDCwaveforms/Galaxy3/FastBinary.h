extern "C" {
    #include <fftw3.h>
    
    double AEnoise(double f);
}

class FastResponse {
private:
    long N, M;
    
    double *u,*v,*k;            // Gravitational Wave basis vectors
    double *kdotx, **kdotr;     // Dot products
    double *xi, *f, *fonfs;     // Distance, gravitational wave frequency & ratio of f and transfer frequency f*
    double **eplus, **ecross, **dplus, **dcross;    // Polarization basis tensors, convenient quantities    
    double *x, *y, *z;                              // Spacecraft position and separation vectors
    double *r12, *r13, *r21, *r23, *r31, *r32;
    double **TR, **TI;                              // Time varying quantities (Re & Im) broken up into convenient segments
    double *data12, *data13, *data21, *data23, *data31, *data32;    // Fourier coefficients before FFT and after convolution:
                                                                    // Time series of slowly evolving terms at each vertex   
    double *a12, *a13, *a21, *a23, *a31, *a32;                      // Fourier coefficients of slowly evolving terms (numerical)
    double *b;                                                      // Fourier coefficients of rapidly evolving terms (analytical)
    double *c12, *c13, *c21, *c23, *c31, *c32;                      // Fourier coefficients of entire response (convolution)
    double ***d;                                                    // Package cij's into proper form for TDI subroutines

    double *ReA, *ImA, *ReB, *ImB, *ReC, *ImC;    

    double *X, *Y, *Z;

    fftw_complex *in, *out;     // used by fftw3
    fftw_plan plan_forward;

    void spacecraft(double t);
    void convolve(double *a, double *b, double *cn);
    void XYZ(double f0, long q, double *XLS, double *XSL, double *YLS, double *YSL, double *ZLS, double *ZSL);

public:
    FastResponse(long Nreq,long Mreq);
    ~FastResponse();
    
    void Response(double f0,double fdot,double theta,double phi,double A,double iota,double psi,double phio,
                  double *XLS,long XLSlen,double *XSL,long XSLlen,
                  double *YLS,long YLSlen,double *YSL,long YSLlen,
                  double *ZLS,long ZLSlen,double *ZSL,long ZSLlen);
};
