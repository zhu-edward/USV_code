/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: timer.h                                              */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in timer.h                               */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "stategen.h"
#include <math.h>
#include <fstream>
//#define PI (4*atan(1))

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

StateGen::StateGen() {
	dxd = 0;
	dyd = 0;
	ddxd = 0;
	ddyd = 0;
	psid = 0;
	ud = 0;
	t = 0;
	lastTime = 0;
	lastdxd = 0;
	lastdyd = 0;
	init = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool StateGen::OnNewMail(MOOSMSG_LIST &NewMail) {

	MOOSMSG_LIST::iterator p1;

	for (p1 = NewMail.begin(); p1!=NewMail.end(); p1++) {
		CMOOSMsg & rMsg = *p1;

		if (MOOSStrCmp(rMsg.GetKey(), "DESIRED_HEADING")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"DESIRED_HEADING\" needs to be a double");
			psid = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "DESIRED_SPEED")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"DESIRED_SPEED\" needs to be a double");
			ud = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "TIME")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"TIME\" needs to be a double");
			t = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "X_INIT")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"X_INIT\" needs to be a double");
			x0 = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "Y_INIT")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"Y_INIT\" needs to be a double");
			y0 = Msg.GetDouble();
		}
	}

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool StateGen::OnStartUp() {

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool StateGen::OnConnectToServer() {
	Register("DESIRED_HEADING", 0);
	Register("DESIRED_SPEED", 0);
	Register("TIME", 0);
	Register("X_INIT", 0);
	Register("Y_INIT", 0);
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool StateGen::Iterate() {
	
	if (init == 0) {
		xd = x0;
		yd = y0;
		init = 1;
	}

	// Convert from HelmIvP reference frame (z-down, N-zero), to model and controller reference frame (z-up, E-zero)
	psid_refE = (90-psid)*PI/180;

	if (psid_refE < 0) {
		psid_refE = fmod(psid_refE, 2*PI) + 2*PI;
	}
	else {
		psid_refE = fmod(psid_refE, 2*PI);
	}

	// Find the desired velocities in the global x and y directions
	dxd = ud * cos(psid_refE);
	dyd = ud * sin(psid_refE);

	// Find the desired positions in the global x and y directions by numerically integrating the velocities
	xd = xd + ((t - lastTime) * dxd);
	yd = yd + ((t - lastTime) * dyd);

	// Find the desired accelerations in the global x and y directions by differentiating the velocities
	ddxd = (dxd - lastdxd) / (t - lastTime);
	ddyd = (dyd - lastdyd) / (t - lastTime);

	// Record current state to find next state
	lastTime = t;
	lastdxd = dxd;
	lastdyd = dyd;

	// Publish the state variables and initial conditions to MOOSDB
	Notify("Xd", xd);
	Notify("Yd", yd);
	Notify("Dxd", dxd);
	Notify("Dyd", dyd);
	Notify("Ddxd", ddxd);
	Notify("Ddyd", ddyd);
	Notify("DESIRED_HEADING_REFE", psid_refE);

	return true;
}