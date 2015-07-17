/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: timer.h                                              */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for timer.cpp                      */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"

class StateGen : public CMOOSApp 
{
public:
	StateGen();
	virtual ~StateGen() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double xd;
	double yd;
	double x0;
	double y0;
	double dxd;
	double dyd;
	double ddxd;
	double ddyd;
	double psid;
	double psid_refE;
	double ud;
	double t;
	double lastTime;
	double lastdxd;
	double lastdyd;
};
