/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: trajectorysim.h                                      */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for trajectorysim.cpp              */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"

class TrajectorySim : public CMOOSApp 
{
public:
	TrajectorySim();
	virtual ~TrajectorySim() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double ud;
	double psid;
	double t;
};
