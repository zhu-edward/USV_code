#ifndef JOYSTICKMAPPER_H_
#define JOYSTICKMAPPER_H_

//*****************************************************************************
//Includes
#include <stdio.h>
#include <MOOS/libMOOS/MOOSLib.h>
#include <mutex>
#include <algorithm>
#include <set>

const char * const buttonMappingPubs[] = {
    "VideoRay_CameraCmd",
    "VideoRay_CameraCmd",
    "VideoRay_CameraCmd",
    "VideoRay_CameraCmd",
    NULL,
    "VideoRay_LightCmd",
    NULL,
    "VideoRay_LightCmd",
    NULL,
    "VideoRay_CameraCmd",
    "VideoRay_AutoCmd",
    "VideoRay_AutoCmd",
};

const char * const buttonMappingKeys[] = { 
    "CameraButton",
    "CameraButton",
    "CameraButton", 
    "CameraButton",
    NULL,
    "Lights",
    NULL,
    "Lights",
    NULL,
    "CameraButton",
    "AutoHeading",
    "AutoDepth"
};


const char * const buttonMappingValues[] = { 
    "left",
    "down",
    "right",
    "up",
    NULL,
    "up",
    NULL,
    "down",
    NULL,
    "on",
    "toggle_current",
    "toggle_current"};




const char * const axesMapping[] = {
    NULL,
    "Vertical",
    "Rotate",
    "Translate",
    NULL,
    "CameraTilt"
};


const char * const allPubs[] = {
    "ROV_MOTION_REQUEST",
    "VideoRay_CameraCmd",
    "VideoRay_LightCmd",
    "VideoRay_AutoCmd"
};


struct inputMap {
    bool valid;
    std::string key;
    std::string value;
    std::string value2;
    int scale;
    std::string pub;
    unsigned int ID;
    unsigned int pubIdx;
};


struct pubEntry {
    std::string name;
    std::string msg;
};

class JoystickMapper : public CMOOSApp
{
	public:
		JoystickMapper();
		~JoystickMapper();
		

    std::vector <inputMap> buttonMapping;
    std::vector <inputMap> axisMapping;
    
    // Monitor which pubs are axis, these need to be handled by iterate
    std::set <int> axisPubs;
    std::set <int> buttonPubs;
    



	protected:
		// overloaded functions from CMOOSApp
		bool OnStartUp();
		bool Iterate();	
		bool OnConnectToServer();
		bool OnNewMail(MOOSMSG_LIST &NewMail);

        void publishData();


		// private attributes
		int m_nAppTick;		//frequency at which the app runs
		bool m_bVerbose;	//will tell us whether or not the program shall use MOOSTrace

    std::string name; // Name of this process (used to know if the videoray is under teleop)
    bool inControl;

    // buttons and axes retrieved from the joystick app
    //std::vector<int> buttons;
    //std::vector<int> axes;
    
    int deadband;
    
    std::vector<std::string> pubNames;
    std::vector<std::string> pubList;
    
    std::mutex publish_mutex;
    
    void determineButtonMapping(std::vector<int> buttons);
    void determineAxisMapping(std::vector<int> axes);

    unsigned int addToPubList(std::string pub);
    
    // Configuration parsing
    std::vector<std::string>  GetRepeatedConfigurations(const std::string & token);

};
#endif /*JOYSTICKAPP_H_*/
