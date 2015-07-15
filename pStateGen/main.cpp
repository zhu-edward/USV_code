/*****************************************************************/
/*    NAME: Edward Zhu										     */
/*    ORGN: 2015 NREIP CINT HAMMER Project, San Diego, CA        */
/*    FILE: main.cpp                                             */
/*    DATE: Jul 9th, 2015                                        */
/*																 */
/*    Main function that executes the process                    */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "ColorParse.h"
#include "stategen.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(i == 2)
      run_command = argi;
  }

  cout << termColor("green");
  cout << "pStateGen launching as " << run_command << endl;
  cout << termColor() << endl;

  StateGen stategen;
  stategen.Run(run_command.c_str(), mission_file.c_str(), argc, argv);

  return(0);
}