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
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include "JoystickMapper.h"


using namespace std;

#define BUFFER_LEN 256

int main(int argc, char* argv[])
{
  int  nError = 0;
  bool bRun = true;
  char sMissionFile[BUFFER_LEN];
  char sMOOSName[BUFFER_LEN];
  char sSvnVersion[BUFFER_LEN];

  strcpy(sMissionFile, (char*)"JoystickMapper.moos");
  strcpy(sMOOSName,    (char*)"pJoystickMapper");
  strcpy(sSvnVersion,  (char*)"Unknown");

#ifndef _WIN32
#ifdef SVN_REVISION
  strcpy(sSvnVersion, SVN_REVISION);
#endif
#endif

  printf("%s version %s\n", argv[0], sSvnVersion);

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

    JoystickMapper jm;
    if (jm.Run(sMOOSName, sMissionFile) == false)
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
