/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: thrustsim.h                                          */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in thrustsim.h                           */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "thrustsim.h"
#include <math.h>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

ThrustSim::ThrustSim() {
	t = 0;
	Fl = 0;
	Fr = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()
// Since this process does not need to subscribe to any variables from
// the MOOOSDB, this function always returns true

bool ThrustSim::OnNewMail(MOOSMSG_LIST &NewMail) {
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

bool ThrustSim::OnStartUp() {
	Fl = 100;
	if(!m_MissionReader.GetConfigurationParam("DESIRED_PORTTHRUSTER",Fl))
		MOOSTrace("Warning parameter \"DESIRED_PORTTHRUSTER\" not specified. Using default of \"%f\"\n",Fl);

	Fr = 100;
	if(!m_MissionReader.GetConfigurationParam("DESIRED_STARBOARDTHRUSTER",Fr))
		MOOSTrace("Warning parameter \"DESIRED_STARBOARDTHRUSTER\" not specified. Using default of \"%f\"\n",Fr);

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.
// Since this process does not subscribe to any variables, this function
// always returns true

bool ThrustSim::OnConnectToServer() {
	Register("TIME",0);
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool ThrustSim::Iterate() {
	Notify("DESIRED_PORTTHRUSTER", Fl);
	Notify("DESIRED_STARBOARDTHRUSTER", Fr);
	return true;
}