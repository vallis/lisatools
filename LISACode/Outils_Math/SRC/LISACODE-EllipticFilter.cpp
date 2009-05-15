/***********************************************************/
/*                                                         */
/*  Subroutines for elliptic filter computations           */
/*                                                         */
/* Author : H. Halloin                                     */
/* Date of creation : 07 Nov 2006                          */
/*                                                         */
/* Adapted from correponding program in                    */
/* Maurice Bellanger, Traitement numÈrique du signal       */
/*                                                         */
/***********************************************************/


#include "LISACODE-EllipticFilter.h"

// Computation of poles, zeros and coef of elliptic cells
//
/*************************************************/
/*! \brief Poles, zeros and elliptic cells coefficients computation.
 *
 * Inputs are:
 * \arg eps : Oscillations in working bandwidth
 * \arg A :  Weakening of attenuated band
 * \arg f : Low frequency transition edge  [Hz]
 * \arg fb : High frequency transition edge [Hz]
 * \arg fe : Sampling frequency  [Hz]
 * \arg NCellMax : Maximum number of cells
 *
 * Outputs are :
 * \arg NCells : number of cells, must be positive and lower or equal to NCellMax
 * \arg poles : poles of the cells (imaginary part positive or null) 
 * \arg zero : zeros of the cells (imaginary part positive or null)
 * \arg CoefA : coefficient A of the cells
 * \arg CoefB : coefficient B of the cells
 * \arg CoefC : coefficient C of the cells
 * \arg CoefD : coefficient D of the cells
 *
 * Computations : \n
 * \f$  \omega_c = fb \cdot 2 \cdot \pi \f$ \n
 * \f$  \omega_r = fa \cdot 2 \cdot \pi \f$ \n
 * \f$  T = 1/fe \f$ \n
 * \f$  dk1 = \frac{eps}{\sqrt{A^2-1}} \f$ \n
 * \f$  dk = \frac{tan(\omega_c \cdot \frac{T}{2})}{tan(\omega_r \cdot \frac{T}{2})} \f$ \n
 * \f$  dkp = \sqrt{1-dk^2} \f$ \n
 * \f$  ak1 = ak(dk) \textrm{ using ak function} \f$ \n
 * \f$  ak2 = ak(dk1) \textrm{ using ak function} \f$ \n
 * \f$  ak3 = ak(dkp) \textrm{ using ak function} \f$ \n
 * \f$  ak4 = cak(dk1^2) \textrm{ using cak function} \f$ \n
 *
 * \f$ N = \frac{1}{2} \cdot ceil \big( ceil(\frac{ak4 \cdot ak1}{ak2 \cdot ak3}+1) \big) \f$ \n
 * N is checked : \f$ 0 \le N \le NCellMax \f$ \n
 *
 * \f$  U_0 = -\frac{ak3}{ak4} \cdot \frac{alog(1+\sqrt{(1+eps^2)})}{eps} \f$ \n
 * \arg for  \f$ i=0,\dots,N-1 \f$ \n
 * \f$ xmag = 2 \cdot i \cdot \frac{ak1}{2 \cdot N} \f$ \n
 * \f$ zeros[i] = -ak3 + I \cdot xmag \f$ \n
 * \f$ poles[i] = U_0 + I \cdot xmag \f$ \n
 * \arg for  \f$ i=0,\dots,2 \cdot N-1 \f$ \n
 * \f$ Q = real(zeros[mod(i,N)]) \f$ \n
 * \f$ R = imag(zeros[mod(i,N)]) \f$ \n
 * \f$ a1 = sn(Q, dkp, ak3, ak1) \textrm{ using sn function} \f$ \n
 * \f$ b1 = sn(R, dk,  ak1, ak3) \textrm{ using sn function} \f$ \n
 * \f$ \sigma= \left\{ \begin{array}{ll} 0 & \textrm{if } i \le N \\
 a1 \cdot \sqrt{(1-a1^2)*(1-b1^2)} \cdot \frac{dn}{de} & else \end{array} \right. \f$ \n
 * \f$ dn = \sqrt{1-{(dk \cdot b1)}^2} \f$ \n
 * \f$ de = 1-{(a1 \cdot dn)}^2 \f$ \n
 * \f$ \omega = b1 \cdot \frac{\sqrt{(1-(dkp \cdot a1)^2)}}{de} \f$ \n
 * \f$ C[i] = -2 \cdot \sigma \cdot \omega_c \f$ \n
 * \f$ D[i] = (\sigma^2 + \omega^2) \cdot {\omega_c}^2 \f$ \n
 * \f$ \sigma = \sigma \cdot tan(\omega_c \cdot \frac{T}{2}) \f$ \n
 * \f$ \omega = \omega \cdot tan(\omega_c \cdot \frac{T}{2}) \f$ \n
 * \f$ \left\{ \begin{array}{ll}
 \textrm{if } i \le N & zeros[i] = \sigma + I \cdot \omega \\
 \textrm{else} & poles[i] = \sigma + I \cdot \omega \end{array} \right. \f$ \n
 * \arg for  \f$ i=2 \cdot N-1,\dots,0 \f$ \n
 * \f$ \left\{ \begin{array}{ll} \textrm{if } i \le N -1 & (X,Y)=(real(zeros[i]),imag(zeros[i])) \\
 else & (X,Y)=(real(poles[i]),imag(poles[i])) \end{array} \right. \f$ \n
 * \f$    Re = \frac{1-X^2-Y^2}{(1-X)^2+Y^2} \f$ \n
 * \f$    V = \frac{2 \cdot Y}{(1-X)^2+Y^2} \f$ \n
 * \f$    c1 = -2 \cdot Re \f$ \n
 * \f$    d1 = Re^2 + V^2 \f$ \n
 * \f$ \left\{ \begin{array}{ll}
 \textrm{if } i \le N -1 & \left\{ \begin{array}{l}
zeros[i]=Re+I \cdot V \\ CoefB[i]=c1 \\ CoefA[i]=d1  \end{array} \right. \\
 else & \left\{ \begin{array}{l}
 poles[i-N]=Re+I \cdot V  \\  CoefD[i-N]=c1 \\ CoefC[i-N]=d1 \end{array} \right.
 \end{array} \right. \f$ \n
 */
void elli(double eps,      // Oscillations in working bandwidth
	  double A,        // Weakening of attenuated band
	  double fa,       // Low frequency transition edge  [Hz]
	  double fb,       // High frequency transition edge [Hz]
	  double fe,       // Sampling frequency  [Hz]
	  int NCellMax,         // Maximum number of cells
	  int *NCells,          // Output number of cells
	  complex<double> poles[], // Poles of the cells (imaginary part >= 0) 
	  complex<double> zeros[], // Zeros of the cells (imaginary part >= 0)
	  double CoefA[],  // A coef from H(Z) = (1+BZ-1+AZ-2) / (1+DZ-1+BZ-2)
	  double CoefB[],  // B coef from H(Z) = (1+BZ-1+AZ-2) / (1+DZ-1+BZ-2)
	  double CoefC[],  // C coef from H(Z) = (1+BZ-1+AZ-2) / (1+DZ-1+BZ-2)
	  double CoefD[]   // D coef from H(Z) = (1+BZ-1+AZ-2) / (1+DZ-1+BZ-2)
	  ) {

  double *C, *D;
  double T, dk1, dk, dkp, ak1, ak2, ak3, ak4;
  double U0, xmag, Q, R, sigma;
  double a1, b1, dn, de, omega;
  double X, Y, Re, V, c1, d1;
  double wr, wc;
  int i, j, N, NDeu;
  complex<double> p;
  
  wc = fb*2*M_PI;
  wr = fa*2*M_PI;

  T = 1/fe;
  dk1 = eps/sqrt(A*A-1);
  dk = tan(wc*T/2.) / tan(wr*T/2.);
  dkp = sqrt(1-dk*dk);
  ak1 = ak(dk);
  ak2 = ak(dk1);
  ak3 = ak(dkp);
  //ak4 = ak(sqrt(1-dk1*dk1));
  ak4 = cak(dk1*dk1);

  N = (int)(ak4*ak1/(ak2*ak3));
  N = (N/2) + 1;
  NDeu = 2*N;

  *NCells = N;
  if ( N<0 || N>NCellMax) {
    fprintf(stderr,"\n\n ***  Maximum number of cells (%d) reached. %d cells needed. Exiting ... ***\n\n",NCellMax,N);
    exit(1);
    return;
  }
  C = (double*)malloc(NDeu*sizeof(double));
  D = (double*)malloc(NDeu*sizeof(double));

  U0 = (-ak3/ak4)*alog((1+sqrtl(1+eps*eps))/eps);
  for (i=1;i<=N;i++) {
    xmag = (2*i-1)*ak1/NDeu;
    zeros[i-1] = -ak3 + Im*xmag;
    poles[i-1] =   U0 + Im*xmag;
  }

  for (i=1; i<=NDeu; i++) {
    if (i<=N) {
      Q = real(zeros[i-1]);
      R = imag(zeros[i-1]);
    }
    else {
      Q = real(poles[i-N-1]);
      R = imag(poles[i-N-1]);
    }

    sigma = 0.;
    a1 = sn(Q, dkp, ak3, ak1);
    b1 = sn(R, dk,  ak1, ak3);

    dn = sqrt(1.-(dk*b1)*(dk*b1));
    de = 1-(a1*dn)*(a1*dn);
    
    if ( i > N ) {
      sigma = a1*sqrt((1-a1*a1)*(1-b1*b1))*dn/de;
    }
    
    omega = b1*sqrt(1-(dkp*a1)*(dkp*a1))/de;

    C[i-1] = -2*sigma*wc;
    D[i-1] = (sigma*sigma + omega*omega)*wc*wc;
    sigma = sigma*tan(wc*T/2.);
    omega = omega*tan(wc*T/2.);
    
    if ( i <= N ) {
      zeros[i-1] = sigma + Im*omega;
    }
    else {
      poles[i-N-1] = sigma + Im*omega;
    }
  }

  for (i=NDeu; i>=1; i--) {
    j = i;
    
    if ( j > NDeu/2 ) j = j - NDeu/2;
    
    
    if ( i<=N ) {
      X = real(zeros[i-1]);
      X = 0;
      Y = imag(zeros[i-1]);
    }
    else {
      X = real(poles[i-N-1]);
      Y = imag(poles[i-N-1]);
    }
    
    Re = (1-X*X-Y*Y)/((1-X)*(1-X)+Y*Y);
    V = 2*Y/((1-X)*(1-X)+Y*Y);
    c1 = -2*Re;
    d1 = Re*Re + V*V;
    if ( i <= N ) {
      zeros[i-1] = Re + Im*V;
      CoefB[i-1] = c1;
      CoefA[i-1] = d1;
    }
    else {
      poles[i-N-1] = Re + Im*V;
      CoefD[i-N-1] = c1;
      CoefC[i-N-1] = d1;
    }

  }


  free(C);
  free(D);

  return;
}
/*******************************************************/



// Auxiliary functions for elliptic filter computation
/*******************************************************/
/*! \brief Integral filter parameter computation.
 *
 * #cak function is called.
 * \f[ \textrm{returned value}=cak(1-y^2) \f]
 */
double ak(double y) {
	double x;
	
	x = (1-y)*(1+y);
	
	return cak(x);
}


// return ak(1-y*y);
/*! \brief Developped filter parameter computation.
 *
 * Numerical problems that could occur when x is near to zero are avoided.
 *
 * \f[ \textrm{returned value}=  1.38629436112 
 + 0.09666344259 \cdot x + 0.03590092383 \cdot x^2 + 0.0374256371 \cdot x^3 + 0.01451196212 \cdot x^4 \f]
 * \f[ - alog(x) \cdot (0.5 + 0.12498593597 \cdot x  + 0.068802485763 \cdot x^2  + 0.03328355346 \cdot x^3 + 0.004417870122 \cdot x^4) \f]
 */
double cak(double x) {
	double res;
	
	
	res = 
		1.38629436112 
		+ x*(0.09666344259 
			 + x*(0.03590092383 
				  + x*(0.0374256371 
					   + x*0.01451196212)))
		- alog(x)*(0.5
				   + x*(0.12498593597 
						+ x*(0.06880248576 
							 + x*(0.03328355346 
								  + x*0.00441787012))));
	
	/*   res =  */
	/*     1.38629436112  */
	/*     + 0.09666344259*x + 0.03590092383*x*x + 0.03742563713*x*x*x + 0.01451196212*x*x*x*x - alog(x)*(0.5+0.12498593597*x + 0.06880248576*x*x +  */
	/* 	     0.03328355346*x*x*x + 0.00441787012*x*x*x*x); */
	
	return res;
}




/*! \brief Recursive or direct coefficients computation.
 *
 * Inputs are:
 * \arg y
 * \arg A 
 * \arg ak1
 * \arg ak3
 *
 * Computations :
 * \f[ ns = \sqrt{\frac{50 \cdot ak1}{\pi \cdot ak3}}+2 \f]
 * \f[ x = \frac{y}{2 \cdot ak1} \f]
 * \f[ q = e^{-\frac{\pi \cdot ak3}{ak1}} \f]
 * \f[ \textrm{returned value} = 2 \cdot \frac{ q^\frac{1}{4} \cdot sin(\pi \cdot x) +
 \sum_{N=1}^{ns} {(  (-1)^N \cdot q^{(N+\frac{1}{2})^2} \cdot sin((2 \cdot N+1) \cdot \pi \cdot x) )}}
 {\big( 1+ 2 \cdot \sum_{N=1}^{ns} {( (-1)^N \cdot q^{N^2} \cdot cos(2 \cdot N \cdot \pi \cdot x) )} \big)\cdot \sqrt{A}} \f]
 */
double sn(double y, double A, double ak1, double ak3) {
  double ns, res;
  double x, q, sup, den;
  int i, N;

  ns = sqrt(50.*ak1/(M_PI*ak3))+2;
  x = y*0.5/ak1;
  q = exp(-M_PI*ak3/ak1);
  sup = 2.*(pow(q,0.25))*sin(M_PI*x);
  den = 1.;
  i = -2;
  N = 1;

  do {
    sup = sup + i*(pow(q,(N+0.5)*(N+0.5)))*sin((2*N+1)*M_PI*x);
    den = den + i*(pow(q,N*N))*cos(2*N*M_PI*x);
    i = -i;
    N = N+1;
  } while (N <= ns);

  res = sup/(den*sqrt(A));

  return res;
}
/**********************************************************/


// Filtering step with a elliptic cell
/**********************************************************/
/*! \brief Elliptic cell filtering step, depending on xn and Cell (type QuadCell) inputs.
 *
 * Computations :
 * \f[ u = x_n  -b2_{Cell} \cdot u_{Cell}[1] -  -b1_{Cell} \cdot u_{Cell}[0] \f]
 * \f[ \textrm{returned value}= a0_{Cell} \cdot (u + a1_{Cell} \cdot u_{Cell}[0] + u_{Cell}[1]) \f]
 * Cell u memory attribute is updated :
 * \f[ \textrm{new } u_{Cell}= \left( \begin{array}{l} u \\ \textrm{old } u_{Cell}[0]  \end{array} \right)\f]
 */
double FilterQuadCell(double xn, QuadCell *Cell) {
  double yn, u;
  
  u = -Cell->b2*(Cell->u)[1] - Cell->b1*(Cell->u)[0] + xn;
  yn = 
    Cell->a0*(u +
	      Cell->a1*(Cell->u)[0] +
	      (Cell->u)[1]);

  (Cell->u)[1] = (Cell->u)[0];
  (Cell->u)[0] = u;
  return yn;
}
/**********************************************************/

// Filtering a chain of elliptic cells
/**********************************************************/
/*! \brief Elliptic cells chain filtering step, depending on xn, number of cells NCells and Cell (type QuadCell) inputs.
 *
 * For each cell, #FilterQuadCell function is called.\n
 * Cells are updated and returned value depends on last cell.
 */
double FilterQuadCellChain(double xn, int NCells, QuadCell Cell[]) {

  double yn;
  int i;

  yn = xn;
  for (i=0;i<NCells;i++) 
    yn = FilterQuadCell(yn, Cell+i);

  return yn;
}
/**********************************************************/

// Z transform of an elliptic cell
/**********************************************************/
/*! \brief Elliptic cell Z transform.
 *
 * \f[ \textrm{returned value} =   \frac{a0_{Cell} \cdot \frac{a1_{Cell} + \frac{1}{Z}}{Z}}{1+\frac{b1_{Cell}+\frac{b2_{Cell}{Z}}{Z}}{Z}} \f]
 */
complex<double> TransfZQuadCell(complex<double> Z, QuadCell Cell) {
  
  complex<double> num;
  complex<double> den;
  
  num = Cell.a0*(1.0 + (Cell.a1 + 1.0/Z)/Z);
  den = 1.0 + (Cell.b1 + Cell.b2/Z)/Z;

  return num/den;
}
/**********************************************************/

// Z transform of a chain of elliptic cells
/**********************************************************/
/*! \brief  Elliptic cells chain  Z transform, depending on Z, number of cells NCells and Cell (type QuadCell) inputs.
 *
 * For each cell, #TransfZQuadCell function is called.\n
 * Returned value is product of all cells Z transform.
 */
complex<double> TransfZQuadCellChain(complex<double> Z, int NCells, QuadCell Cell[]) {
  complex<double> res;

  int i;
  res = 1.0;
  for (i=0;i<NCells;i++) 
    res *= TransfZQuadCell(Z,Cell[i]);

  return res;
}
/**********************************************************/

// Modulus of the frequency response for an elliptic cell
// f = reduced frequency (=f/fsampling)
/**********************************************************/
/*! \brief  Frequency response modulus, depending on f frequency and Cell (type QuadCell) inputs.
 *
 * \f[  \textrm{returned value} = \sqrt{\frac{ {(a0_{Cell} \cdot (a1_{Cell} + 2 \cdot cos ( 2 \cdot \pi \cdot f)))}^2 }
 {1+b1_{Cell}^2+b2_{Cell}^2 + 2 \cdot b1_{Cell} \cdot b2_{Cell} \cdot cos ( 2 \cdot \pi \cdot f) + 2 \cdot b2_{Cell} \cdot cos ( 4 \cdot \pi \cdot f)}} \f]
 */
double AbsRespFunctQuadCell(double f, QuadCell Cell) {
  double w, cw;
  double res, arg;

  w = 2*M_PI*f;
  cw = cos(w);
  arg = Cell.a0*(Cell.a1+2*cw);
  res = 
    arg*arg / 
    (1 + Cell.b1*Cell.b1 + Cell.b2*Cell.b2 + 2*Cell.b1*(1.+Cell.b2)*cw + 2*Cell.b2*cos(2*w));

  return sqrt(res);
}
/**********************************************************/


// Modulus of the frequency response for a chain of elliptic cells
// f = reduced frequency (=f/fsampling)
/**********************************************************/
/*! \brief  Elliptic cells chain  frequency response modulus, depending on f frequency, number of cell NCells and Cell (type QuadCell) inputs.
 *
 * For each cell, #AbsRespFunctQuadCell function is called.\n
 * Returned value is product of all cells frequency response modulus.
 */
double AbsRespFunctQuadCellChain(double f, int NCells, QuadCell Cell[]) {
  double res;
  int i;

  res = 1.0;
  for (i=0;i<NCells;i++) 
    res *= AbsRespFunctQuadCell(f,Cell[i]);

  return res;
}
/**********************************************************/


// Returns max |1/D(w)|, where D(w) is the denominator of 
// an ellitpic cell
/***********************************************************/
/*! \brief Returns max |1/D(w)|, where D(w) is the denominator of an ellitpic cell (type QuadCell).
 *
 * \f[  \textrm{returned value} = \frac{1}
   {(1 - b2_{Cell}) \cdot \sqrt{1- \frac{{b1_{Cell}}^2}{4 \cdot b2_{Cell}}}} \f]
*/
double HmQuadCell(QuadCell Cell) {
  double res;

  res = (1-Cell.b2)*sqrt(1-Cell.b1*Cell.b1/(4*Cell.b2));

  return 1./res;
}
/***********************************************************/


// Returns max|Ell(w)|, where Ell(w) is the freq response of 
// an elliptic cell
/***********************************************************/
/*! \brief Returns max|Ell(w)|, where Ell(w) is the frequency response of an ellitpic cell (type QuadCell).
 *
 * #HmQuadCell is called and
 * \f$  \textrm{returned value} = 
   a0_{Cell} \cdot (a1_{Cell} - b1_{Cell} \cdot \frac{1+b2_{Cell}}{2 \cdot+b2_{Cell}}) \cdot HmQuadCell(Cell)\f$
*/
double KmQuadCell(QuadCell Cell) {
  double res;

  res = Cell.a0*(Cell.a1 - Cell.b1*(1+Cell.b2)/(2*Cell.b2))*HmQuadCell(Cell);

  return res;
}
/***********************************************************/


// Matches nearest poles for a chain af elliptic cells
/***********************************************************/
/*! \brief Matches nearest poles for a chain of elliptic cells.
 *
 * Elliptic cells chain is defined by number of cells NCells and Cell (type QuadCell) inputs.\n
 * First, cells are ordered according to the distance between the pole and the unity circle.\n
 * For all cells (i index) the nearest zero (jmin index) corresponding to a pole is found by minimizing
 * \f$  dmin = min_{j=i}^{NCells}{(zero_{Cell[j]} - pole_{Cell[i]})}\f$.
 * \f$ \textrm{If } i \ne jmin\f$, 
 * a0, a1 and zero attributes of Cell[i] and Cell[jmin] are inverted.
*/
void PoleMatching(int NCells, QuadCell Cell[]) {

  int i, j, k, jmin;
  double dmin, dcur;
  double tmpd;
  complex<double> tmpc;
  int *index;
  QuadCell *TmpCells;


  // Orders according to the distance of the pole to the unity circle
  /*********************************************************************/
  index = (int*)malloc(NCells*sizeof(int));
  TmpCells = (QuadCell*)malloc(NCells*sizeof(QuadCell));
  for (i=0; i<NCells; i++) index[i] = i;

  for (i=0; i<NCells; i++) {
    for (j=i+1;j<NCells;j++) {
      if ((Cell[index[i]]).b2 < (Cell[index[j]]).b2 ) {
	k = index[j];
	index[j] = index[i];
	index[i] = k;
	i--;
	break;
      }
    }
  }
    
  for (i=0;i<NCells;i++) 
    TmpCells[i] = Cell[index[i]];

  for (i=0;i<NCells;i++) 
    Cell[i] = TmpCells[i];
  /*********************************************************************/
  

  // Find the nearest zero corresponding to a pole
  /*********************************************************************/
  for (i=0; i<NCells; i++) {

    dmin = abs(Cell[i].zero - Cell[i].pole);
    jmin = i;

    // Find the nearest zero
    for (j=i+1; j<NCells; j++) {
      dcur = abs(Cell[j].zero - Cell[i].pole);
      if (dcur < dmin) {
	dmin = dcur;
	jmin = j;
      }
    }

    if (jmin != i) {
      tmpd = Cell[jmin].a0;
      Cell[jmin].a0 = Cell[i].a0;
      Cell[i].a0 = tmpd;

      tmpd = Cell[jmin].a1;
      Cell[jmin].a1 = Cell[i].a1;
      Cell[i].a1 = tmpd;


      tmpc = Cell[jmin].zero;
      Cell[jmin].zero = Cell[i].zero;
      Cell[i].zero = tmpc;
    }
  }

  free(index);
  free(TmpCells);
}
/***********************************************************/


// Orders Cells according to the the max of inf norm
/***********************************************************/
/*! \brief Orders cells according to the the max of inf norm.
 *
 * Elliptic cells chain is defined by number of cells NCells and Cell (type QuadCell) inputs.\n
 * First, #KmQuadCell function is called for each cell.\n
 * Then, cells are ordered according to the the max of inf norm.
 */
void OrderCellMaxNorm(int NCells, QuadCell Cell[]) {
  double *Km;
  //double KmMax;
  int i, j, k;
  int *index;
  QuadCell *TmpCells;
  
  Km = (double*)malloc(NCells*sizeof(double));
  TmpCells = (QuadCell*)malloc(NCells*sizeof(QuadCell));
  index = (int*)malloc(NCells*sizeof(int));

  for (i=0;i<NCells;i++) {
    Km[i] = KmQuadCell(Cell[i]);
    index[i] = i;
  }
  
  for (i=0;i<NCells;i++) {
    for (j=i+1;j<NCells;j++) {
      if (Km[index[i]] < Km[index[j]]) {
	k = index[j];
	index[j] = index[i];
	index[i] = k;
	i--;
	break;
      }
    }
  }

  for (i=0;i<NCells;i++) 
    TmpCells[i] = Cell[index[i]];

  for (i=0;i<NCells;i++) 
    Cell[i] = TmpCells[i];

  free(Km);
  free(index);
  free(TmpCells);

  return;
}
  
/***********************************************************/


// Compute Scaling factors
// returns the global factor
/***********************************************************/
/*! \brief Scale factors computation for an elliptic cells chain :
 * a0 attributes are updated and global factor is returned.
 *
 * Elliptic cells chain is defined by number of cells NCells and Cell (type QuadCell) inputs.\n
 * #HmQuadCell and #KmQuadCell functions are called for each cell. Results are : \f$ Hm_i\f$, \f$ Km_i \f$. \n
 * \f[   Hm_i = HmQuadCell(Cell[i]) \f]
 * \f[   Km_i = KmQuadCell(Cell[i]) \f]
 * 
 * For all cells execpt the last one (index i=0,...,NCells-2) :
 * \f[   ac_i = \frac{1}{a1_{Cell[i]}} - (b1_{Cell[i]} \cdot \frac{1+b2_{Cell[i]}}{2 \cdot b2_{Cell[i]}}) \cdot Hm_{i+1} \f]
 * \f[   n_i = ceil(\frac{log(ac_i)}{log(2)+\frac{1}{2}} \f]
 * \f[   a0_{Cell[i]} = 2^{n_i} \f]
 *
 * For last cell:
 * \f[ norm = \frac{1}{Hm_0} \cdot \prod_{i=0}^{NCells-1}{\big( a0_{Cell[i]} \cdot \frac{2+a1_{Cell[i]}}{1+b1_{Cell[i]}+b2_{Cell[i]}} \big)} \f]
 * \f[   a0_{Cell[NCells-1]} = \frac{1}{norm} \f]
 *
 * \f[  \textrm{returned value : } ag= \frac{1}{Hm_0} \f]
 * 
 */
double CalcScalingFact(int NCells, QuadCell Cell[]) {
  double ag, ac, norm, *Hm, *Km;
  int i, n;

  Hm = (double*)malloc(NCells*sizeof(double));
  Km = (double*)malloc(NCells*sizeof(double));
  

  
  for (i=0;i<NCells;i++) {
    Hm[i] = HmQuadCell(Cell[i]);
    Km[i] = KmQuadCell(Cell[i]);
  }

  ag = 1./Hm[0];
  norm = ag;

  for (i=0;i<NCells-1;i++) {
    ac = 1.0 / (((Cell[i]).a1 - (Cell[i]).b1*(1+(Cell[i]).b2)/(2*(Cell[i]).b2))*Hm[i+1]);
    n = (int)(log(ac)/log(2.0)+0.5);
    //(Cell[i]).a0 = (double)(1<<n); //pow(2,n);
	(Cell[i]).a0 = pow(2.0,(double)(n));
    norm *= (Cell[i]).a0*(2+(Cell[i]).a1)/(1+(Cell[i]).b1+(Cell[i]).b2);
  }

  norm *= (2+(Cell[i]).a1)/(1+(Cell[i]).b1+(Cell[i]).b2);
  (Cell[i]).a0 = 1./norm;
  
  free(Hm);
  free(Km);

  return ag;
}
/***********************************************************/


// Calculate filter coefficients from user specifications
// return the global scaling factor
/***********************************************************/
/*! \brief Computes filter coefficients from user specifications and returns the global scale factor.
 *
 * Inputs are:
 * \arg fe :  sampling frequency [Hz]
 * \arg at :  attenuation [dB]
 * \arg bp :  oscillations in bandwidth [dB]
 * \arg fb :  low transition frequency [Hz]
 * \arg fa :  high transition frequency [Hz]
 * \arg NCellMax : maximum number of cells
 *
 * Outputs are :
 * \arg FilterCellsOut : cells
 * \arg NCellsOut : number of cells
 *
 * #elli function is called and its outputs are NCellsOut, Poles, Zeros, CoefA, CoefB, CoefC, CoefD.\n
 * For all cells (index i=0,...,NCells-1), FilterCellsOut[i] are filled :
 * \f[   u = (0,0) \f]
 * \f[   a0 = 1 \f]
 * \f[   a1 = CoefB[i] \f]
 * \f[   b1 = CoefD[i] \f]
 * \f[   b2 = CoefC[i] \f]
 * \f[   zero = Zeros[i] \f]
 * \f[   pole = Poles[i] \f]
 * #PoleMatching, then #OrderCellMaxNorm and #CalcScalingFact functions are called using NCells and FilterCells arguments.\n
 * #CalcScalingFact result is returned.
 */
double CalcEllipticFilter(double fe,    // sampling frequency [Hz]
			  double at,    // attenuation [dB]
			  double bp,    // oscillations in bandwidth [dB]
			  double fb,    // low transition frequency [Hz]
			  double fa,    // high transition frequency [Hz]
			  int NCellMax, // maximum number of cells
			  QuadCell **FilterCellsOut,  // Output cells
			  int *NCellsOut   // output number of cells
			  ) {
  
  double A, T, eps;
  double wc,wr;
  double *CoefA, *CoefB;
  double *CoefC, *CoefD;
  complex<double> *Poles, *Zeros;
  int NCells;
  QuadCell *FilterCells;
  double ag;
  int i;


  
  eps = sqrtl(exp(log(10.)*bp/10.)-1.);
  A = exp(log(10.)*at/20.);
  T = 1./fe;

  wc = fb*2*M_PI;
  wr = fa*2*M_PI;

  CoefA = (double*)malloc(NCellMax * sizeof(double));
  CoefB = (double*)malloc(NCellMax * sizeof(double));
  CoefC = (double*)malloc(NCellMax * sizeof(double));
  CoefD = (double*)malloc(NCellMax * sizeof(double));
  Poles = (complex<double>*)malloc(NCellMax * sizeof(complex<double>));
  Zeros = (complex<double>*)malloc(NCellMax * sizeof(complex<double>));
  
  elli(eps,A,fa,fb,fe,
       NCellMax,
       &NCells,
       Poles, Zeros,
       CoefA, CoefB, CoefC, CoefD);
  *NCellsOut = NCells;

  
  FilterCells = (QuadCell*)malloc(NCells*sizeof(QuadCell));
  *FilterCellsOut = FilterCells;
  for (i=0;i<NCells;i++) {
    (FilterCells[i]).u[0] = (FilterCells[i]).u[1] = 0;
    
    (FilterCells[i]).a0 = 1.0;
    (FilterCells[i]).a1 = CoefB[i];

    (FilterCells[i]).b1 = CoefD[i];
    (FilterCells[i]).b2 = CoefC[i];

    (FilterCells[i]).zero = Zeros[i];
    (FilterCells[i]).pole = Poles[i];
  }
  
  /*
   for (i=0;i<NCells;i++) {
	  cout << "  Cells " << i << " : (FilterCells[i]).a0   = " << (FilterCells[i]).a0 << endl;
	  cout << "  Cells " << i << " : (FilterCells[i]).a1   = " << (FilterCells[i]).a1 << endl;
	  cout << "  Cells " << i << " : (FilterCells[i]).b1   = " << (FilterCells[i]).b1 << endl;
	  cout << "  Cells " << i << " : (FilterCells[i]).b2   = " << (FilterCells[i]).b2 << endl;
	  cout << "  Cells " << i << " : (FilterCells[i]).zero = " << (FilterCells[i]).zero << endl;
	  cout << "  Cells " << i << " : (FilterCells[i]).pole = " << (FilterCells[i]).pole << endl;
  }
   */
  
  PoleMatching(NCells,FilterCells);
  OrderCellMaxNorm(NCells,FilterCells);
  ag = CalcScalingFact(NCells,FilterCells);

  free(CoefA);
  free(CoefB);
  free(CoefC);
  free(CoefD);
  free(Poles);
  free(Zeros);

  return ag;
}
/***********************************************************/



// Calculate filter coefficients from user specifications
// For use with LISA Code
// include the global scaling factor in the first cell
/***********************************************************/
/*! \brief Computes filter coefficients from LISA Code user specifications.\n
 * The  global scale factor is included in the first cell.
 *
 * Inputs are:
 * \arg fe :  sampling frequency [Hz]
 * \arg at :  attenuation [dB]
 * \arg bp :  oscillations in bandwidth [dB]
 * \arg fb :  low transition frequency [Hz]
 * \arg fa :  high transition frequency [Hz]
 * \arg NCellMax : maximum number of cells
 *
 * Outputs are :
 * \arg CellsCoef : cells coefficients
 * \arg NCellsOut : number of cells
 *
 * #CalcEllipticFilter function is called : its outputs are NCellsOut, FilterCells, and its returned value is global scaling factor ag.\n
 * For all cells (index i=0,...,NCellsOut-1) :
 * \f[   u_{FilterCellsOut[i]} = (0,0) \f]
 * \f[    CellsCoef[i][0] = b1_{FilterCells[i]} \f]
 * \f[    CellsCoef[i][1] = b2_{FilterCells[i]} \f]
 * \f[    CellsCoef[i][2] = a0_{FilterCells[i]} \f]
 * \f[    CellsCoef[i][3] = a0_{FilterCells[i]} \cdot a1_{FilterCells[i]} \f]
 * \f[    CellsCoef[i][4] = a0_{FilterCells[i]} \f]
 * First cell is rescaled :
 * \f[    CellsCoef[0][2] = ag \cdot a0_{FilterCells[i]} \f]
 * \f[    CellsCoef[0][3] = ag \cdot a0_{FilterCells[i]} \cdot a1_{FilterCells[i]} \f]
 * \f[    CellsCoef[0][4] = ag \cdot a0_{FilterCells[i]} \f]
 */
void CalcEllipticFilter4LISACode(double fe,    // sampling frequency [Hz]
				 double at,    // attenuation [dB]
				 double bp,    // oscillations in bandwidth [dB]
				 double fb,    // low transition frequency [Hz]
				 double fa,    // high transition frequency [Hz]
				 int NCellMax, // maximum number of cells
				 double CellsCoef[][5], // Cells coefficients
				 int *NCellsOut   // output number of cells
				 ) {

  QuadCell *FilterCells;
  double ag;
  int i;
  
  ag = CalcEllipticFilter(fe, at, bp, fb, fa,
			  NCellMax, &FilterCells, NCellsOut);

  for (i=0; i<*NCellsOut; i++) {
    CellsCoef[i][0] = FilterCells[i].b1;
    CellsCoef[i][1] = FilterCells[i].b2;
    CellsCoef[i][2] = FilterCells[i].a0;
    CellsCoef[i][3] = FilterCells[i].a0*FilterCells[i].a1;
    CellsCoef[i][4] = FilterCells[i].a0;
  }

  // Rescale first Cell
  CellsCoef[0][2] *= ag;
  CellsCoef[0][3] *= ag;
  CellsCoef[0][4] *= ag;

  // free memory
  free(FilterCells);


  return;
}
/**********************************************************/
