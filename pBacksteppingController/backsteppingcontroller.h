/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: backsteppingcontroller.h                             */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in backsteppingcontroller.cpp            */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <string>

class BacksteppingController : public CMOOSApp 
{
public:
	BacksteppingController();
	virtual ~BacksteppingController() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	// Error states
	double e1s;
	double e2s;
	double zeta1;
	double zeta2;
	double z1;
	double z2;
	double z3;

	// State of vehicle
	double x;
	double y;
	double psi;
	double u;
	double v;
	double r;
	double t;
	double lastTime;
	double dT;

	double ud;

	// Position error state of all vehicles
	double e0_X;
	double e0_Y;
	double e1_X;
	double e1_Y;
	double e2_X;
	double e2_Y;
	double e3_X;
	double e3_Y;
	double e1out;
	double e2out;

	// Information weights
	double a0;
	double a1;
	double a2;
	double a3;

	// Vehicle parameters
	double L;

	// Estimated vehicle parameters
	double m1e;
	double m2e;
	double m3e;
	double d1e;
	double d2e;
	double d3e;

	// Control gains
	double rho1;
	double rho2;
	double rho3;
	double rho4;
	double rho5;
	double rho6;
	double rho7;
	double rhox;
	double rhoy;
	double K1;
	double K2;
	double K3;
	double K4;
	double K5;
	
	// Command filtered control signals
	double epsilon;

	double h;

	double eta1;
	double eta2;
	double eta3;
	double eta4;
	double eta5;
	double eta3f;
	double eta4f;
	double eta5f;
	
	double deta3f;
	double deta4f;
	double deta5f;

	double angleChange;

	// Control thrust values
	double Fmax;
	double delta1;
	double delta2;
	double DESIRED_PORTTHRUSTER;
	double DESIRED_STARBOARDTHRUSTER;
	double portThrustCMD;
	double stdbThrustCMD;

	std::string neighbors;
	int vehicleIdent;
	double connections;

	double IMU_ready;
	/*std::string pattern;
	char buffer [10];
	double Header1XPos, Header1YPos, Header2XPos, Header2YPos, Header3XPos, Header3YPos;*/
	
	double P1_X, P1_Y, P2_X, P2_Y, P3_X, P3_Y;

	std::string debug;
	char debugBuf[100];

};