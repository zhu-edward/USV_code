/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: patterndefinition.cpp                                */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Defines functions in patterndefinition.h                   */
/*****************************************************************/

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "patterndefinition.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace std;

//--------------------------------------------------------------------
// Procedure: Constructor

PatternDefinition::PatternDefinition() {

}

//--------------------------------------------------------------------
// Procedure: OnNewMail()

bool PatternDefinition::OnNewMail(MOOSMSG_LIST &NewMail) 
{
	return true;
}

//--------------------------------------------------------------------
// Procedure: OnStartUp()
// Initialize the starting position and the time by reading from the
// configuration file. If file is not present, use the default value

bool PatternDefinition::OnStartUp() {
	// Initialize controller parameters
	
	height = 20;
	if(!m_MissionReader.GetConfigurationParam("Height",height))
	    MOOSTrace("Warning parameter \"Height\" not specified. Using default of \"%f\"\n",height);
	if (height < 10) {
		height = 10;
		MOOSTrace("Warning parameter \"Height\" too small. Using minimum height of \"%f\"\n",height);
	}

	base = 40;
	if(!m_MissionReader.GetConfigurationParam("Base",base))
	    MOOSTrace("Warning parameter \"Base\" not specified. Using default of \"%f\"\n",base);
	if (base < 20) {
		height = 20;
		MOOSTrace("Warning parameter \"Base\" too small. Using minimum height of \"%f\"\n",base);
	}

	// Pattern is a triangle with followers at the verticies and the leader at the centroid
	// using this information calculate the pattern definition coordinates centered around the 
	// leader being at the origin

	leader.X = 0;
	leader.Y = 0;

	follower1.X = 0;
	follower1.Y = 2*height/3;

	follower2.X = -base/2;
	follower2.Y = -height/3;

	follower3.X = base/2;
	follower3.Y = -height/3;

	sprintf(buffer, "P0_X=%4.1f,P0_Y=%4.1f,P1_X=%4.1f,P1_Y=%4.1f,P2_X=%4.1f,P2_Y=%4.1f,P3_X=%4.1f,P3_Y=%4.1f", leader.X, leader.Y, follower1.X, follower1.Y, follower2.X, follower2.Y, follower3.X, follower3.Y);
	//sprintf(buffer, "P0_X=%4.1f,P0_Y=%4.1f", leader.X, leader.Y);

	pattern = buffer;

	return true;
}

bool PatternDefinition::OnConnectToServer() {

	return true;
}

bool PatternDefinition::Iterate() {

	Notify("pattern",pattern);
	/*Notify("P0_X", leader.X);
	Notify("P0_Y", leader.Y);
	Notify("P1_X", follower1.X);
	Notify("P1_Y", follower1.Y);
	Notify("P2_X", follower2.X);
	Notify("P2_Y", follower2.Y);
	Notify("P3_X", follower3.X);
	Notify("P3_Y", follower3.Y);*/

	return true;
}