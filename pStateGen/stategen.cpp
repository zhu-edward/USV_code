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

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

StateGen::StateGen() {
	xd = 0;
	yd = 0;
	dxd = 0;
	dyd = 0;
	ddxd = 0;
	ddyd = 0;
	psid = 0;
	ud = 0;
	x0 = 0;
	y0 = 0;
	t = 0;
	lastTime = 0;
	lastdxd = 0;
	lastdyd = 0;
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
	}

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool StateGen::OnStartUp() {
	// Initialize state variables
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

	x0 = 0.0;	// default value for initial x coordinate
	// here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("x0",x0))
	MOOSTrace("Warning parameter \"x0\" not specified. Using default of \"%f\"\n",x0);

	y0 = -20.0; // default value for initial y coordinate
	// here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("y0",y0))
	MOOSTrace("Warning parameter \"y0\" not specified. Using default of \"%f\"\n",y0);

	xd = x0;
	yd = y0;

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool StateGen::OnConnectToServer() {
	Register("DESIRED_HEADING", 0);
	Register("DESIRED_SPEED", 0);
	Register("TIME", 0);
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool StateGen::Iterate() {
	
	dxd = ud * sin(psid);
	dyd = ud * cos(psid);
	xd = xd + ((t - lastTime) * dxd);
	yd = yd + ((t - lastTime) * dyd);
	ddxd = (dxd - lastdxd) / (t - lastTime);
	ddyd = (dyd - lastdyd) / (t - lastTime);

	Notify("Xd", xd);
	Notify("Yd", yd);
	Notify("Dxd", dxd);
	Notify("Dyd", dyd);
	Notify("Ddxd", ddxd);
	Notify("Ddyd", ddyd);
	Notify("X_INIT", x0);
	Notify("Y_INIT", y0);

	lastTime = t;
	lastdxd = dxd;
	lastdyd = dyd;

	return true;
}