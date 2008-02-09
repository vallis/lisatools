// $Id: LISACODE-TrFctGW.cpp,v 1.8 2007/04/05 09:33:12 ruiloba Exp $
/*
 *  LISACODE-TrFctGW.cpp
 *  V 1.4
 *  LISACode
 *
 *  Created on 21/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 18/05/06 by A.Petiteau, H.Halloin 
 *
 */

#include "LISACODE-TrFctGW.h"
#include <fstream.h>

/* Constructor */
/*! \brief Constructs an instance and initializes it with default values.
 *
 * init method is called 
 * 
 */
TrFctGW::TrFctGW ()
{
	Geometry LISAGeo_n;
	vector<GW *> GWSources_n(1);
	GWSources_n[0] = new GW;
	init(& GWSources_n, & LISAGeo_n);
}


//Takes as arguments:
// the location of the TrFctGW in the barycentric system (angles theta, phi)
// computes the unit vector w connecting the Sun to the TrFctGW and the unit transverse vectors u and v
/*! \brief Constructs an instance and initializes it with default values.
 *
 * init method is called 
 * 
 */
TrFctGW::TrFctGW (vector<GW *> * GWSources_n, Geometry * LISAGeo_n)
{
	init(GWSources_n, LISAGeo_n);
}


/* Destructor. */
TrFctGW::~TrFctGW ()
{
}


/* Access methods */
/*! \brief Initializes an instance with default values and inputs.
 *
 * #GWSources attribute = GWSources_n input\n
 * #LISAGeo attribute = LISAGeo_n input\n
 * size of #u, #v, and #k attributes is set to GWSources_n input size\n
 * \arg For each source in #GWSources (GW index) :\n
 * (k,u,v) is direct,\n
 * the ecliptic latitude is \f$ \beta=\Pi-\theta \f$ and the ecliptic longitude \f$ \phi=\lambda \f$ .\n
 * \f[ \lambda = \lambda_{GW}+\pi \f]
 * \f[ \beta = -\beta_{GW}+\pi \f]
 * \f[ \psi = AnglPol_{GW} \f]
 * \f[ k =  \left( \begin{array}{l}
 cos(\lambda) \cdot cos(\beta) \\
 sin(\lambda) \cdot cos(\beta) \\
 sin(\beta)  \end{array} \right) \f]
 * \f[ u =  \left( \begin{array}{l}
 sin(\beta) \cdot cos(\lambda) \cdot cos(\psi) + sin(\lambda) \cdot sin(\psi) \\
 sin(\beta) \cdot sin(\lambda) \cdot cos(\psi) - cos(\lambda) \cdot cos(\psi) \\
 - cos(\beta) \cdot cos(\psi)  \end{array} \right) \f]
 * \f[ v=  \left( \begin{array}{l} 
 sin(\beta) \cdot cos(\lambda) \cdot sin(\psi) - sin(\lambda) \cdot cos(\psi) \\
 sin(\beta) \cdot sin(\lambda) \cdot sin(\psi) + cos(\lambda) \cdot cos(\psi) \\
 - cos(\beta) \cdot sin(\psi)  \end{array} \right) \f]
 */
void TrFctGW::init(vector<GW *> * GWSources_n, Geometry * LISAGeo_n)
{
	GWSources = GWSources_n;
	u.resize(GWSources->size());
	v.resize(GWSources->size());
	k.resize(GWSources->size());
	LISAGeo = LISAGeo_n;
	double cl,sl,cb,sb,cp,sp;
	double lambdaPropDir, betaPropDir, psi; // LambdaPropDir and BetaPropDir are the ecliptic coordinate for the propagation direction
	for(int iGW=0; iGW<GWSources->size(); iGW++){
		//lambdaPropDir = (*GWSources)[iGW]->getLambda();  // Michele
		//betaPropDir = (*GWSources)[iGW]->getBeta();  // Michele
		lambdaPropDir = (*GWSources)[iGW]->getLambda()+M_PI; // OK 
		betaPropDir = -1.0*(*GWSources)[iGW]->getBeta(); // OK
		psi = (*GWSources)[iGW]->getAnglPol();
		cl=cos(lambdaPropDir);
		sl=sin(lambdaPropDir);
		cb=cos(betaPropDir); 
		sb=sin(betaPropDir);
		cp=cos(psi); 
		sp=sin(psi);
		
		// u,v, is direct
		// the ecliptic latitude is beta=Pi-theta and the ecliptic longitude phi=lambda
		
		k[iGW].p[0] = cl * cb; // OK
		k[iGW].p[1] = sl * cb; // OK
		k[iGW].p[2] = sb; // OK
		
		//k[iGW].p[0] = -cl * cb;  // Michele
		//k[iGW].p[1] = -sl * cb;  // Michele
		//k[iGW].p[2] = -sb;  // Michele
		
		u[iGW].p[0] = sb*cl*cp + sl*sp;
		u[iGW].p[1] = sb*sl*cp - cl*sp;
		u[iGW].p[2] = -cb*cp;
		
		v[iGW].p[0] = sb*cl*sp - sl*cp;
		v[iGW].p[1] = sb*sl*sp + cl*cp;
		v[iGW].p[2] = -cb*sp;
		
		
		//cout << "In GW constructor :" <<endl;
		//cout << "  - k = " << k.p[0] << " " << k.p[1] << " " << k.p[2] << " " << endl;
		//cout << "  - u = " << u.p[0] << " " << u.p[1] << " " << u.p[2] << " " << endl;
		//cout << "  - v = " << v.p[0] << " " << v.p[1] << " " << v.p[2] << " " << endl;	
	}
}


/* Others methods */
// Retuns the frequency fluctuation due to GW
// Takes as argument the link and the TrFctGW structure
// See formula given by gr-qc/01112059 at the bottom of p13

/*! \brief Returns the fluctuation frequency due to GW.
 *
 * \f[ tem = trec + LISAGeo->gtdelay(em, rec, order, trec) \f]
 * \f[ \overrightarrow{rrec} = \frac{LISAGeo->gposition(rec, trec)}{C}  \f]
 * \f[ \overrightarrow{rem} = LISAGeo->gposition(em, trec) \f]
 * \f[ \overrightarrow{dr} = \overrightarrow{rrec}-\overrightarrow{rem} \f]
 * \f[ \overrightarrow{n} = \frac{\overrightarrow{dr}}{\big\|\overrightarrow{dr}\big\|} \f]
 * \f[ \delta_\nu=0 \f]
 * For each source (iGWindex) in #GWSources, \n 
 * \f[ hpr = (*GWSources)[iGW]->hp(trec- \overrightarrow{k[iGW]} \cdot \overrightarrow{rrec}) \f] 
 * \f[ hpe = (*GWSources)[iGW]->hp(tem- \overrightarrow{k[iGW]} \cdot \overrightarrow{rem}) \f] 
 * \f[ hcr = (*GWSources)[iGW]->hc(trec- \overrightarrow{k[iGW]} \cdot \overrightarrow{rrec}) \f] 
 * \f[ hce = (*GWSources)[iGW]->hc(tem- \overrightarrow{k[iGW]} \cdot \overrightarrow{rem}) \f]    
 * \f[ \delta_\nu = \delta_\nu + 
 * \frac{(hpe-hpr) \cdot \frac{{(\overrightarrow{u[iGW]} \cdot \overrightarrow{n})}^2-{(\overrightarrow{v[iGW]} \cdot \overrightarrow{n})}^2}{2}+(hce-hcr) \cdot (\overrightarrow{u[iGW]} \cdot \overrightarrow{n}) \cdot (\overrightarrow{v[iGW]} \cdot \overrightarrow{n})}
 * {1- \frac {\overrightarrow{k[iGW]} \cdot \overrightarrow{rrec} -\overrightarrow{k[iGW]} \cdot \overrightarrow{rem}}
 * {\big\|\overrightarrow{dr}\big\|}} \f]
 */

double TrFctGW::deltanu(int rec, int em, int order, double trec)
{
	Vect n, rem, rrec, dr;  
	double tem, tr, te;
	Couple hrec, hem;
	double ndr;
	double hpr,hpe,hcr,hce;
	//double hr,he; 
	double ksip,ksic,un,vn,krrec,krem;
	double delta(0.0);
	//double delta_tmp(0.0);
	int NbGWs;
	int test_eric = 0;
	try {
		tem = trec + LISAGeo->gtdelay(em, rec, order, trec);
		rrec = LISAGeo->gposition(rec, trec)/c_SI;
		rem = LISAGeo->gposition(em, trec)/c_SI;
		dr = rrec-rem;
		ndr = dr.norme();
		n = dr/ndr;
		//n = (rrec-rem).unit();
		
		//cout << "  tem  = " << tem << endl;
		//cout << "  rrec = " << rrec.p[0] << " " << rrec.p[1] << " " << rrec.p[2] << endl;
		//cout << "  rrec = " << rem.p[0] << " " << rem.p[1] << " " << rem.p[2] << endl;
		//cout << "  n    = " << n.p[0] << " " << n.p[1] << " " << n.p[2] << endl;
		NbGWs = GWSources->size();
		for(int iGW=0; iGW<NbGWs; iGW++){
			//ksip = (u[iGW]*n)*(u[iGW]*n)-(v[iGW]*n)*(v[iGW]*n);
			//ksic = 2.*(u[iGW]*n)*(v[iGW]*n);
			un = u[iGW]*n;
			vn = v[iGW]*n;
			ksip = 0.5*(un*un-vn*vn); // OK
			//ksip = 0.5*(vn*vn-un*un); // Michele
			ksic = un*vn;
			
			krrec = k[iGW]*rrec;
			krem = k[iGW]*rem;
			//tr = trec-(k[iGW]*rrec)/c_SI;
			//te = tem-(k[iGW]*rem)/c_SI;
			tr = trec-krrec;
			te = tem-krem;
			
			//cout << "GW : x = " << k[iGW].p[0] << " y = " << k[iGW].p[1] << " z = " << k[iGW].p[2] << endl ;
			//cout << "Pos : rrec = " << rrec.p[0] << " " << rrec.p[1] << " " << rrec.p[2] << endl;
			//cout << "kr : krrec = " << krrec << " krem = " << krem << endl;
			//cout << "trec=" << trec << " " << "tem=" << tem << endl;
			//cout << "tr=" << tr << " " << "te=" << te << endl;
			
			hpr = (*GWSources)[iGW]->hp(tr); 
			hpe = (*GWSources)[iGW]->hp(te); 
			hcr = (*GWSources)[iGW]->hc(tr); 
			hce = (*GWSources)[iGW]->hc(te);  
			//cout << "trec,tem,hp_rec,hp_emm = " << tr << "  " << te << "  " << hpr << "  " << hpe << endl ;
			//cout <<trec<< "  " << tr << "  " << hpr << "  " << rec<< "   order " << order <<endl ;
			//cout << te << "  " << hpe << "  " << em << "   order " << order << endl ;

			
			//faiT une boucle autour de tr en fraction de secondes
			test_eric = 0 ;
			if((test_eric ==1) && (trec > 20000.)){
			  double ttt;
			  ofstream eric_file;
			  eric_file.open("data_hp.txt");
			  cout.precision(10);
			  eric_file.precision(10);
			  cout << "Writing in file : data_hp.txt" << endl ;
			  for (int l=0 ; l<10000 ;l++){
			    ttt=l/100.;
			    tr = trec-krrec+ttt;
			    te = tem-krem+ttt;
			    hpr = (*GWSources)[iGW]->hp(tr);
			    hpe = (*GWSources)[iGW]->hp(te); 
			    delta = ((hpe-hpr)*ksip)/(1.-(krrec-krem)/ndr);
			    eric_file << tr << "  " << hpr << "  "<< delta << "  " << ttt << endl ;
			    //cout << tr << "  " << hpr << "  "<< delta << "  " << ttt << endl ;
			  }
			  eric_file.close();
			  eric_file.open("data_nu.txt");
			  cout << "Writing in file : data_nu.txt" << endl ;
			  eric_file << "#time x  y " << endl ;
			  //for (int l=10000 ; l<210000 ;l++){
			  for (int l=10000 ; l<210000 ;l=l+1){
			    ttt=l;
			    tr = trec-krrec+ttt;
			    te = tem-krem+ttt;
			    hpr = (*GWSources)[iGW]->hp(tr);
			    hpe = (*GWSources)[iGW]->hp(te); 
			    delta = ((hpe-hpr)*ksip)/(1.-(krrec-krem)/ndr);
			    eric_file << tr << "  " << hpr << "  "<< delta << "  " << ttt << endl ;
			    //cout << tr << "  " << hpr << "  "<< delta << "  " << ttt << endl ;
			  }
			  cout << "stopped in TrFctGW::deltanu: " << endl;			  
			  throw ;
			}
			//*/

			//hr = hpr*ksip+hcr*ksic;
			//he = hpe*ksip+hce*ksic;
			
			//cout << "hp_rec=" << hpr << " " << "hc_rec=" << hcr  << " " << "h_rec=" << hr << endl;
			//cout << "hp_em=" << hpe << " " << "hc_em=" << hce << " " << "h_em=" << he << endl;
			
			//delta += ((hpe-hpr)*ksip+(hce-hcr)*ksic)/(1.-(krrec-krem)/ndr);
			/*delta_tmp = ((hpe-hpr)*ksip+(hce-hcr)*ksic)/(1.-(krrec-krem)/ndr);
			if(isnan(delta_tmp))
				throw invalid_argument("TrFctGW::deltanu : Gravitational wave signal is not a number !");
			*/
			delta += ((hpe-hpr)*ksip+(hce-hcr)*ksic)/(1.-(krrec-krem)/ndr);
			//cout << "Delta : Num = " << ((hpe-hpr)*ksip+(hce-hcr)*ksic) << "  , Den = " << 1.-(krrec-krem)/ndr ;
			//cout << " , Delta_tmp = " << delta_tmp ;
			//cout << " , Delta = " << delta << endl;
			//cout << endl << endl << endl;
		}
		/*
		if((rec == 1) && (em == 2)){
		cout << "TrFctGW : t,delta_nu = "<< trec << "  " << delta << " rec,em =  "<< rec << "   " << em << endl ; 
		}
		*/
		return delta;
	}
	
	catch( exception & e ) 
	{       
		cerr << "error in TrFctGW::deltanu: " << e.what() << endl;
		throw;
	}
	
}


//end of LISACODE-TrFctGW.cpp
