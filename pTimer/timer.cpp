/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: timer.h                                              */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in timer.h                               */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "timer.h"
#include <math.h>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

Timer::Timer() {
	t0 = 0;
	t = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()
// Since this process does not need to subscribe to any variables from
// the MOOOSDB, this function always returns true

bool Timer::OnNewMail(MOOSMSG_LIST &NewMail) {
	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool Timer::OnStartUp() {

	t0 = MOOSTime();

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.
// Since this process does not subscribe to any variables, this function
// always returns true

bool Timer::OnConnectToServer() {
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool Timer::Iterate() {
	t = MOOSTime() - t0;
	Notify("TIME", t);
	return true;
}