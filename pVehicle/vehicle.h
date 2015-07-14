/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: vehicle.h                                            */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    This is the header file for vehicle.cpp                    */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"

class Vehicle : public CMOOSApp 
{
public:
	Vehicle();
	virtual ~Vehicle() {}

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool OnStartUp();
	bool OnConnectToServer();
	bool Iterate();

protected:
	double m11;
	double m22;
	double m33;
	double d11;
	double d22;
	double d33;
	double D11;
	double D22;
	double D33;
	double L;

	double x;
	double y;
	double psi;
	double u;
	double v;
	double r;
	double DESIRED_PORTTHRUSTER;
	double DESIRED_STARBOARDTHRUSTER;
	double t;
	double t_r;	
	double dx;
	double dy;	
	double dpsi;
	double du;
	double dv;
	double dr;
	double dT;
	std::string m_sVehicleName;
	double eta1;
	double eta2;
};
