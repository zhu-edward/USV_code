/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: patterndefinition.h                                  */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for patterndefinition.cpp          */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <string>

class PatternDefinition : public CMOOSApp 
{
public:
	PatternDefinition();
	virtual ~PatternDefinition() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	struct point {double X; double Y;} leader, follower1, follower2, follower3;
	double height;
	double base;

	char buffer [100];
	std::string pattern;
}; 