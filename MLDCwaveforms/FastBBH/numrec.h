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
#define JMAX 40
#define JMAXP (JMAX+1)
#define K 5
#define NRANSI
#define Mnr 7
#define NSTACK 50
#define NR_END 1
#define FREE_ARG char*
#define TINY 1.0e-10
#define func(x) ((*func)(x))
#define funk(y) ((*func)(y[]))
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
#define SWAPP(a,b) tempr=(a);(a)=(b);(b)=tempr;
#define NMAX 10000
#define GOLD 1.618034
#define GLIMIT 100.0
#define TTINY 1.0e-20
#define RR 0.61803399
#define Cgold 0.38196601
#define SHFT2(a,b,c) (a)=(b);(b)=(c);
#define SHFT3(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define ffunc(x) ((*func) (x, *y, **z))
#define GET_PSUM							\
  for (j=1;j<=ndim;j++) {						\
    for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];			\
    psum[j]=sum;}
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);	\
  a[k][l]=h+s*(g-h*tau);
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))
static double maxarg1, maxarg2;
#define FMAX(a, b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2)?	\
		    (maxarg1):(maxarg2))
#define SQR(x) ((x) * (x))
#define CUBE(x) ((x) * (x) * (x))
#define FOURTH(x) ((x) * (x) * (x) * (x))

void dfour1(double data[], unsigned long nn, int isign);

void drealft(double data[], unsigned long n, int isign);

void jacobi(double **a, int n, double e[], double **v, int *nrot);

int *ivector(long nl, long nh);
void free_ivector(int *v, long nl, long nh);

double *dvector(long nl, long nh);
void free_dvector(double *v, long nl, long nh);

unsigned long *lvector(long nl, long nh);
void free_lvector(unsigned long *v, long nl, long nh);

double **dmatrix(long nrl, long nrh, long ncl, long nch);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);

double ***d3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_d3tensor(double ***t, long nrl, long nrh, long ncl, long nch, long ndl, long ndh);

double gasdev2(long *idum);

double ran2(long *idum);

void correlate (int n, double *data1, double *data2, double *psd, double *data, double *norm);

void polint(double xa[], double ya[], int n, double x, double *y, double *dy);
