/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: vehicle.cpp                                          */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in vehicle.h                             */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "controller.h"
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

Controller::Controller() {
	x=0;
	y=0;
	psi=0;
	u=0;
	ud = 0;
	v=0;
	r=0;
	xd=0;
	yd=0;
	dxd=0;
	dyd=0;
	ddxd=0;
	ddyd=0;
	
	P0_X = 0;
	P0_Y = 0;

	IMU_ready = 0;
}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool Controller::OnNewMail(MOOSMSG_LIST &NewMail) {

	MOOSMSG_LIST::iterator p1;

	for(p1=NewMail.begin();p1!=NewMail.end();p1++)
	{
		//lets get a reference to the Message - no need for pointless copy
		CMOOSMsg & rMsg = *p1;

		// repetitive "ifs" is one way to build a switch yard for
		// the messages
		if(MOOSStrCmp(rMsg.GetKey(),"NAV_Y"))
		{
			//this message is about something called "NAV_Y"
			CMOOSMsg &Msg = rMsg;
			if(!Msg.IsDouble())
				return MOOSFail("Ouch - was promised \"NAV_Y\" would be a double");

			y = Msg.GetDouble();
			//MOOSTrace("Input_FL is %f\n",x);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"NAV_X"))
		{
			//this message is about something called "NAV_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"NAV_X\" would be a double");

			x = Msg1.GetDouble();
			//MOOSTrace("Y is %f\n",y);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"NAV_YAW"))
		{
			//this message is about something called "Psi"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"NAV_YAW\" would be a double");

			psi = Msg1.GetDouble();
			//MOOSTrace("Psi is %f\n",ipsi);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"NAV_SPEED"))
		{
			//this message is about something called "U"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"U\" would be a double");

			u = Msg1.GetDouble();
			//MOOSTrace("U is %f\n",u);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"V"))
		{
			//this message is about something called "V"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"V\" would be a double");

			v = Msg1.GetDouble();
			//MOOSTrace("V is %f\n",v);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"R"))
		{
			//this message is about something called "R"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"R\" would be a double");

			r = Msg1.GetDouble();
			//MOOSTrace("R is %f\n",R);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"Xd"))
		{
			//this message is about something called "Xd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Xd\" would be a double");

			xd = Msg1.GetDouble();
			//MOOSTrace("Xd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"Yd"))
		{
			//this message is about something called "Yd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Yd\" would be a double");

			yd = Msg1.GetDouble();
			//MOOSTrace("Yd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}		
		else if(MOOSStrCmp(rMsg.GetKey(),"Dxd"))
		{
			//this message is about something called "Dxd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Dxd\" would be a double");

			dxd = Msg1.GetDouble();
			//MOOSTrace("Dxd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"Dyd"))
		{
			//this message is about something called "Dyd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Dyd\" would be a double");

			dyd = Msg1.GetDouble();
			//MOOSTrace("Dyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"Ddxd"))
		{
			//this message is about something called "Ddxd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Ddxd\" would be a double");

			ddxd = Msg1.GetDouble();
			//MOOSTrace("Ddxd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}			
		else if(MOOSStrCmp(rMsg.GetKey(),"Ddyd"))
		{
			//this message is about something called "Ddyd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"Ddyd\" would be a double");

			ddyd = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"TIME"))
		{
			//this message is about something called "Ddyd"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"TIME\" would be a double");

			t = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"DESIRED_SPEED"))
		{
			//this message is about something called "DESIRED_SPEED"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"DESIRED_SPEED\" would be a double");

			ud = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"IMU_GPS_Stable"))
		{
			//this message is about something called "IMU_GPS_Stable"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"IMU_GPS_Stable\" would be a double");

			IMU_ready = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
	}

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool Controller::OnStartUp() {
	// Initialize controller parameters
	
	L=1;

	if(!m_MissionReader.GetConfigurationParam("L",L))
	    MOOSTrace("Warning parameter \"L\" not specified. Using default of \"%f\"\n",L);

	Err_e_i=100;
	if(!m_MissionReader.GetConfigurationParam("Err_e_i",Err_e_i))
	    MOOSTrace("Warning parameter \"Err_e_i\" not specified. Using default of \"%f\"\n",Err_e_i);
	Err_z_i=100;
	if(!m_MissionReader.GetConfigurationParam("Err_z_i",Err_z_i))
	    MOOSTrace("Warning parameter \"Err_z_i\" not specified. Using default of \"%f\"\n",Err_z_i);

	K1_i=0.1;
	if(!m_MissionReader.GetConfigurationParam("K1_i",K1_i))
	    MOOSTrace("Warning parameter \"K1_i\" not specified. Using default of \"%f\"\n",K1_i);

	K2_i=0.1;
	if(!m_MissionReader.GetConfigurationParam("K2_i",K2_i))
	    MOOSTrace("Warning parameter \"K2_i\" not specified. Using default of \"%f\"\n",K2_i);

	K3_i=0.1;
	if(!m_MissionReader.GetConfigurationParam("K3_i",K3_i))
	    MOOSTrace("Warning parameter \"K3_i\" not specified. Using default of \"%f\"\n",K3_i);

	K1_m=5;
	if(!m_MissionReader.GetConfigurationParam("K1_m",K1_m))
	    MOOSTrace("Warning parameter \"K1_m\" not specified. Using default of \"%f\"\n",K1_m);

	K2_m=5;
	if(!m_MissionReader.GetConfigurationParam("K2_m",K2_m))
	    MOOSTrace("Warning parameter \"K2_m\" not specified. Using default of \"%f\"\n",K2_m);

	K3_m=5;
	if(!m_MissionReader.GetConfigurationParam("K3_m",K3_m))
	    MOOSTrace("Warning parameter \"K3_m\" not specified. Using default of \"%f\"\n",K3_m);

	epsilon=1;
	if(!m_MissionReader.GetConfigurationParam("Constant_epsilon",epsilon))
	    MOOSTrace("Warning parameter \"epsilon\" not specified. Using default of \"%f\"\n",epsilon);

	m11b=300;
	if(!m_MissionReader.GetConfigurationParam("M11b",m11b))
	    MOOSTrace("Warning parameter \"m11b\" not specified. Using default of \"%f\"\n",m11b);
	m22b=300;
	if(!m_MissionReader.GetConfigurationParam("M22b",m22b))
	    MOOSTrace("Warning parameter \"m22b\" not specified. Using default of \"%f\"\n",m22b);
	m33b=300;
	if(!m_MissionReader.GetConfigurationParam("M33b",m33b))
	    MOOSTrace("Warning parameter \"m33b\" not specified. Using default of \"%f\"\n",m33b);

	d11b=100;
	if(!m_MissionReader.GetConfigurationParam("D11b",d11b))
	    MOOSTrace("Warning parameter \"d11b\" not specified. Using default of \"%f\"\n",d11b);
	d22b=100;
	if(!m_MissionReader.GetConfigurationParam("D22b",d22b))
	    MOOSTrace("Warning parameter \"d22b\" not specified. Using default of \"%f\"\n",d22b);
	d33b=100;
	if(!m_MissionReader.GetConfigurationParam("D33b",d33b))
	    MOOSTrace("Warning parameter \"d33b\" not specified. Using default of \"%f\"\n",d33b);

	rho1=10;
	if(!m_MissionReader.GetConfigurationParam("Rho1",rho1))
	    MOOSTrace("Warning parameter \"rho1\" not specified. Using default of \"%f\"\n",rho1);
	rho2=10;
	if(!m_MissionReader.GetConfigurationParam("Rho2",rho2))
	    MOOSTrace("Warning parameter \"rho2\" not specified. Using default of \"%f\"\n",rho2);
	rho3=10;
	if(!m_MissionReader.GetConfigurationParam("Rho3",rho3))
	    MOOSTrace("Warning parameter \"rho3\" not specified. Using default of \"%f\"\n",rho3);
	rho4=10;
	if(!m_MissionReader.GetConfigurationParam("Rho4",rho4))
	    MOOSTrace("Warning parameter \"rho4\" not specified. Using default of \"%f\"\n",rho4);
	rho5=10;
	if(!m_MissionReader.GetConfigurationParam("Rho5",rho5))
	    MOOSTrace("Warning parameter \"rho5\" not specified. Using default of \"%f\"\n",rho5);
	rho6=10;
	if(!m_MissionReader.GetConfigurationParam("Rho6",rho6))
	    MOOSTrace("Warning parameter \"rho6\" not specified. Using default of \"%f\"\n",rho6);
	rho7=10;
	if(!m_MissionReader.GetConfigurationParam("Rho7",rho7))
	    MOOSTrace("Warning parameter \"rho7\" not specified. Using default of \"%f\"\n",rho7);
	rho8=10;
	if(!m_MissionReader.GetConfigurationParam("Rho8",rho8))
	    MOOSTrace("Warning parameter \"rho8\" not specified. Using default of \"%f\"\n",rho8);
	rho9=10;
	if(!m_MissionReader.GetConfigurationParam("Rho9",rho9))
	    MOOSTrace("Warning parameter \"rho9\" not specified. Using default of \"%f\"\n",rho9);
	rho10=10;
	if(!m_MissionReader.GetConfigurationParam("Rho10",rho10))
	    MOOSTrace("Warning parameter \"rho10\" not specified. Using default of \"%f\"\n",rho10);
	Fmax = 100;
	if(!m_MissionReader.GetConfigurationParam("Control_Saturation", Fmax))
		MOOSTrace("Warning parameter \"Control_Saturation\" not specified. Using default of \"%f\"\n", Fmax);

	return true;
}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool Controller::OnConnectToServer() {
	Register("NAV_X",0);	//NAV_X: x. (X,y) and (xd,yd) should be in the same coordinate
	Register("NAV_Y",0);	//NAV_Y: y
	Register("NAV_YAW",0);	//NAV_HEADING: psi
 	Register("NAV_SPEED",0);	//NAV_SPEED: u
	Register("V",0);
	Register("R",0);
	Register("Xd",0);
	Register("Yd",0);
	Register("Dxd",0);
	Register("Dyd",0);
	Register("Ddxd",0);
	Register("Ddyd",0);
	Register("TIME",0);
	Register("DESIRED_SPEED", 0);
	Register("IMU_GPS_Stable", 0);

	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool Controller::Iterate() {
	
	if (IMU_ready) {

		e1out = x - P0_X;
		e2out = y - P0_Y;

		e1=x-xd+epsilon*cos(psi);
		e2=y-yd+epsilon*sin(psi);

		if ((e1*e1+e2*e2)>Err_e_i) {
		       K1 = K1_i;
		}
		else {
		       K1=K1_m;
		}
	    
		tp1=-K1*e1+sin(psi)*v+dxd;
		tp2=-K1*e2-cos(psi)*v+dyd;
		eta1=cos(psi)*tp1+sin(psi)*tp2;
		eta2=-sin(psi)*tp1/epsilon+cos(psi)*tp2/epsilon;
		z1=u-eta1;
		z2=r-eta2;

		if (z1*z1+z2*z2>Err_z_i) {
			K2 = K2_i;
			K3 = K3_i;
		}
		else {
		    K2=K2_m;
		    K3=K3_m;
		}
		tp1=-K1*e1+dxd;
		tp2=-K1*e2+dyd;
		eta1b=cos(psi)*tp1+sin(psi)*tp2;
		eta2b=-sin(psi)*tp1/epsilon+cos(psi)*tp2/epsilon;
		de1=-K1*e1+cos(psi)*z1-epsilon*sin(psi)*z2;
		de2=-K1*e2+sin(psi)*z1+epsilon*cos(psi)*z2;
		dtp1=-K1*de1+ddxd;
		dtp2=-K1*de2+ddyd;

		deta1b=-sin(psi)*r*tp1+cos(psi)*dtp1+cos(psi)*r*tp2+sin(psi)*dtp2;
		deta2b=-cos(psi)*r*tp1/epsilon-sin(psi)*dtp1/epsilon-sin(psi)*r*tp2/epsilon+cos(psi)*dtp2/epsilon;

		zeta1=-K2*z1-m22b*v*r+d11b*u+m11b*deta1b-(rho2*abs(v*r)+rho4*abs(u)+rho1*abs(deta1b)+rho6)*copysign(1.0,z1);
		zeta2=-K3*z2-(m11b-m22b)*u*v+d33b*r+m33b*deta2b+m11b*m33b/m22b*u*r/epsilon+d22b*m33b/m22b*v/epsilon-((rho1+rho2)*abs(u*v)+rho5*abs(r)+rho3*abs(deta2b)+rho7+rho8*abs(u*r)/epsilon+rho10/epsilon)*copysign(1.0,z2);

		DESIRED_PORTTHRUSTER=(zeta1-zeta2/L)/2;   //left force
		DESIRED_STARBOARDTHRUSTER=(zeta1+zeta2/L)/2;  //right force

		if (abs(ud) <= 0.01) {
			DESIRED_PORTTHRUSTER = 0;
			DESIRED_STARBOARDTHRUSTER = 0;
		}

		if (abs(DESIRED_PORTTHRUSTER) > Fmax) {
			DESIRED_PORTTHRUSTER = copysign(Fmax, DESIRED_PORTTHRUSTER);
		}

		if (abs(DESIRED_STARBOARDTHRUSTER) > Fmax) {
			DESIRED_STARBOARDTHRUSTER = copysign(Fmax, DESIRED_STARBOARDTHRUSTER);
		}
	}
	else {
		DESIRED_PORTTHRUSTER = 0;
		DESIRED_STARBOARDTHRUSTER = 0;
	}

	portThrustCMD = DESIRED_PORTTHRUSTER*1000/Fmax;
	stdbThrustCMD = DESIRED_STARBOARDTHRUSTER*1000/Fmax;

	Notify("DESIRED_PORTTHRUSTER",DESIRED_PORTTHRUSTER);
	Notify("DESIRED_STARBOARDTHRUSTER",DESIRED_STARBOARDTHRUSTER);
	Notify("DESIRED_PORTTHRUSTER_CMD", portThrustCMD);
	Notify("DESIRED_STARBOARDTHRUSTER_CMD", stdbThrustCMD);
	Notify("e1out", e1out);
	Notify("e2out", e2out);

	return true;
}
