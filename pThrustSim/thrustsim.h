/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: thrustsim.h                                          */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for thrustsim.cpp                  */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"

class ThrustSim : public CMOOSApp 
{
public:
	ThrustSim();
	virtual ~ThrustSim() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double Fl;
	double Fr;
	double t;
};
