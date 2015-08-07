/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: vehicle.h                                            */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for vehicle.cpp                    */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <string>

class Controller : public CMOOSApp 
{
public:
	Controller();
	virtual ~Controller() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double NAV_YAW;
	double NAV_DEPTH;
	double t;
	double A;
	double B;
	double L;
	double x;	//(x,y) can be the GPS position (NAV_LAT,NAV_LONG) or local coordinate possition (NAV_X,NAV_Y)
	double y;
	double psi;	//NAV_HEADING
	double u;	//NAV_SPEED
	double ud;
	double v;
	double r;	
	double xd;	//(xd,yd) is the GPS coordibate position if (x,y) is the GPS pisition (NAV_LAT,NAV_LONG) or it is local coordinate possition if (x,y) is local coorinate position (NAV_X,NAV_Y). 
	double yd;
	double dxd;
	double dyd;

	double ddxd;
	double ddyd;

	double DESIRED_PORTTHRUSTER;
	double DESIRED_STARBOARDTHRUSTER;
	double portThrustCMD;
	double stdbThrustCMD;
	double Fmax;

	double Err_e_i;
	double Err_z_i;
	double K1_i;
	double K2_i;
	double K3_i;
	double K1_m;
	double K2_m;
	double K3_m;

	double K1;
	double K2;
	double K3;
	double epsilon;
	double e1;
	double e2;
	double de1;
	double de2;

	double eta1;
	double eta2;
	double eta1b;
	double eta2b;
	double deta1b;
	double deta2b;
	double tp1;
	double tp2;
	double dtp1;
	double dtp2;
	double z1;
	double z2;
	double zeta1;
	double zeta2;
	double m11b;
	double m22b;
	double m33b;
	double d11b;
	double d22b;
	double d33b;
	double rho1;
	double rho2;
	double rho3;
	double rho4;
	double rho5;
	double rho6;
	double rho7;
	double rho8;
	double rho9;
	double rho10;

	double e1out;
	double e2out;
	double P0_X;
	double P0_Y;

	double IMU_ready;
	
	/*std::string pattern;
	char buffer[10];
	double Header0XPos;
	double Header0YPos;
	double Header1XPos;*/
};
