/**
* 
* 
*   
*
*/
#include "JoystickMapper.h"
#include <time.h>	 //for localtime
#include <unistd.h>  //for usleep
#include <string.h>
#include <math.h>
#include <sstream>

bool validAxis(int val, int deadband) {
    if (abs(val)<deadband)
        return false;
    return true;
}



int applyDeadband(int val, int deadband) {
    if (abs(val)<deadband)
        return 0;

    if (val<0)
        return val+deadband;
    else
        return val-deadband;
}


// Helper function, as MOOS does not have a moosvectorfromstring for a signed int vector
bool MOOSVectorFromString(const std::string & sStr,std::vector<int> & dfValVec,int & nRows, int & nCols)
{

    /*unsigned int*/ size_t  nPos = sStr.find('[');

    if(nPos==std::string::npos)
        return false;

    nRows = atoi( sStr.data()+nPos+1);


    //if we have [456] then implicitlyt we mean [456x1]
    /*unsigned int*/ size_t  nXPos = sStr.find('x',nPos);

    nCols = 1;
    if(nXPos!=std::string::npos)
    {
        nCols = atoi( sStr.data()+nXPos+1);
        nPos = nXPos;
    }

    nPos = sStr.find('{',nPos);

    if(nPos==std::string::npos)
        return false;


    if(nCols==0 ||nRows==0)
        return false;

    dfValVec.clear();
    dfValVec.reserve(nRows*nCols);


    const char * pStr = sStr.data();
    for(int i = 1; i<=nRows;i++)
    {
        for(int j = 1; j<=nCols;j++)
        {
            int dfVal = atoi(pStr+nPos+1);

            dfValVec.push_back(dfVal);
            nPos = sStr.find(',',nPos+1);
        }
    }

    return true;

}




//****************************************************************************
//Author:	
//Date:		
//Desc:		
//			
//****************************************************************************
JoystickMapper::JoystickMapper() :
    inControl(false),
    name(""),
    deadband(2000)
{
    //some sensible defaults (missionfile can overwrite this)
    SetAppFreq(100);
}

//****************************************************************************
//Author:	
//Date:		
//Desc:		JoystickMapper destructor.  
//****************************************************************************
JoystickMapper::~JoystickMapper()
{
}



void JoystickMapper::determineButtonMapping(std::vector<int> buttons) {

    std::vector<std::string> buttonCfg = GetRepeatedConfigurations("Button");
    
    // Initialize the buttonMapping, for every button, flag it invalid
    /*
    inputMap defMap;
    defMap.valid=false;
    defMap.key="";
    defMap.value="";
    defMap.pub="";
    for (unsigned int i=0; i<buttons.size(); i++) {
        buttonMapping.push_back(defMap);
    }
    */
    
    for(std::vector<std::string>::iterator q=buttonCfg.begin(); q!=buttonCfg.end(); q++) {
        std::string cfgLine = *q;
        MOOSRemoveChars(cfgLine, " ");
        
        unsigned int idNum;
        if (!MOOSValFromString(idNum,cfgLine,"ID")) {
            std::cerr << "JoystickMapper: no ID specified for button in configuration mapping:" 
                      << cfgLine << std::endl;
            continue;
        }
        
        inputMap bMap;
        bMap.valid=true;
        bMap.ID=idNum;
        if (!MOOSValFromString(bMap.pub,cfgLine,"PublishName")) {
            std::cerr << "JoystickMapper: no PublishName specified for button in configuration mapping:"
                      << cfgLine << std::endl;
            continue;
        }

        // Add to the publist if it doesn't already exist, record the index to the pub entry
        bMap.pubIdx=addToPubList(bMap.pub);
        buttonPubs.insert(bMap.pubIdx);
                
        
        // Get all the optional fields for the button
        MOOSValFromString(bMap.key,cfgLine,"Key");
        MOOSValFromString(bMap.value,cfgLine,"Value");

        buttonMapping.push_back(bMap);
        

    }
    
    //std::cerr << "Button mapping:" << std::endl;
    for (unsigned int i=0; i<buttonMapping.size(); i++) {
        std::cerr << "Button " << buttonMapping[i].ID << ": ";
        if (buttonMapping[i].valid) {
            if (!buttonMapping[i].key.empty())
                std::cerr << "Key=" << buttonMapping[i].key << " ";
            if (!buttonMapping[i].value.empty())
                std::cerr << "Value=" << buttonMapping[i].value << " ";
            std::cerr << "PubIdx=" << buttonMapping[i].pubIdx << " ";
            std::cerr << "PubName=" << buttonMapping[i].pub << std::endl;

        } else {
            std::cerr << "INVALID" << std::endl;
        }
    }

}


void JoystickMapper::determineAxisMapping(std::vector<int> axes) {

    std::vector<std::string> axisCfg = GetRepeatedConfigurations("Axis");
    
    // Initialize the buttonMapping, for every button, flag it invalid
    /*
    inputMap defMap;
    defMap.valid=false;
    defMap.key="";
    defMap.value="";
    defMap.pub="";
    for (unsigned int i=0; i<axes.size(); i++)
        axisMapping.push_back(defMap);
    */

    for(std::vector<std::string>::iterator q=axisCfg.begin(); q!=axisCfg.end(); q++) {
        std::string cfgLine = *q;
        MOOSRemoveChars(cfgLine, " ");
        
        unsigned int idNum;
        if (!MOOSValFromString(idNum,cfgLine,"ID")) {
            std::cerr << "JoystickMapper: no ID specified for axis in configuration mapping:" 
                      << cfgLine << std::endl;
            continue;
        }
        
        inputMap bMap;
        bMap.valid=true;
        bMap.scale=1;
        bMap.ID = idNum;
        if (!MOOSValFromString(bMap.pub,cfgLine,"PublishName")) {
            std::cerr << "JoystickMapper: no PublishName specified for axis in configuration mapping:"
                      << cfgLine << std::endl;
            continue;
        }

        // Add to the publist if it doesn't already exist, record the index to the pub entry
        bMap.pubIdx=addToPubList(bMap.pub);
        axisPubs.insert(bMap.pubIdx);

        
        // Get all the optional fields for the button
        MOOSValFromString(bMap.key,cfgLine,"Key");
        if (MOOSValFromString(bMap.value,cfgLine,"ValueLow"))
            MOOSValFromString(bMap.value2,cfgLine,"ValueHigh");
        
        MOOSValFromString(bMap.scale,cfgLine,"Scale");

        axisMapping.push_back(bMap);
        

    }
    
    std::cerr << "Axis mapping:" << std::endl;
    for (unsigned int i=0; i<axisMapping.size(); i++) {
        std::cerr << "Axis " << axisMapping[i].ID << ": ";
        if (axisMapping[i].valid) {
            if (!axisMapping[i].key.empty())
                std::cerr << "Key=" << axisMapping[i].key << " ";
            if (!axisMapping[i].value.empty())
                std::cerr << "Value=" << axisMapping[i].value << " ";
            std::cerr << "PubIdx=" << axisMapping[i].pubIdx << " ";
            std::cerr << "PubName=" << axisMapping[i].pub << std::endl;

        } else {
            std::cerr << "INVALID" << std::endl;
        }
    }

    
    

}


unsigned int JoystickMapper::addToPubList(std::string pub) {
    for (unsigned int i=0; i<pubNames.size(); i++) {
        if (pubNames[i]==pub)
            return i;
    }

    pubNames.push_back(pub);
    pubList.resize(pubNames.size());
    
    return pubNames.size()-1;

}


//****************************************************************************
//Author:	
//Date:		
//Desc:		Mail handler for JoystickMapper MOOS App
//****************************************************************************
bool JoystickMapper::OnNewMail(MOOSMSG_LIST &NewMail)
{	

    CMOOSMsg Msg;
    double dfNow = MOOSTime();
    
    // Process all mail and send an event for each, otherwise we may miss button presses
    MOOSMSG_LIST::iterator p;





    // I hope these are in order from oldest to newest
    for (p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg & rMsg = *p;




        if (MOOSStrCmp(rMsg.GetKey(),"VehicleTeleop")) {
            if (rMsg.GetString()==name) {
                if (!inControl) std::cerr << "JoystickMapper Teleop now in control!" << std::endl;
                inControl=true;
            } else {
                if (inControl) std::cerr << "JoystickMapper Teleop is no longer in control!" << std::endl;
                inControl=false;
            }

        } else if ((MOOSStrCmp(rMsg.GetKey(),"Joystick_Buttons")) && inControl) {

            // Clear out the buttons in publist
            for(std::set<int>::iterator q=buttonPubs.begin(); q!=buttonPubs.end(); q++) 
                pubList[*q]="";


            int rows, cols;
            std::vector<int> buttons;
            MOOSVectorFromString(rMsg.GetString(), buttons, rows, cols);

            // First button publication, build up the mapping of button inputs
            if (buttonMapping.size()==0) 
                determineButtonMapping(buttons);
          
            // Go through all the buttons. For any pressed buttons, determine the publication name, and key value pair
            // that needs to be published and append that to the proper publication string
            // If the key value is null, then just put the actual button value in
            //for (unsigned int i=0; i < buttons.size(); i++) {
            for (unsigned int i=0; i < buttonMapping.size(); i++) {
                // If pressed
                if (buttons[buttonMapping[i].ID]) {
                    int pubIdx=buttonMapping[i].pubIdx;

                    if (pubList[pubIdx].size()>0)
                        pubList[pubIdx]+=",";

                    // Add the key if specified
                    if (!buttonMapping[i].key.empty()) {
                        pubList[pubIdx]+=buttonMapping[i].key;
                        pubList[pubIdx]+="=";
                    }

                    // Add the value if specified, otherwise the raw button return is used
                    if (buttonMapping[i].value.empty())
                        pubList[pubIdx]+=std::to_string((long long int)buttons[buttonMapping[i].ID]);
                    else
                        pubList[pubIdx]+=buttonMapping[i].value;
                }
            }
            
        } else if ((MOOSStrCmp(rMsg.GetKey(),"Joystick_Axes")) && inControl) {
            int rows, cols;
            std::vector<int> axes;
            MOOSVectorFromString(rMsg.GetString(), axes, rows, cols);

            // First axis publication, build up the mapping of axisinputs
            if (axisMapping.size()==0) 
                determineAxisMapping(axes);

            // A deadband is applied to all axes.  Normally, the max value would be 32767.
            // When the deadband is applied, this lowers the limit.
            double adjustedLimit = 32767.0-deadband;

            // Clear out the axes in publist
            for(std::set<int>::iterator q=axisPubs.begin(); q!=axisPubs.end(); q++) 
                pubList[*q]="";
            

            for (unsigned int i=0; i< axisMapping.size(); i++) {

                //for (unsigned int i=0; i < axes.size(); i++) {
                //if (axisMapping[i].valid) {
                publish_mutex.lock();

                int pubIdx = axisMapping[i].pubIdx;

                if (pubList[pubIdx].size()>0)
                    pubList[pubIdx]+=",";
                    
                // Add the key if specified
                if (!axisMapping[i].key.empty()) {
                    pubList[pubIdx]+=axisMapping[i].key;
                    pubList[pubIdx]+="=";
                }

                // Determine the type of value to add
                if(axisMapping[i].value.empty()) {
                    // If inside the deadband, set to zero, but still send, so we stop moving
                    // If outside, subtract off the deadband so there is still a nice linear progression
                    // from 0 to 100 thrust.
                    axes[axisMapping[i].ID]=applyDeadband(axes[axisMapping[i].ID],deadband);
                    double normVal=axes[axisMapping[i].ID]/adjustedLimit;
                    pubList[pubIdx]+=std::to_string((long long int)round(axisMapping[i].scale*normVal));
                    
                } else {
                    // There were specified high/low values
                    if (axes[axisMapping[i].ID]>0) {
                        pubList[pubIdx]+=axisMapping[i].value;
                    } else if (axes[i]<0) {
                        pubList[pubIdx]+=axisMapping[i].value2;
                    }
                }
                publish_mutex.unlock();
                
            }


 
        }

        if (inControl) {
            for (unsigned int i=0; i<pubList.size(); i++) {
                if (pubList[i].size()>0) {
                    publish_mutex.lock();
                    m_Comms.Notify(pubNames[i],pubList[i],MOOSTime());
                    publish_mutex.unlock();

                } else {
                    //std::cerr << "Empty publication, nothing to publish" << std::endl;
                }
            }
            
        }            
        
       

    }
    
    
    return true;
}


void JoystickMapper::publishData() {
}    

//****************************************************************************
//Author:	
//Date:		
//Desc:		Iteration loop for JoystickMapper.  
//****************************************************************************
bool JoystickMapper::Iterate()
{
 
    
    publish_mutex.lock();

    for(std::set<int>::iterator q=axisPubs.begin(); q!=axisPubs.end(); q++) {
        if (pubList[*q].size()>0) {
            m_Comms.Notify(pubNames[*q],pubList[*q],MOOSTime());
        }
    }

    publish_mutex.unlock();
    
	return true;
}

//****************************************************************************
//Author:	
//Date:		
//Desc:		The function sets up the MOOS Variable notifications.    
//****************************************************************************
bool JoystickMapper::OnConnectToServer()
{
    m_Comms.Register("Joystick_Buttons",0);
    m_Comms.Register("Joystick_Axes",0);
    m_Comms.Register("VehicleTeleop",0);

	return true;
}


//****************************************************************************
//Author:	
//Date:		
//Desc:		Startup function for MOOSApp.  Grabs initialization variables from
//	.moos file, opens serial port, and initializes JoystickMapper settings.
//****************************************************************************
bool JoystickMapper::OnStartUp()
{
    CMOOSApp::OnStartUp();

	m_MissionReader.GetConfigurationParam("AppTick", m_nAppTick);
	m_MissionReader.GetConfigurationParam("Verbosity", m_bVerbose);
	m_MissionReader.GetConfigurationParam("VehicleName", name);
	m_MissionReader.GetConfigurationParam("Deadband", deadband);


    return true;

}


std::vector<std::string> JoystickMapper::GetRepeatedConfigurations(const std::string & token)
{
	STRING_LIST params;
	m_MissionReader.GetConfiguration( GetAppName(),params);

	STRING_LIST::iterator q;
	std::vector<std::string> results;
	for(q=params.begin(); q!=params.end();q++)
	{
		std::string tok,val;

		m_MissionReader.GetTokenValPair(*q,tok,val,false);

		if(MOOSStrCmp(tok,token))
		{
			results.push_back(val);
		}
	}
	return results;
}

