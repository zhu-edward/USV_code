/*
 * simple MOOSApp test example
 */

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <math.h>

class VehicleApp : public CMOOSApp
{
	bool OnStartUp(){

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

    		//here we extract the vehicle name..
		m_sVehicleName = "UnNamed";
		if(!m_MissionReader.GetConfigurationParam("VehicleName",m_sVehicleName))
		    MOOSTrace("Warning parameter \"VechicleName\" not specified. Using default of \"%s\"\n",m_sVehicleName.c_str());

		x=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("InitialLocation_x",x))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",x);
		
		y=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("InitialLocation_y",y))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",y);

		psi=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("InitialLocation_psi",psi))
		MOOSTrace("Warning parameter \"InitialLocation\" not specified. Using default of \"%f\"\n",psi);

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
		
		//MOOSTrace("input_FL=%f, input_FR=%f, time=%f\n",input_FL,input_FR,MOOSTime());

		t_r=MOOSTime();
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
			if(MOOSStrCmp(rMsg.GetKey(),"DESIRED_PORTTHRUSTER"))
			{
				//this message is about something called "DESIRED_PORTTHRUSTER"
				CMOOSMsg &Msg = rMsg;
				if(!Msg.IsDouble())
					return MOOSFail("Ouch - was promised \"DESIRED_PORTTHRUSTER\" would be a double");

				DESIRED_PORTTHRUSTER = Msg.GetDouble();
				//MOOSTrace("DESIRED_PORTTHRUSTER is %f\n",DESIRED_PORTTHRUSTER);//the actual heading
				//if you want to see all details in Msg, you can print a message by
				//Msg.Trace();
			}
			else if(MOOSStrCmp(rMsg.GetKey(),"DESIRED_STARBOARDTHRUSTER"))
			{
				//this message is about something called "Input_FR"
				CMOOSMsg &Msg1 = rMsg;
				if(!Msg1.IsDouble())
					return MOOSFail("Ouch - was promised \"DESIRED_STARBOARDTHRUSTER\" would be a double");

				DESIRED_STARBOARDTHRUSTER = Msg1.GetDouble();
				//MOOSTrace("DESIRED_STARBOARDTHRUSTER is %f\n",DESIRED_STARBOARDTHRUSTER);//the actual heading
				//if you want to see all details in Msg, you can print a message by
				//Msg.Trace();
			}						
		}
		
		return true;
	}

	bool OnConnectToServer(){
		//register to be told about every change (write) to "VehicleStatus"
		Register("DESIRED_PORTTHRUSTER",0);
		Register("DESIRED_STARBOARDTHRUSTER",0);

    
		return true;	
	
	}

	bool Iterate()
	{
		//u=eta1;
		//r=eta2;
		//Calculate new state
		dx=u*cos(psi)-v*sin(psi);
		dy=u*sin(psi)+v*cos(psi);
		dpsi=r;
		dv=(-m11*u*r-d22*v-D22)/m22;
		du=(DESIRED_PORTTHRUSTER+DESIRED_STARBOARDTHRUSTER+m22*v*r-d11*u-D11)/m11;
		dr=(L*(DESIRED_STARBOARDTHRUSTER-DESIRED_PORTTHRUSTER)+(m11-m22)*u*v-d33*r-D33)/m33;

		

		dT=MOOSTime()-t_r;
		t_r=MOOSTime();	//Update t_r

		x=x+dT*dx;
		y=y+dT*dy;
		psi=psi+dT*dpsi;
		u=u+dT*du;
		v=v+dT*dv;
		r=r+dT*dr;
		
		//Update state to MOOSDB

		Notify("NAV_X",x);
		Notify("NAV_Y",y);
		Notify("NAV_LAT",y);
		Notify("NAV_LONG",x);
		Notify("NAV_HEADING",psi);
		Notify("NAV_SPEED",u);
		Notify("V",v);
		Notify("R",r);
//		MOOSTrace("m11=%f, m22=%f, m33=%f, d11=%f, d22=%f, d33=%f, D11=%f, D22=%f, D33=%f\n",m11,m22,m33,d11,d22,d33,D11,D22,D33);
//		MOOSTrace("input_FL=%f, input_FR=%f, time=%f\n",input_FL,input_FR,MOOSTime());
//		MOOSTrace("x=%f, y=%f, psi=%f, u=%f, v=%f, r=%f\n",x,y,psi,u,v,r);
//		MOOSTrace("eta1=%f, eta2=%f, time=%f\n",eta1,eta2,MOOSTime());
		return true;


	}
	
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

int main(int argc, char * argv[])
{
	//set up some default application parameters

    //what's the name of the configuration file that the application
    //should look in if it needs to read parameters?
    const char * sMissionFile = "controller.moos";

    //under what name shoud the application register with the MOOSDB?
    const char * sMOOSName = "vehicle";

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
    VehicleApp TheApp;

    //run forever pasing registration name and mission file parameters
    TheApp.Run(sMOOSName,sMissionFile);

    //probably will never get here..
    return 0;
}
