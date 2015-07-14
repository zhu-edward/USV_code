/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: trajectorysim.cpp                                    */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in trajectorysim.h                       */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "trajectorysim.h"
#include <math.h>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

TrajectorySim::TrajectorySim() {
	ud = 0;
	psid = 0;
	t = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool TrajectorySim::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::iterator p1;

	for (p1 = NewMail.begin(); p1!=NewMail.end(); p1++) {
		CMOOSMsg & rMsg = *p1;

		if (MOOSStrCmp(rMsg.GetKey(), "TIME")) {
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

bool TrajectorySim::OnStartUp() {
	t = 0;

	ud = 0.5;	// default value for initial x coordinate
	// here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("ud",ud))
		MOOSTrace("Warning parameter \"ud\" not specified. Using default of \"%f\"\n",ud);

	psid = 1.0;	// default value for initial x coordinate
	// here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("psid",psid))
		MOOSTrace("Warning parameter \"psid\" not specified. Using default of \"%f\"\n",psid);

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool TrajectorySim::OnConnectToServer() {
	Register("TIME", 0);
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool TrajectorySim::Iterate() {
	//ud = 0.5 + 0.5*sin(0.1*t);
	psid = 3.14/2+(t*2*3.14/100);
	Notify("DESIRED_HEADING", psid);
	Notify("DESIRED_SPEED", ud);
	return true;
}