// $Id: LISACODE-Filter.cpp,v 1.6 2007/03/29 15:17:08 lalanne Exp $
/*
 *  Filter.cpp
 *  V 1.4
 *  LISACode
 *
 *
 * Description :  BASIC CLASS
 * ----------
 * Applique un filtre sur des donnees
 *
 *
 *  Created on 25/10/05 by Antoine PETITEAU (APC)
 *  Last modification on 25/10/05 by Antoine PETITEAU (APC)
 *
 */


#include "LISACODE-Filter.h"
using namespace std;

/* Constructor */
/*! \brief Constructs an instance and initializes it with default values.
 *
 * \arg	#alpha = empty
 * \arg	#beta = 1 element with value = 1
 * \arg	#TmpData = 2 elements
 */
Filter::Filter()
{
	alpha.resize(0);
	beta.resize(0);
	vector<double> TmpVect(1);
	TmpVect.push_back(1.0);
	beta.push_back(TmpVect);
	NbDataStab = 0;
	TmpData.resize(2);
}

/*! \brief Constructs an instance and initializes it with default values and inputs.
 *
 * #init method is called to set attributes :
 * \arg	#alpha = alpha_n input
 * \arg	#beta = beta_n input
 * \arg	#NbDataStab = 0
 * \arg	#TmpData = alpha_n size + 1 null vectors
 */
Filter::Filter(	vector< vector<double> > alpha_n,
				vector< vector<double> > beta_n)
{
	init(alpha_n, beta_n, 0);
}

/*! \brief Constructs an instance and initializes it with inputs.
 *
 * #init method is called to set attributes :
 * \arg	#alpha = alpha_n input
 * \arg	#beta = beta_n input
 * \arg	#NbDataStab = NbDataStab_n input
 * \arg	#TmpData = (alpha_n size + 1) null vectors
 */
Filter::Filter(	vector< vector<double> > alpha_n,
				vector< vector<double> > beta_n, 
				int NbDataStabilization_n)
{
	init(alpha_n, beta_n, NbDataStabilization_n);
}

/*! \brief Constructs an instance and initializes it with inputs.
 *
 * \param fe sampling frequency
 * \param at attenuation
 * \param bp oscillations in bandwidth
 * \param fb low transition frequency
 * \param fa high transition frequency
 *
 * #CalcEllipticFilter4LISACode method is called with inputs and 30 as maximum number of cells.\n
 *
 * Then, #CalcEllipticFilter4LISACode outputs are used :
 * \arg	CellsCoef Cells coefficients
 * \arg	NCellsOut number of cells
 *
 * #init method is called to set attributes :
 * \arg	#alpha = 2 first coefficients opposite, for each cell (using #CalcEllipticFilter4LISACode outputs)
 * \arg	#beta = 3 last coefficients, for each cell
 * \arg	#NbDataStab = \f$ \frac{100}{fe} \f$
 * \arg	#TmpData = (alpha_n size + 1 null) vectors
 */
Filter::Filter( double fe, double at, double bp, double fb, double fa)
{
	vector< vector<double> > alpha_n;
	vector< vector<double> > beta_n;
	double CellsCoef[30][5];
	int NCellsOut(0);
	vector<double> TmpVect;
	
	/*cout << " Elliptic filter :" <<endl;
	cout << "  - sampling frequency        = " << fe << " Hz" << endl;
	cout << "  - attenuation               = " << at << " dB" << endl;
	cout << "  - oscillations in bandwidth = " << bp << " dB" << endl;
	cout << "  - low transition frequency  = " << fb << " Hz" << endl;
	cout << "  - high transition frequency = " << fa << " Hz" << endl;*/
	
	cout << "        Make Elliptic filter : fe=" << fe << "Hz att=" << at << "dB osc=" << bp << "dB fb=" << fb << "Hz fa=" << fa << "Hz";
	
	CalcEllipticFilter4LISACode(fe, at, bp, fb, fa, 30, CellsCoef, &NCellsOut);
	
	cout << " " << NCellsOut << "Cells " << endl; 
	for(int iCell=0; iCell<NCellsOut; iCell++){
		//cout << "   Cellule " << iCell << " : " << CellsCoef[iCell][0] << " , " << CellsCoef[iCell][1] << " , " << CellsCoef[iCell][2] << " , " << CellsCoef[iCell][3] << " , " << CellsCoef[iCell][4] << endl;
		TmpVect.resize(0);
		TmpVect.push_back(-1.*CellsCoef[iCell][0]);
		TmpVect.push_back(-1.*CellsCoef[iCell][1]);
		alpha_n.push_back(TmpVect);
		TmpVect.resize(0);
		TmpVect.push_back(CellsCoef[iCell][2]);
		TmpVect.push_back(CellsCoef[iCell][3]);
		TmpVect.push_back(CellsCoef[iCell][4]);
		beta_n.push_back(TmpVect);
	}
	
	init(alpha_n, beta_n, 100.0/fe);
}

/*! \brief Destructor */
Filter::~Filter(){
	
}
	
/* Access methods */
/*! \brief Initializes an instance with default values and inputs.
 *
 * \arg	#alpha = alpha_n input
 * \arg	#beta = beta_n input
 * \arg	#NbDataStab = NbDataStabilization_n input
 * \arg	#TmpData = (alpha_n size + 1) null vectors
 */
void Filter::init(	vector< vector<double> > alpha_n,
					vector< vector<double> > beta_n,
					int NbDataStabilization_n)
{
	alpha.resize(alpha_n.size());
	for(int iFil=0; iFil<alpha.size(); iFil++){
		alpha[iFil].resize(alpha_n[iFil].size());
		for(int i=0; i<alpha[iFil].size(); i++){
			(alpha[iFil])[i] = (alpha_n[iFil])[i];
		}
	}
    //cout << endl;
	beta.resize(beta_n.size());
	for(int iFil=0; iFil<beta.size(); iFil++){
		beta[iFil].resize(beta_n[iFil].size());
		for(int i=0; i<beta[iFil].size(); i++){
			(beta[iFil])[i] = (beta_n[iFil])[i];
		}

	}
	NbDataStab = NbDataStabilization_n;
	TmpData.resize(alpha.size()+1);
	for(int iFil=0; iFil<alpha.size(); iFil++)
		TmpData[iFil+1].resize(0);
}

/*! \brief Gives maximum of #alpha or #beta attribute size. */
int Filter::getDepth()
{
	int Depth(0);
	for(int i=0; i<alpha.size(); i++)
		Depth = MAX(alpha[i].size()+1, Depth);
	for(int i=0; i<beta.size(); i++)
		Depth = MAX(beta[i].size(), Depth);
	return(Depth);
}

/*! \brief Returns #NbDataStab attribute. */
int Filter::getNbDataStab()
{
	return(NbDataStab);
}

/*  Others methods */
/*! \brief Appends data from RawData input (starting at StartBin index) to #TmpData attribute and ti FilterData output.
 *
 * RawData is first copied in TmpData, then filtering is applied.\n
 * For all TmpData elements (index :iFil=0,...,size(\f$ \alpha \f$)), a recursive computation is done :\n
 * \f$ \textrm{for }i=0,\dots,StartBin \f$
 * \f[ TmpData[iFil+1][i]=\sum_{k=0}^{size(\alpha[iFil])}{\alpha[iFil][k] \cdot TmpData[iFil+1][k+i+1]} \f]
 * \f[ + \sum_{k=0}^{size(\beta[iFil])}{\beta[iFil][k] \cdot TmpData[iFil][k+i]} \f]
 * Then last TmpData array is copied into FilterData. 
 */
void Filter::App(int StartBin, const vector<double> & RawData, vector<double> & FilterData){
	double Num_tmp (0.0);
	double Den_tmp (0.0);
	TmpData[0].resize(RawData.size());
	/*for(int i=0; i<RawData.size(); i++){ // Copie les donnees brutes dans la premiere liste intermediaire
		(TmpData[0])[i] = RawData[i];
	}*/
	TmpData[0] = RawData; 
	// Parcours des listes intermediaires en appliquant le filtre correspondant sur chacune
	for(int iFil=0; iFil<alpha.size(); iFil++){
		//cout << "Filtre " << iFil+1 << " :" << endl;
		if(TmpData[iFil+1].size() == 0) // Si premiere utilisation du filtre
			TmpData[iFil+1].resize(FilterData.size(),0.0); // on donne sa taille a la liste
		TmpData[iFil+1].insert(TmpData[iFil+1].begin(), StartBin+1, 0.0); // On insert des 0 la ou les resultats du filtre vont arriver
		// On applique le filtre
		for(int i=StartBin; i>=0; i--){
			Num_tmp = 0.0;
			Den_tmp = 0.0;
			//cout.precision(15);
			//cout << " i = " << i << endl;
			// Calcul du numerateur
			for(int k=0; k<(int)(alpha[iFil].size()); k++){
				Den_tmp += (alpha[iFil])[k]*(TmpData[iFil+1])[k+i+1];
				//cout << "   ++ a " << k << "(sur " << (TmpData[iFil+1])[k+i+1] <<") : " << (alpha[iFil])[k]*(TmpData[iFil+1])[k+i+1]<< " " << Den_tmp << endl;
			}
			// Calcul du denominateur
			for(int k=0; k<(int)(beta[iFil].size()); k++){
				Num_tmp += (beta[iFil])[k]*(TmpData[iFil])[k+i];
				//cout << "   ++ b " << k << "(sur " << (TmpData[iFil])[k+i] <<") : " << (beta[iFil])[k]*(TmpData[iFil])[k+i] << " " << Num_tmp << endl;
			}
			//cout << " ++ Result : " << Num_tmp << " + " << Den_tmp << " = " << Num_tmp + Den_tmp << endl;
			(TmpData[iFil+1])[i] = Num_tmp + Den_tmp;
		}
		// On elimine les dernieres valeurs
		(TmpData[iFil+1]).resize(FilterData.size());
	}
	// La liste des donnees completement filtre correspond a la derniere liste 
	/*cout << "StartBin = " << StartBin << endl;
	for(int i=0; i<10 ; i++){
		for(int iFil=0; iFil<TmpData.size(); iFil++){
			cout << " ";
			cout.width(10);
			cout << (TmpData[iFil])[i];
		}
		cout << endl;
	}*/
	FilterData = TmpData[TmpData.size()-1];
	/*for(int i=0; i<FilterData.size(); i++){
		FilterData[i] = (TmpData[TmpData.size()-1])[i];
		//cout.precision(25);
		//cout << (TmpData[TmpData.size()-1])[i] << " -> " << (FilterData[i]-(TmpData[TmpData.size()-1])[i])/(FilterData[i]) << endl;
	}*/
}

// end of Filter.cpp

