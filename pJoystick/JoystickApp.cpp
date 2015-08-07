/**
* 
* 
*   
*
*/
#include "JoystickApp.h"
#include <time.h>	 //for localtime
#include <unistd.h>  //for usleep
#include <string.h>


//****************************************************************************
//Author:	
//Date:		
//Desc:		
//			
//****************************************************************************
JoystickApp::JoystickApp() :
    devPath("")
{
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(100);
    openAttempts=0;
}

//****************************************************************************
//Author:	
//Date:		
//Desc:		JoystickApp destructor.  
//****************************************************************************
JoystickApp::~JoystickApp()
{
    joystick.closefd();
}



//****************************************************************************
//Author:	
//Date:		
//Desc:		Mail handler for JoystickApp MOOS App
//****************************************************************************
bool JoystickApp::OnNewMail(MOOSMSG_LIST &NewMail)
{	
	return true;
}


void JoystickApp::publishData() {

    std::stringstream axesState;


    axesState << "[" << joystick.axes.size() << "]{";
    for (unsigned int i=0; i< joystick.axes.size(); i++) {
        axesState << joystick.axes[i];
        if ( i != joystick.axes.size()-1)
            axesState << ",";
    }
    axesState << "}";
    


    std::stringstream buttonsState;
    buttonsState << "[" << joystick.buttons.size() << "]{";
    for (unsigned int i=0; i< joystick.buttons.size(); i++) {
        buttonsState << joystick.buttons[i];
        if ( i != joystick.buttons.size()-1)
            buttonsState << ",";
    }
    buttonsState << "}";

    m_Comms.Notify("Joystick_Axes",axesState.str(),MOOSTime());
    m_Comms.Notify("Joystick_Buttons",buttonsState.str(),MOOSTime());
    
}    

//****************************************************************************
//Author:	
//Date:		
//Desc:		Iteration loop for JoystickApp.  
//****************************************************************************
bool JoystickApp::Iterate()
{
    if (!opened) {
        opened=joystick.open(devPath);
        if (!opened) {
            openAttempts++;
            if (openAttempts%10==0) 
                std::cerr << "Error opening joystick device!" << std::endl;
            return true;
        }
    }

    // Get events from the joystick and publish them 
    while(joystick.getJoystickEvent())
    {
        //std::cerr << "JoystickApp: got joystick event, publishing joystick" << std::endl;
        publishData();
    }
 
	return true;
}

//****************************************************************************
//Author:	
//Date:		
//Desc:		The function sets up the MOOS Variable notifications.    
//****************************************************************************
bool JoystickApp::OnConnectToServer()
{
	return true;
}


//****************************************************************************
//Author:	
//Date:		
//Desc:		Startup function for MOOSApp.  Grabs initialization variables from
//	.moos file, opens serial port, and initializes JoystickApp settings.
//****************************************************************************
bool JoystickApp::OnStartUp()
{
    CMOOSApp::OnStartUp();

	m_MissionReader.GetConfigurationParam("AppTick", m_nAppTick);
	m_MissionReader.GetConfigurationParam("Verbosity", m_bVerbose);
	m_MissionReader.GetConfigurationParam("DevPath", devPath);


    opened=joystick.open(devPath);
    return true;

}

