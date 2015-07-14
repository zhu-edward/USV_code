/*
 * simple MOOSApp test example
 */

#include "MOOS/libMOOS/App/MOOSApp.h"
#include <math.h>

class ExampleApp : public CMOOSApp
{
	bool OnStartUp(){
    		//here we extract the parameters..

		x_c=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("x_c",x_c))
		MOOSTrace("Warning parameter \"x_c\" not specified. Using default of \"%f\"\n",x_c);
		y_c=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("y_c",y_c))
		MOOSTrace("Warning parameter \"y_c\" not specified. Using default of \"%f\"\n",y_c);
		r=50;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("R",r))
		MOOSTrace("Warning parameter \"R\" not specified. Using default of \"%f\"\n",r);
		w=0.01;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("W",w))
		MOOSTrace("Warning parameter \"w\" not specified. Using default of \"%f\"\n",w);
		theta=0;	//default value
		//here we extract a double from the configuration file
		if(!m_MissionReader.GetConfigurationParam("Theta",theta))
		MOOSTrace("Warning parameter \"theta\" not specified. Using default of \"%f\"\n",theta);

		MOOSTrace("x_c=%f, y_c=%f, R=%f, w=%f, theta=%f\n",x_c,y_c,r,w,theta);

		t_i=MOOSTime();
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
		return true;
	}

	bool OnConnectToServer(){
		//register to be told about every change (write) to "VehicleStatus"
		//Register("Y",0);

    
		return true;	
	
	}

	bool Iterate()
	{
		t=MOOSTime()-t_i;
		xd=x_c+r*cos(w*t+theta);		
		yd=y_c+r*sin(w*t+theta);
		psid=w*t+theta;
		dxd=-r*w*sin(w*t+theta);		
		dyd=r*w*cos(w*t+theta);

		ddxd=-r*w*w*cos(w*t+theta);		
		ddyd=-r*w*w*sin(w*t+theta);

		Notify("Xd",xd);
		Notify("Yd",yd);
		Notify("Dxd",dxd);
		Notify("Dyd",dyd);
		Notify("Ddxd",ddxd);
		Notify("Ddyd",ddyd);
//		MOOSTrace("xd=%f, yd=%f, dxd=%f, dyd=%f, ddxd=%f, ddyd=%f, t=%f\n",xd,yd,dxd,dyd,ddxd,ddyd,t);

//		MOOSPause(3000) ;
		return true;


	}
	
	double t_i;
	double t;
	double x_c;	//center of the circle
	double y_c;	//center of the circle
	double r;	//Radius of the circle
	double w;	//frequency 
	double theta;	//angle in radian
	double xd;
	double yd;
	double psid;
	double dxd;
	double dyd;
	double dpsid;
	double ddxd;
	double ddyd;
	double ddpsid;

//	std::string m_sVehicleName;

};

int main(int argc, char * argv[])
{
	//set up some default application parameters

    //what's the name of the configuration file that the application
    //should look in if it needs to read parameters?
    const char * sMissionFile = "mission.moos";

    //under what name shoud the application register with the MOOSDB?
    const char * sMOOSName = "desiredTrajectory";

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
