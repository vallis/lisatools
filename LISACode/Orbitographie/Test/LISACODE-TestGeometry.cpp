// $Id: LISACODE-TestGeometry.cpp,v 1.2 2007/02/19 09:56:23 ruiloba Exp $
/*
 *  TestGeometry.cpp
 *  V 1.4
 *  LISACode
 *
 *  Description :
 *  -------------
 
 *  
 *
 *
 *  Created on 19/09/05 by  A.Petiteau, T.Regimbau
 *  Last modification on 19/09/05 by A.Petiteau, T.Regimbau 
 *
 */

#include <stdexcept>
#include <iostream.h>
#include <fstream>
#include <math.h>
#include "LISACODE-Geometry.h"
#include "LISACODE-GeometryAnalytic.h"
#include "LISACODE-GeometryMLDC.h"
#include "LISACODE-Couple.h"
#include "LISACODE-Vect.h"
using namespace std;


int main (int argc, char * const argv[])
{
	try {
		
		cout << endl << "   ********************************** ";
		cout << endl << "   *                                * ";
		cout << endl << "   *  Test de la Geometrie de LISA  * ";
		cout << endl << "   *  ---------------------------   * ";
		cout << endl << "   *      ("<< LCVersion <<")       * ";
		cout << endl << "   *                                * ";
		cout << endl << "   ********************************** " << endl << endl; 
		
		
		
		// ************************
		// * begin of declaration *
		// ************************
		cout << "Creation of parameters ..." << endl;
		// Declaration of variables which control the simulation
		double tStep(100.0), tmax(Yr_SI);
		double StepdDislpay(24.0*3600.0);
		// Declaration of temporary variables
		double t(0.0), ttmpAff(0.0);
		//Order in compute of tdelay
		int order(2);
		// Arms lengths (in m)
		double L0_m(5.0e9); // Armlength
		double t0(0.0);		// Initial time (seconds) 
		double rot0(0.0);	// Initial rotation (radians)
		int GeoOrder(2);	// Order of geometry compute
		
		
		cout << endl << "Creation of items ..." << endl;
		// Declaration of used memorys
		ofstream Record_position, Record_velocity, Record_normal, Record_tdelay, Record_order05, Record_order1, Record_ArmVelocity  ;
		ofstream Record_QuickPos, Record_QuickDelay; 
		Record_position.open("GEOposition.txt"); //File of recording of the positions
		Record_velocity.open("GEOvelocity.txt"); //File of recording of the velocitys
		Record_normal.open("GEOnormal.txt");
		Record_tdelay.open("GEOtdelay.txt"); //File of recording of the delays
		Record_order05.open("GEOorder05.txt"); //File of recording of order 1/2
		Record_order1.open("GEOorder1.txt"); //File of recording of order 1/2
		Record_ArmVelocity.open("GEOArmVelocity.txt"); //File of recording of velocity along en arm
		Record_QuickPos.open("GeoQuickPos.txt");
		Record_QuickDelay.open("GeoQuickDelay.txt");
		
		
		// Creation of Geometry
		cout << " Compute geometry at order " << GeoOrder << endl;
		GeometryAnalytic Geo(t0, rot0, L0_m, GeoOrder, 1);
		
		cout << "  Creation --> OK" << endl << endl;
		
		
		
		// *******************
		// * Initialisations *
		// *******************
		cout << "Initialisations ..." << endl;
		Record_position<< "#Time px1 py1 pz1 px2 py2 pz2 px3 py3 pz3" << endl;
		Record_velocity << "#Time vx1 vy1 vz1 vx2 vy2 vz2 vx3 vy3 vz3" << endl;
		Record_normal << "#Time nx ny nz" << endl;
		Record_tdelay << "#Time D12 D23 D31 D13 D21 D32" << endl;
		Record_order05 << "#Time D12o05 D23o05 D31o05 D13o05 D21o05 D32o05" << endl;
		Record_order1 << "#Time D12o1 D23o1 D31o1 D13o1 D21o1 D32o1" << endl;
		Record_ArmVelocity << "#Time v12 v23 v31" << endl;
		Record_QuickPos << "#Time px1 py1 pz1 px2 py2 pz2 px3 py3 pz3" << endl;
		Record_QuickDelay << "#Time D12 D23 D31 D13 D21 D32" << endl;
		
		cout << "  Initialisations --> OK" << endl << endl;
		
		// *************
		// * Execution *
		// *************
		cout << "Runnning in progress (->"<< tmax << "s) ..." << endl;
		
		do{
			t += tStep;
			
			//Display time
			ttmpAff += tStep;
			if(ttmpAff >= StepdDislpay){
				cout << t/86400.0 << " days" << endl;
				ttmpAff = 0.0;
			}
			
			//Record spacecrafts positions and velocity
			Record_position << t;
			Record_velocity << t;
			Record_QuickPos << t;
			for(int iSC=1; iSC<=3; iSC++) {
				Vect tmp_pos(Geo.position(iSC,t));
				Vect tmp_vel(Geo.velocity(iSC,t));
				Vect tmp_qp(Geo.gposition(iSC,t));
				Record_position << " " << tmp_pos.p[0] << " " << tmp_pos.p[1] << " " << tmp_pos.p[2];
				Record_velocity << " " << tmp_vel.p[0] << " " << tmp_vel.p[1] << " " << tmp_vel.p[2];
				Record_QuickPos << " " << tmp_qp.p[0] << " " << tmp_qp.p[1] << " " << tmp_qp.p[2];
			}
			Record_position << endl;
			Record_velocity << endl;
			Record_QuickPos << endl;
			Vect tmp_n(Geo.VectNormal(t));
			Record_normal << t << " " << tmp_n.p[0] << " " << tmp_n.p[1] << " " << tmp_n.p[2] << endl;  
			
			//Record Delay
			Record_tdelay.precision(10);
			Record_tdelay << t ;
			Record_tdelay << " " << Geo.tdelay(1, 2, order, t);
			Record_tdelay << " " << Geo.tdelay(2, 3, order, t);
			Record_tdelay << " " << Geo.tdelay(3, 1, order, t);
			Record_tdelay << " " << Geo.tdelay(1, 3, order, t);
			Record_tdelay << " " << Geo.tdelay(2, 1, order, t);
			Record_tdelay << " " << Geo.tdelay(3, 2, order, t);
			Record_tdelay << endl;
						
			
			// Record Order 1/2
			Record_order05 << t ;
			Record_order05 << " " << Geo.tdelayOrderContribution(1, 2, 1, t);
			Record_order05 << " " << Geo.tdelayOrderContribution(2, 3, 1, t);
			Record_order05 << " " << Geo.tdelayOrderContribution(3, 1, 1, t);
			Record_order05 << " " << Geo.tdelayOrderContribution(1, 3, 1, t);
			Record_order05 << " " << Geo.tdelayOrderContribution(2, 1, 1, t);
			Record_order05 << " " << Geo.tdelayOrderContribution(3, 2, 1, t);
			Record_order05 << endl;
			
			// Record Order 1
			Record_order1 << t ;
			Record_order1 << " " << Geo.tdelayOrderContribution(1, 2, 2, t);
			Record_order1 << " " << Geo.tdelayOrderContribution(2, 3, 2, t);
			Record_order1 << " " << Geo.tdelayOrderContribution(3, 1, 2, t);
			Record_order1 << " " << Geo.tdelayOrderContribution(1, 3, 2, t);
			Record_order1 << " " << Geo.tdelayOrderContribution(2, 1, 2, t);
			Record_order1 << " " << Geo.tdelayOrderContribution(3, 2, 2, t);
			Record_order1 << endl;
			
			// Record velocity along an arm
			Record_ArmVelocity << t ;
			Record_ArmVelocity << " " << Geo.ArmVelocity(1, 2, t);
			Record_ArmVelocity << " " << Geo.ArmVelocity(2, 3, t);
			Record_ArmVelocity << " " << Geo.ArmVelocity(3, 1, t);
			Record_ArmVelocity << endl;
			
			
			//Record Delay
			Record_QuickDelay.precision(10);
			Record_QuickDelay << t ;
			Record_QuickDelay << " " << Geo.gtdelay(1, 2, order, t);
			Record_QuickDelay << " " << Geo.gtdelay(2, 3, order, t);
			Record_QuickDelay << " " << Geo.gtdelay(3, 1, order, t);
			Record_QuickDelay << " " << Geo.gtdelay(1, 3, order, t);
			Record_QuickDelay << " " << Geo.gtdelay(2, 1, order, t);
			Record_QuickDelay << " " << Geo.gtdelay(3, 2, order, t);
			Record_QuickDelay << endl;
			
			
		}while(t<=tmax);
		
		cout << "Temps final : " << t << " s" << endl;
		cout << "Fin" << endl;
		Record_position.close();
		Record_velocity.close();
		Record_tdelay.close();
		Record_order05.close();
		Record_order1.close();
		Record_ArmVelocity.close();
	}
	
	catch( exception & e ) 
{
		cerr << "error in main: " << e.what()<<endl;
		cerr << "abort!" << endl;
		exit(1);
}	

return(0);
}



// end of SimulLISA.cpp
