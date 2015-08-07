#ifndef JOYSTICKAPP_H_
#define JOYSTICKAPP_H_

//*****************************************************************************
//Includes
#include <stdio.h>
#include <MOOS/libMOOS/MOOSLib.h>
#include <linux/joystick.h>
#include "Joystick.h"

class JoystickApp : public CMOOSApp
{
	public:
		JoystickApp();
		~JoystickApp();
		
	protected:
		// overloaded functions from CMOOSInstrument
		bool OnStartUp();
		bool Iterate();	
		bool OnConnectToServer();
		bool OnNewMail(MOOSMSG_LIST &NewMail);

        void publishData();


		// private attributes
		int m_nAppTick;		//frequency at which the app runs
		bool m_bVerbose;	//will tell us whether or not the program shall use MOOSTrace

        std::string devPath;
		
        Joystick joystick;
        bool opened;
        unsigned int openAttempts;


};
#endif /*JOYSTICKAPP_H_*/
