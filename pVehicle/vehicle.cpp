/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: vehicle.cpp                                          */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in vehicle.h                             */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "vehicle.h"
#include <math.h>
#include <fstream>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

Vehicle::Vehicle() {
	// Initialize model parameters
	m11=190;
	m22=240;
	m33=82;
	d11=75;
	d22=105;
	d33=55;
	D11=0;
	D22=0;
	D33=0;
	L=1;	//it should be the same as that in the controller
	t=0;
	t_r=0;
	init = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool Vehicle::OnNewMail(MOOSMSG_LIST &NewMail) {

	MOOSMSG_LIST::iterator p1;

	for (p1 = NewMail.begin(); p1!=NewMail.end(); p1++) {
		CMOOSMsg & rMsg = *p1;

		if (MOOSStrCmp(rMsg.GetKey(), "DESIRED_PORTTHRUSTER")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"DESIRED_PORTTHRUSTER\" needs to be a double");
			DESIRED_PORTTHRUSTER = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "DESIRED_STARBOARDTHRUSTER")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"DESIRED_STARBOARDTHRUSTER\" needs to be a double");
			DESIRED_STARBOARDTHRUSTER = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "TIME")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"TIME\" needs to be a double");
			t = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "X_INIT")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"X_INIT\" needs to be a double");
			x0 = Msg.GetDouble();
		}
		else if (MOOSStrCmp(rMsg.GetKey(), "Y_INIT")) {
			CMOOSMsg &Msg = rMsg;
			if (!Msg.IsDouble())
				return MOOSFail("\"Y_INIT\" needs to be a double");
			y0 = Msg.GetDouble();
		}
	}

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool Vehicle::OnStartUp() {
	// Define initial conditions
	
	m_sVehicleName = "UnNamed";
	if(!m_MissionReader.GetConfigurationParam("VehicleName",m_sVehicleName))
		MOOSTrace("Warning parameter \"VechicleName\" not specified. Using default of \"%s\"\n",m_sVehicleName.c_str());

	psi=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("InitialLocation_psi",psi))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",psi);
	psi_deg = fmod(psi * 180 / PI, 360);


	u=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("InitialLocation_u",u))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",u);

	v=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("InitialLocation_v",v))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",v);

	r=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("InitialLocation_r",r))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",r);

	DESIRED_PORTTHRUSTER=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("DESIRED_PORTTHRUSTER",DESIRED_PORTTHRUSTER))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",DESIRED_PORTTHRUSTER);

	DESIRED_STARBOARDTHRUSTER=0;	//default value
	//here we extract a double from the configuration file
	if(!m_MissionReader.GetConfigurationParam("DESIRED_STARBOARDTHRUSTER", DESIRED_STARBOARDTHRUSTER))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",DESIRED_STARBOARDTHRUSTER);

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool Vehicle::OnConnectToServer() {
	Register("DESIRED_PORTTHRUSTER",0);
	Register("DESIRED_STARBOARDTHRUSTER",0);
	Register("X_INIT",0);
	Register("Y_INIT",0);
	Register("TIME",0);
	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool Vehicle::Iterate() {
	
	if (init==0) {
		x = x0;
		y = y0;
		init = 1;
	}

	dx=u*cos(psi)-v*sin(psi);
	dy=u*sin(psi)+v*cos(psi);
	dpsi=r;
	dv=(-m11*u*r-d22*v-D22)/m22;
	du=(DESIRED_PORTTHRUSTER+DESIRED_STARBOARDTHRUSTER+m22*v*r-d11*u-D11)/m11;
	dr=(L*(DESIRED_STARBOARDTHRUSTER-DESIRED_PORTTHRUSTER)+(m11-m22)*u*v-d33*r-D33)/m33;

	dT=t-t_r;
	t_r=t;	//Update t_r

	x=x+dT*dx;
	y=y+dT*dy;
	psi=psi+dT*dpsi;
	u=u+dT*du;
	v=v+dT*dv;
	r=r+dT*dr;
	
	psi_deg = fmod(psi * 180 / PI, 360);

	//Update state to MOOSDB
	Notify("NAV_X", x);
	Notify("NAV_Y", y);
	Notify("NAV_LAT", y);
	Notify("NAV_LONG", x);
	Notify("NAV_HEADING", psi_deg);
	Notify("NAV_YAW", psi);
	Notify("NAV_SPEED", u);
	Notify("V", v);
	Notify("R", r);

	return true;
}