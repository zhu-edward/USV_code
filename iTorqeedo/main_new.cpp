//
// file: main.cpp
// author: Ben Axelrod
// date: June 18, 2013
// 
// simple "main" file which serves to build and run a CMOOSApp-derived application
//

#include "iTorqeedo.h"

#include <signal.h>
#include <string.h>

CiTorqeedo TheApp;

void got_signal(int)
{
    TheApp.RequestQuit();
};

int main(int argc ,char * argv[])
{
    //here we do some command line parsing...
    MOOS::CommandLineParser P(argc, argv);

    //mission file could be first free parameter
    std::string mission_file = P.GetFreeParameter(0, "Mission.moos");

    //app name can be the  second free parameter
    std::string app_name = P.GetFreeParameter(1, "iTorqeedo");

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = got_signal;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    //run forever pasing registration name and mission file parameters
    TheApp.Run(app_name, mission_file, argc, argv);

    //probably will never get here..
    return 0;
}

