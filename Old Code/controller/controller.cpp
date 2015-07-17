/**
 Controller MOOSApp 
 
It requires the state of a vehicle and a desired trajectory and generates the required forces for 
the vehice to track the desired trajectory

Inputs: 

	NAV_LAT: x coordinate
	NAV_LONG: y coordinate
	NAV_HEADING: psi
	NAV_SPEED: u
	V: v
	R: r

	Xd: xd is the desired position of x
	Yd: yd is the desired position of y
	Dxd: dxd is the derivative of xd
	Dyd: dyd is the derivative of yd
	Ddxd: ddxd is the derivative of dxd
	Ddyd: ddyd is the derivative of dyd

Outputs:

	

In real time control (x,y,psi,u,v,r) is provided by GPS/INS sensor. In simulation, (x,y,psi,u,v,r) is provided by veicle simulator (vehicle.cpp).


(xd,yd,dxd,dyd,ddxd,ddyd) is privoded by desiredTrajectoy.cpp
 **/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <math.h>
//#include <iostream>
#include <fstream>
//using namespace std;

class ExampleApp : public CMOOSApp
{
	bool OnStartUp(){
		x=0;	//default value
		y=0;
		psi=0;
		u=0;
		v=0;
		r=0;
		xd=0;
		yd=0;
//		psid=0;
		dxd=0;
		dyd=0;
//		dpsid=0;
		ddxd=0;
		ddyd=0;
//		ddpsid=0;
		t_s=MOOSTime();
		t_ref=0;
		Notify("T_ref",t_ref);

    		//here we extract the control parameters..

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

//MOOSTrace("Err_e_i=%f,Err_z_i=%f,K1_i=%f,K1_m=%f,K2_i=%f,K2_m=%f,K3_i=%f,K3_m=%f\n",Err_e_i,Err_z_i,K1_i,K1_m,K2_i,K2_m,K3_i,K3_m);
		return true;
	}

	bool OnNewMail(MOOSMSG_LIST & NewMail){
		//process it
/**		
		MOOSMSG_LIST::iterator p;
    
		for(p=NewMail.begin();p!=NewMail.end();p++)
		{
			//lets get a reference to the Message - no need for pointless copy
			CMOOSMsg & rMsg = *p;
    
			// repetitive "ifs" is one way to build a switch yard for
			// the messages
			p->Trace();
		}

**/
		//Retrieve informaiton
		MOOSMSG_LIST::iterator p1;
    
		for(p1=NewMail.begin();p1!=NewMail.end();p1++)
		{
			//lets get a reference to the Message - no need for pointless copy
			CMOOSMsg & rMsg = *p1;
    
			// repetitive "ifs" is one way to build a switch yard for
			// the messages
			if(MOOSStrCmp(rMsg.GetKey(),"NAV_LAT"))
			{
				//this message is about something called "NAV_LAT"
				CMOOSMsg &Msg = rMsg;
				if(!Msg.IsDouble())
					return MOOSFail("Ouch - was promised \"NAV_LAT\" would be a double");

				y = Msg.GetDouble();
				//MOOSTrace("Input_FL is %f\n",x);//the actual heading
				//if you want to see all details in Msg, you can print a message by
				//Msg.Trace();
			}
			else if(MOOSStrCmp(rMsg.GetKey(),"NAV_LONG"))
			{
				//this message is about something called "NAV_LONG"
				CMOOSMsg &Msg1 = rMsg;
				if(!Msg1.IsDouble())
					return MOOSFail("Ouch - was promised \"NAV_LONG\" would be a double");

				x = Msg1.GetDouble();
				//MOOSTrace("Y is %f\n",y);//the actual heading
				//if you want to see all details in Msg, you can print a message by
				//Msg.Trace();
			}			
			else if(MOOSStrCmp(rMsg.GetKey(),"NAV_HEADING"))
			{
				//this message is about something called "Psi"
				CMOOSMsg &Msg1 = rMsg;
				if(!Msg1.IsDouble())
					return MOOSFail("Ouch - was promised \"Psi\" would be a double");

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
/**			else if(MOOSStrCmp(rMsg.GetKey(),"NODE_REPORT_LOCAL"))
			{
				CMOOSMsg &Msg1 = rMsg;
				//if you want to see all details in Msg, you can print a message by
				Msg1.Trace();
			}
**/
		}
		
		return true;
	}

	bool OnConnectToServer(){
		//register to be told about vehicle state and desired trajectory
		Register("NAV_X",0);	//NAV_X: x. (X,y) and (xd,yd) should be in the same coordinate
		Register("NAV_Y",0);	//NAV_Y: y

		Register("NAV_LAT",0);	//NAV_LAT: y. (x,y) and (xd,yd) should be in the same coordinate
		Register("NAV_LONG",0);	//NAV_LONG: x
		Register("NAV_HEADING",0);	//NAV_HRADING: psi
    		Register("NAV_SPEED",0);	//NAV_SPEED: u
		Register("V",0);
		Register("R",0);
		Register("Xd",0);
		Register("Yd",0);
		Register("Dxd",0);
		Register("Dyd",0);
		Register("Ddxd",0);
		Register("Ddyd",0);
//		Register("NODE_REPORT_LOCAL",0);
		return true;	
	
	}

	bool Iterate()
	{

		e1=x-xd+epsilon*cos(psi);
		e2=y-yd+epsilon*sin(psi);


		if ((e1*e1+e2*e2)>Err_e_i)
		{
		       K1 = K1_i;
		}
		else
		{
		       K1=K1_m;
		}
    
		tp1=-K1*e1+sin(psi)*v+dxd;
		tp2=-K1*e2-cos(psi)*v+dyd;
		eta1=cos(psi)*tp1+sin(psi)*tp2;
		eta2=-sin(psi)*tp1/epsilon+cos(psi)*tp2/epsilon;
		z1=u-eta1;
		z2=r-eta2;

		if (z1*z1+z2*z2>Err_z_i)
		{
			K2 = K2_i;
			K3 = K3_i;
		}
		else
		{
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

		Notify("DESIRED_PORTTHRUSTER",DESIRED_PORTTHRUSTER);
		Notify("DESIRED_STARBOARDTHRUSTER",DESIRED_STARBOARDTHRUSTER);
//		Notify("E1",e1);
//		Notify("E2",e2);

		t_ref=MOOSTime()-t_s;

		Notify("T_ref",t_ref);

//		MOOSTrace("m11b=%f, m22b=%f, m33b=%f, d11b=%f, d22b=%f, d33b=%f\n",m11b,m22b,m33b,d11b,d22b,d33b);

//		MOOSTrace("input_FL=%f, input_FR=%f, time=%f\n",input_FL,input_FR,MOOSTime());
//		MOOSTrace("x=%f, y=%f, psi=%f, u=%f, v=%f, r=%f, time=%f\n",x,y,psi,u,v,r,MOOSTime());
//		MOOSTrace("xd=%f, yd=%f, psid=%f, dxd=%f, dyd=%f, dpsid=%f,ddxd=%f, ddyd=%f, ddpsid=%f, time=%f\n",xd,yd,psid,dxd,dyd,dpsid,ddxd,ddyd,ddpsid,MOOSTime());
//		MOOSTrace("K1=%f, K2=%f, K3=%f, epsilon=%f, time=%f\n",K1,K2,K3,epsilon,MOOSTime());
		MOOSTrace("xe=%f, ye=%f, time=%f\n",e1,e2,t_ref);
//		MOOSTrace("eta1=%f, eta2=%f, time=%f\n",eta1,eta2,MOOSTime());
//		MOOSPause(3000) ;


		return true;


	}
//	double NAV_X;
//	double NAV_Y;
//	double NAV_LAT;
//	double NAV_LONG;
//	double NAV_HEADING;
//	double NAV_SPEED;
	double NAV_YAW;
	double NAV_DEPTH;
	double t_s;
	double t_ref;
	double A;
	double B;
	double L;
	double x;	//(x,y) can be the GPS position (NAV_LAT,NAV_LONG) or local coordinate possition (NAV_X,NAV_Y)
	double y;
	double psi;	//NAV_HEADING
	double u;	//NAV_SPEED
	double v;
	double r;	
	double xd;	//(xd,yd) is the GPS coordibate position if (x,y) is the GPS pisition (NAV_LAT,NAV_LONG) or it is local coordinate possition if (x,y) is local coorinate position (NAV_X,NAV_Y). 
	double yd;
//	double psid;
	double dxd;
	double dyd;
//	double dpsid;

	double ddxd;
	double ddyd;
//	double ddpsid;

	double DESIRED_PORTTHRUSTER;
	double DESIRED_STARBOARDTHRUSTER;

	double Err_e_i;
	double Err_z_i;
	double K1_i;
	double K2_i;
	double K3_i;
	double K1_m;
	double K2_m;
	double K3_m;

	double K1;
	double K2;
	double K3;
	double epsilon;
	double e1;
	double e2;
	double de1;
	double de2;

	double eta1;
	double eta2;
	double eta1b;
	double eta2b;
	double deta1b;
	double deta2b;
	double tp1;
	double tp2;
	double dtp1;
	double dtp2;
	double z1;
	double z2;
	double zeta1;
	double zeta2;
	double m11b;
	double m22b;
	double m33b;
	double d11b;
	double d22b;
	double d33b;
	double rho1;
	double rho2;
	double rho3;
	double rho4;
	double rho5;
	double rho6;
	double rho7;
	double rho8;
	double rho9;
	double rho10;

//	std::string m_sVehicleName;
//	ofstream dataFile;

};

int main(int argc, char * argv[])
{
	//set up some default application parameters

    //what's the name of the configuration file that the application
    //should look in if it needs to read parameters?
    const char * sMissionFile = "controller.moos";

    //under what name shoud the application register with the MOOSDB?
    const char * sMOOSName = "controller";

    switch(argc)
    {
    case 3:
    //command line says don't register with default name
    sMOOSName = argv[2];
    case 2:
    //command line says don't use default "mission.moos" config file
    sMissionFile = argv[1];
    }

    //make an application
    ExampleApp TheApp;

    //run forever pasing registration name and mission file parameters
    TheApp.Run(sMOOSName,sMissionFile);

    //probably will never get here..
    return 0;
}
