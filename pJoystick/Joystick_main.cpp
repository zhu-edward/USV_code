// *****************************************************************************
//
//
// Author:	
// Date:	
// Function:	main
// Args:	command line args
// Return:	int (but doesn't mean anything, just to shut compiler warning off)
// Description:	This function will simply instantiate and then start the 
//		VideoRayComms MOOS process
//
// *****************************************************************************
#include <iostream>
#include "JoystickApp.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <signal.h>


using namespace std;

#define BUFFER_LEN 256

JoystickApp joy;

void got_signal(int)
{
    joy.RequestQuit();
};

int main(int argc, char* argv[])
{
  int  nError = 0;
  bool bRun = true;
  char sMissionFile[BUFFER_LEN];
  char sMOOSName[BUFFER_LEN];
  char sSvnVersion[BUFFER_LEN];

  strcpy(sMissionFile, (char*)"pJoystick.moos");
  strcpy(sMOOSName,    (char*)"pJoystick");
  strcpy(sSvnVersion,  (char*)"Unknown");

#ifndef _WIN32
#ifdef SVN_REVISION
  strcpy(sSvnVersion, SVN_REVISION);
#endif
#endif

  printf("%s version %s\n", argv[0], sSvnVersion);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = got_signal;
    sigfillset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

  if (argc > 1)
  {
    // IF IT IS A VERSION NUMBER REQUEST THEN JUST REPORT
    // THE VERSION NUMBER AND DON'T EXECUTE MODULE
    if (strncmp(argv[1], "-v", 2) == 0)
    {
      bRun = false;
    }
    else
    {
      if (argc == 3)
      {
        strcpy(sMOOSName,    (char*)argv[2]);
        strcpy(sMissionFile, (char*)argv[1]);
      }
      else if (argc == 2)
      {
        strcpy(sMissionFile, (char*)argv[1]);
      }
      else if (argc > 3)
      {
        nError = 1;
        bRun = false;
      }
    }
  }
  else
  {
    // EXECUTE WITH DEFAULT VALUES
  }

  

  if (bRun == true)
  {
    
    if (joy.Run(sMOOSName, sMissionFile) == false)
    {
      nError = 1; 
      MOOSTrace("Unable to execute module %s\n", sMOOSName); 
    }
      
  }
  else if (nError != 0)
  {
    MOOSTrace("Invalid number of parameters passed to module %s\n", sMOOSName); 
  }

  
  return(nError);
}
