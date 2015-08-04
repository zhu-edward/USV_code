/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: backsteppingcontroller.cpp                           */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in backsteppingcontroller.h              */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "backsteppingcontroller.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

BacksteppingController::BacksteppingController() {

	e1out = 0;
	e2out = 0;
	e0_X = 0;
	e0_Y = 0;
	e1_X = 0;
	e1_Y = 0;
	e2_X = 0;
	e2_Y = 0;
	e3_X = 0;
	e3_Y = 0;

	x = 0;
	y = 0;
	psi = 0;
	u = 0;
	v = 0;
	r = 0;
	t = 0;
	lastTime = 0;
	dT = 0;

	ud = 0;

	connections = 0;

	P1_X = 0;
	P1_Y = 0;
	P2_X = 0;
	P2_Y = 0;
	P3_X = 0;
	P3_Y = 0;

	a0 = 0;
	a1 = 0;
	a2 = 0;
	a3 = 0;
}	

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool BacksteppingController::OnNewMail(MOOSMSG_LIST &NewMail) {

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
		else if(MOOSStrCmp(rMsg.GetKey(),"TIME"))
		{
			//this message is about something called "Time"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"TIME\" would be a double");

			t = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"pattern"))
		{
			//this message is about something called "Pattern"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsString())
				return MOOSFail("Ouch - was promised \"pattern\" would be a string");

			// Parse the pattern definition string to find the coordinates
			pattern = Msg1.GetString();

			char newBuf[10] = {'0','0','0','0','0','0','0','0','0','0'};

			Header1XPos = pattern.find("P1_X");
			Header1YPos = pattern.find("P1_Y");
			Header2XPos = pattern.find("P2_X");
			Header2YPos = pattern.find("P2_Y");
			Header3XPos = pattern.find("P3_X");
			Header3YPos = pattern.find("P3_Y");

			pattern.copy(buffer, (Header1YPos-1)-(Header1XPos+5), Header1XPos+5);
			P1_X = atof(buffer);
			strcpy(buffer, newBuf);
			pattern.copy(buffer, (Header2XPos-1)-(Header1YPos+5), Header1YPos+5);
			P1_Y = atof(buffer);
			strcpy(buffer, newBuf);
			pattern.copy(buffer, (Header2YPos-1)-(Header2XPos+5), Header2XPos+5);
			P2_X = atof(buffer);
			strcpy(buffer, newBuf);
			pattern.copy(buffer, (Header3XPos-1)-(Header2YPos+5), Header2YPos+5);
			P2_Y = atof(buffer);
			strcpy(buffer, newBuf);
			pattern.copy(buffer, (Header3YPos-1)-(Header3XPos+5), Header3XPos+5);
			P3_X = atof(buffer);
			strcpy(buffer, newBuf);
			pattern.copy(buffer, pattern.length()-(Header3YPos+5), Header3YPos+5);
			P3_Y = atof(buffer);
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
		else if(MOOSStrCmp(rMsg.GetKey(),"e0_X"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e0_X\" would be a double");

			e0_X = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e0_Y"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e0_Y\" would be a double");

			e0_Y = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e2_X"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e2_X\" would be a double");

			e2_X = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e2_Y"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e2_Y\" would be a double");

			e2_Y = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e3_X"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e3_X\" would be a double");

			e3_X = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e3_Y"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e3_Y\" would be a double");

			e3_Y = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e1_X"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e1_X\" would be a double");

			e1_X = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e1_Y"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e1_Y\" would be a double");

			e1_Y = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e1_X"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e1_X\" would be a double");

			e1_X = Msg1.GetDouble();
			//MOOSTrace("Ddyd is %f\n",input_FR);//the actual heading
			//if you want to see all details in Msg, you can print a message by
			//Msg.Trace();
		}
		else if(MOOSStrCmp(rMsg.GetKey(),"e1_Y"))
		{
			//this message is about something called "e0_X"
			CMOOSMsg &Msg1 = rMsg;
			if(!Msg1.IsDouble())
				return MOOSFail("Ouch - was promised \"e1_Y\" would be a double");

			e1_Y = Msg1.GetDouble();
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

bool BacksteppingController::OnStartUp() {

	vehicleIdent = 1;
	if (!m_MissionReader.GetConfigurationParam("VehicleIdentity", vehicleIdent))
		MOOSTrace("Warning Parameter \"VehicleIdentity\" not specified. Using default of \"%f\"\n", vehicleIdent);
	Fmax = 300;
	if(!m_MissionReader.GetConfigurationParam("Control_Saturation", Fmax))
		MOOSTrace("Warning parameter \"Control_Saturation\" not specified. Using default of \"%f\"\n", Fmax);
	rho1=10;
	if(!m_MissionReader.GetConfigurationParam("rho1",rho1))
	    MOOSTrace("Warning parameter \"rho1\" not specified. Using default of \"%f\"\n",rho1);
	rho2=10;
	if(!m_MissionReader.GetConfigurationParam("rho2",rho2))
	    MOOSTrace("Warning parameter \"rho2\" not specified. Using default of \"%f\"\n",rho2);
	rho3=10;
	if(!m_MissionReader.GetConfigurationParam("rho3",rho3))
	    MOOSTrace("Warning parameter \"rho3\" not specified. Using default of \"%f\"\n",rho3);
	rho4=10;
	if(!m_MissionReader.GetConfigurationParam("rho4",rho4))
	    MOOSTrace("Warning parameter \"rho4\" not specified. Using default of \"%f\"\n",rho4);
	rho5=10;
	if(!m_MissionReader.GetConfigurationParam("rho5",rho5))
	    MOOSTrace("Warning parameter \"rho5\" not specified. Using default of \"%f\"\n",rho5);
	rho6=10;
	if(!m_MissionReader.GetConfigurationParam("rho6",rho6))
	    MOOSTrace("Warning parameter \"rho6\" not specified. Using default of \"%f\"\n",rho6);
	rho7=10;
	if(!m_MissionReader.GetConfigurationParam("rho7",rho7))
	    MOOSTrace("Warning parameter \"rho7\" not specified. Using default of \"%f\"\n",rho7);
	rhox=3;
	if(!m_MissionReader.GetConfigurationParam("rhox",rhox))
	    MOOSTrace("Warning parameter \"rhox\" not specified. Using default of \"%f\"\n",rhox);
	rhoy=3;
	if(!m_MissionReader.GetConfigurationParam("Rhoy",rhoy))
	    MOOSTrace("Warning parameter \"rhoy\" not specified. Using default of \"%f\"\n",rhoy);
	K1=3;
	if(!m_MissionReader.GetConfigurationParam("K1",K1))
	    MOOSTrace("Warning parameter \"K1\" not specified. Using default of \"%f\"\n",K1);
	K2=3;
	if(!m_MissionReader.GetConfigurationParam("K2",K2))
	    MOOSTrace("Warning parameter \"K2\" not specified. Using default of \"%f\"\n",K2);
	K3=3;
	if(!m_MissionReader.GetConfigurationParam("K3",K3))
	    MOOSTrace("Warning parameter \"K3\" not specified. Using default of \"%f\"\n",K3);
	K4=3;
	if(!m_MissionReader.GetConfigurationParam("K4",K4))
	    MOOSTrace("Warning parameter \"K4\" not specified. Using default of \"%f\"\n",K4);
	K5=3;
	if(!m_MissionReader.GetConfigurationParam("K5",K5))
	    MOOSTrace("Warning parameter \"K5\" not specified. Using default of \"%f\"\n",K5);
	m1e=210;
	if(!m_MissionReader.GetConfigurationParam("m1e",m1e))
	    MOOSTrace("Warning parameter \"m1e\" not specified. Using default of \"%f\"\n",m1e);
	m2e=245;
	if(!m_MissionReader.GetConfigurationParam("m2e",m2e))
	    MOOSTrace("Warning parameter \"m2e\" not specified. Using default of \"%f\"\n",m2e);
	m3e=75;
	if(!m_MissionReader.GetConfigurationParam("m3e",m3e))
	    MOOSTrace("Warning parameter \"m3e\" not specified. Using default of \"%f\"\n",m3e);
	d1e=65;
	if(!m_MissionReader.GetConfigurationParam("d1e",d1e))
	    MOOSTrace("Warning parameter \"d1e\" not specified. Using default of \"%f\"\n",d1e);
	d2e=110;
	if(!m_MissionReader.GetConfigurationParam("d2e",d2e))
	    MOOSTrace("Warning parameter \"d2e\" not specified. Using default of \"%f\"\n",d2e);
	d3e=52;
	if(!m_MissionReader.GetConfigurationParam("d3e",d3e))
	    MOOSTrace("Warning parameter \"d3e\" not specified. Using default of \"%f\"\n",d3e);
	L=1;
	if(!m_MissionReader.GetConfigurationParam("L",L))
	    MOOSTrace("Warning parameter \"L\" not specified. Using default of \"%f\"\n",L);
	epsilon = 0.05;
	if(!m_MissionReader.GetConfigurationParam("epsilon",epsilon))
	    MOOSTrace("Warning parameter \"epsilon\" not specified. Using default of \"%f\"\n",epsilon);
	eta3f=1;
	if(!m_MissionReader.GetConfigurationParam("eta3f",eta3f))
	    MOOSTrace("Warning parameter \"eta3f\" not specified. Using default of \"%f\"\n",eta3f);
	eta4f=1;
	if(!m_MissionReader.GetConfigurationParam("eta4f",eta4f))
	    MOOSTrace("Warning parameter \"eta4f\" not specified. Using default of \"%f\"\n",eta4f);
	eta5f=1;
	if(!m_MissionReader.GetConfigurationParam("eta5f",eta5f))
	    MOOSTrace("Warning parameter \"eta5f\" not specified. Using default of \"%f\"\n",eta5f);

	return true;

}

//--------------------------------------------------------------------
// Procedure: OnConnectToServer
// Registers the variables that this process needs to subscribe to.

bool BacksteppingController::OnConnectToServer() {
	
	Register("TIME", 0);
	Register("NAV_X", 0);
	Register("NAV_Y", 0);
	Register("NAV_YAW", 0);
	Register("NAV_SPEED", 0);
	Register("V", 0);
	Register("R", 0);
	Register("pattern", 0);
	Register("DESIRED_SPEED", 0);
	Register("e0_X", 0);
	Register("e0_Y", 0);
	Register("e1_X", 0);
	Register("e1_Y", 0);
	Register("e2_X", 0);
	Register("e2_Y", 0);
	Register("e3_X", 0);
	Register("e3_Y", 0);
			

	return true;
}

//--------------------------------------------------------------------
// Procedure: Iterate
// This function contains the commands that will be iterated upon
// upon runtime

bool BacksteppingController::Iterate() {
	
	h = exp(-0.1*t);

	switch(vehicleIdent) {
		case 1:
			a0 = 0.5;
			a2 = 0.25;
			a3 = 0.25;
			zeta1 = a0*e0_X+a2*e2_X+a3*e3_X;
			zeta2 = a0*e0_Y+a2*e2_Y+a3*e3_Y;
			e1out = x-P1_X;
			e2out = y-P1_Y;
			e1s = e1out-zeta1;
			e2s = e2out-zeta2;
			break;
		case 2:
			a1 = 1;
			zeta1 = a1*e1_X;
			zeta2 = a1*e1_Y;
			e1out = x-P2_X;
			e2out = y-P2_Y;
			e1s = e1out-zeta1;
			e2s = e2out-zeta2;
			break;
		case 3:
			a1 = 1;
			zeta1 = a1*e1_X;
			zeta2 = a1*e1_Y;
			e1out = x-P3_X;
			e2out = y-P3_Y;
			e1s = e1out-zeta1;
			e2s = e2out-zeta2;
			break;
	}
	
	eta1 = -K1*e1s-rhox*(e1s/sqrt(e1s*e1s+h));
	eta2 = -K2*e2s-rhoy*(e2s/sqrt(e2s*e2s+h));
	eta3 = sqrt(eta1*eta1+eta2*eta2);
	eta4 = atan2(eta2,eta1);
	if (eta4 < 0) {
		eta4 = eta4+2*PI;
	}

	deta3f = -(eta3f-eta3)/epsilon;
	angleChange = eta4f-eta4;
	if (angleChange < -PI) {
		angleChange = angleChange+2*PI;
	}
	else if (angleChange > PI) {
		angleChange = angleChange-2*PI;
	}
	deta4f = -(angleChange)/epsilon;

	z1 = u-eta3;
	z2 = psi-eta4;
	if (z2 < -PI) {
		z2 = z2+2*PI;
	}
	else if (z2 > PI) {
		z2 = z2-2*PI;
	}

	eta5 = -K4*z2+deta4f-e1s*u*cos(eta4)*(cos(z2)-1)/(z2+epsilon)+e1s*u*sin(eta4)*sin(z2)/(z2+epsilon)-e2s*u*sin(eta4)*(cos(z2)-1)/(z2+epsilon)-e2s*u*cos(eta4)*sin(z2)/(z2+epsilon);
	if (abs(z2) < (PI/180)) {
		eta5 = 0;
	}
	deta5f = -(eta5f-eta5)/epsilon;

	z3 = r-eta5;

	delta1 = -K3*z1-e1s*cos(eta4)-e2s*sin(eta4)+m1e*deta3f-m2e*v*r+d1e*u-(rho1*abs(deta3f)+rho2*abs(v*r)+rho4*abs(u)+rho6)*copysign(1.0,z1);
	delta2 = -K5*z3-z2-(m1e-m2e)*u*v+d3e*r+m3e*deta5f-((rho1+rho2)*abs(u*v)+rho5*abs(r)+rho3*abs(deta5f)+rho7)*copysign(1.0,z3);

	DESIRED_PORTTHRUSTER = (delta1-delta2/L)/2;
	DESIRED_STARBOARDTHRUSTER = (delta1+delta2/L)/2;

	if (abs(ud) < 0.01) {
		DESIRED_PORTTHRUSTER = 0;
		DESIRED_STARBOARDTHRUSTER = 0;
	}
	
	if (abs(DESIRED_PORTTHRUSTER) > Fmax) {
		DESIRED_PORTTHRUSTER = copysign(Fmax, DESIRED_PORTTHRUSTER);
	}
	
	if (abs(DESIRED_STARBOARDTHRUSTER) > Fmax) {
		DESIRED_STARBOARDTHRUSTER = copysign(Fmax, DESIRED_STARBOARDTHRUSTER);
	}

	dT = t-lastTime;

	eta3f = eta3f+deta3f*dT;
	eta4f = eta4f+deta4f*dT;
	eta5f = eta5f+deta5f*dT;

	lastTime = t;

	sprintf(debugBuf, "e1:%.2f,e2:%.2f,eta1:%.2f,eta2:%.2f,eta3:%.2f,eta4:%.2f,eta5:%.2f,eta3f:%.2f,eta4f:%.2f,eta5f:%.2f,z1:%.2f,z2:%.2f,z3:%.2f", e1s, e2s, eta1, eta2, eta3, eta4, eta5, eta3f, eta4f, eta5f, z1, z2, z3);
	debug = debugBuf;

	Notify("DESIRED_PORTTHRUSTER", DESIRED_PORTTHRUSTER);
	Notify("DESIRED_STARBOARDTHRUSTER", DESIRED_STARBOARDTHRUSTER);
	Notify("e1out", e1out);
	Notify("e2out", e2out);
	Notify("DEBUG", debug);

	return true;

} 