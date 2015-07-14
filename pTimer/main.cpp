/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: timer.h                                              */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Main function that executes the process                    */
/*****************************************************************/

#include <iostream>
#include "timer.h"

using namespace std;

int main(int argc, char * argv[])
{
	//set up some default application parameters

    //what's the name of the configuration file that the application
    //should look in if it needs to read parameters?
    const char * sMissionFile = "HelmTimer.moos";

    //under what name shoud the application register with the MOOSDB?
    const char * sMOOSName = "Timer";

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
    Timer timer;

    //run forever pasing registration name and mission file parameters
    timer.Run(sMOOSName,sMissionFile);

    //probably will never get here..
    return 0;
}