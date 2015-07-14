/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: timer.h                                              */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for timer.cpp                      */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"

class Timer : public CMOOSApp 
{
public:
	Timer();
	virtual ~Timer() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double t0;
	double t;
};
