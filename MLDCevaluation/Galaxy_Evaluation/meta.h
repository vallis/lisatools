#define d 15
#define dd 4
#define da 5
#define Tobs 31454728.0  // I think this was 10 yrs.

// Cash-Karp Runge-Kutta parameters:

#define A2 (1./5.)
#define A3 (3./10.)
#define A4 (3./5.)
#define A5 1.
#define A6 (7./8.)

#define B21 (1./5.)
#define B31 (3./40.)
#define B32 (9./40.)
#define B41 (3./10.)
#define B42 (-9./10.)
#define B43 (6./5.)
#define B51 (-11./54.)
#define B52 (5./2.)
#define B53 (-70./27.)
#define B54 (35./27.)
#define B61 (1631./55296.)
#define B62 (175./512.)
#define B63 (575./13824.)
#define B64 (44275./110592.)
#define B65 (253./4096.)

#define C1 (37./378.)
#define C2 0.
#define C3 (250./621.)
#define C4 (125./594.)
#define C5 0.
#define C6 (512./1771.)

#define D1 (2825./27648.)
#define D2 0.
#define D3 (18575./48384.)
#define D4 (13525./55296.)
#define D5 (277./14336.)
#define D6 (1./4.)

#define CLIGHT 299792458.   // meters/sec
#define RSUN 1.476625e3     // meters
#define TSUN (RSUN/CLIGHT)  // seconds
//#define AU 1.49597871e11    // meters

#define GPCTOMETERS 3.085677581e25

#define RK_H 100000
#define RK_EPS .00000001

// Are we on the last step of an update?

#define ENDYES 1
#define ENDNO 2

// Series of nasty constants that appear in the PN formulae.
#define PN1 (743./336.)
#define PN21 (3058673./1016064.)
#define PN22 (5429./1008.)
#define PN23 (617./144.)
