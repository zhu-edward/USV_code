//
// file: iTorqeedo.cpp
// author: Ben Axelrod
// date: June 18, 2013
// 
// implementation for the CiTorqeedo class.
//

#include <termios.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>
#include <sys/time.h>
#include <errno.h>

#include "iTorqeedo_new.h"
#include "MOOS/libMOOS/App/MOOSApp.h"


// Packet delimiters
#define PACKET_START        0xAC
#define PACKET_END          0xAD
#define PACKET_ESCAPE       0xAE
#define PACKET_ESCAPE_MASK  0x80

// Addresses
#define BUS_MASTER 0x00
#define REMOTE1    0x14
#define DISPLAY    0x20
#define MOTOR      0x30
#define BATTERY    0x80


// Message IDs
//---------------------
// General messages (accepted by all)
#define INFO         0x00
// Remote messages
#define REMOTE       0x01
#define SETUP        0x02
// Display messages
#define SYSTEM_STATE 0x41
#define SYSTEM_SETUP 0x42
// Motor messages 
#define MOTOR_STATUS 0x01
#define DRIVE        0x82
#define PARAM        0x03
#define CONFIG       0x04
#define TEST         0xC1
#define CALIB        0xC2
#define HALL         0x43

//---------------------


//#define DEBUG

//default constructor
CiTorqeedo::CiTorqeedo()
{
#ifdef DEBUG
    printf("ctor\n");
#endif
    _name = "";
    _fd = -1;
    _speed = 0;
    _controlRate = 0.0;
    gettimeofday(&_lastCmd, NULL);
    _iteration = 0;
    _maxOutput = 1000;
    _sendHandController = false;
    _clearError = false;

    infoReceived = false;
    infoTime.tv_sec = 0;
    infoTime.tv_usec = 0;

    memset(recv_buff, 0, 500);
    memset(splice_buff, 0, 500);

}

//default (virtual) destructor
CiTorqeedo::~CiTorqeedo()
{
    if (_fd > 0)
    {
        // FAIL SAFE: stop motor
        for (int i=0; i<5; i++)
        {
            unsigned char msg[9] = {0xAC, 0x30, 0x82, 0x08, 0x00, 0x00, 0x00, 0xB9, 0xAD};
            int res = write(_fd, msg, 9);
            if (res <= 0)
                MOOSFail("write returned %d\n", res);
            
            usleep(50000);
        }
    
        int res = close(_fd);
        if (res <= 0)
            MOOSFail("close returned %d\n", res);
        _fd = -1;
    }
}

/** 
Called by base class whenever new mail has arrived.
Place your code for handling mail (notifications that something
has changed in the MOOSDB in this function

Parameters:
    NewMail :  std::list<CMOOSMsg> reference

Return values:
    return true if everything went OK
    return false if there was problem
**/
bool CiTorqeedo::OnNewMail(MOOSMSG_LIST &NewMail)
{
#ifdef DEBUG
    printf("new mail\n");
#endif
    MOOSMSG_LIST::iterator p;
    for(p=NewMail.begin(); p!=NewMail.end(); p++)
    {
        CMOOSMsg & rMsg = *p;
        
        if(MOOSStrCmp(rMsg.GetKey(), _name + "_CMD") && rMsg.IsDouble())
        {
            double val = rMsg.GetDouble();
#ifdef DEBUG
            printf("got val %f\n", val);
#endif
            if (val > 100.0)
            {
                MOOSFail("WARNING: value: %f not in range: (-100, 100)\n", val);
                val = 100;
            }
            if (val < -100.0)
            {
                MOOSFail("WARNING: value: %f not in range: (-100, 100)\n", val);
                val = -100;
            }
            gettimeofday(&_lastCmd, NULL);
            _speed = (int)round(_maxOutput * val / 100.0);
        } else if(MOOSStrCmp(rMsg.GetKey(), _name + "_RESET_CMD"))
        {
            std::string msgString = rMsg.GetString();
            std::transform(msgString.begin(), msgString.end(), msgString.begin(), ::tolower);
            _clearError=(msgString=="true");
        }
        else
            MOOSFail("WARNING: Got a message but it does not match desired format");
    }
    
    return true;
}

/**
called by the base class when the application has made contact with
the MOOSDB and a channel has been opened. Place code to specify what
notifications you want to receive here.
**/
bool CiTorqeedo::OnConnectToServer()
{
#ifdef DEBUG
    printf("on connect to server\n");
#endif

    //DoRegistrations();
    return true;
}

bool CiTorqeedo::OnDisconnectFromServer()
{
    MOOSTrace("WARNING: Disconnecting from server\n");
    
    // FAIL SAFE: stop motor
    for (int i=0; i<5; i++)
    {
        unsigned char msg[9] = {0xAC, 0x30, 0x82, 0x08, 0x00, 0x00, 0x00, 0xB9, 0xAD};
        int res = write(_fd, msg, 9);
        if (res <= 0)
            MOOSFail("write returned %d\n", res);
        
        usleep(50000);
    }
    
    int res = close(_fd);
    if (res <= 0)
        MOOSFail("close returned %d\n", res);
    _fd = -1;
    
    return true;
}

// Adjust the packet for any packet delimiters present (add escape byte and mask the delimiter)
void CiTorqeedo::maskDelimeters(unsigned char* msg, int &len) {
    // The first and last byte of the message is always AC, AD respectively
    // so ignore them
    for (unsigned int i=1; i<len-1; i++) {
        if (msg[i]>0xAB && msg[i]<0xAF) {
            unsigned char prev=msg[i];
           
            // Toggle the high bit for the special byte
            prev = (prev ^  PACKET_ESCAPE_MASK);

            // Add the special flag
            msg[i]=PACKET_ESCAPE;
            // Shift the data one byte due to the special flag
            for (unsigned int j=i+1; j<len+1; j++){
                //std::cerr << "shifting " << j << std::endl;
                unsigned char next=msg[j];
                msg[j]=prev;
                prev=next;
            }
            len++;
        }
    }
}


// Adjust the packet for any escape bytes and mask the delimiters present (unmask and remove escape byte)
void CiTorqeedo::unMaskDelimeters(unsigned char* msg, int &len) {
    // The first and last byte of the message is always AC, AD respectively
    // so ignore them
    for (unsigned int i=1; i<len-1; i++) {
        if (msg[i]==PACKET_ESCAPE) {
            // Toggle the high bit of the byte after the special byte to unmask the escape byte
            msg[i+1] = (msg[i+1] ^  PACKET_ESCAPE_MASK);

            // Shift the data one byte to the left to remove the special byte
            for (unsigned int j=i+1; j<len; j++) 
                msg[j-1]=msg[j];

            len=len-1;
        }
    }
}


void CiTorqeedo::emulateHandController() {
    

    int splice_loc = 0;
    
    unsigned char msg[20];
    memset(msg, 0, 20);
    int len = 0;

    
    int res = read(_fd, recv_buff, 499);
    if (res <= 0)
    {
        //printf("\nWARNING: read returned %d\n", res);
    } else {
        for (int i=0; i<res; i++) {
            // AC 14 01 89 AD
            splice_buff[splice_loc] = recv_buff[i];
            
            splice_loc++;
            
            //response is ~3ms after
            
            if (splice_loc >= 5 && recv_buff[i] == 0xAD)
            {
                if (splice_buff[splice_loc-2] == 0x89 &&
                    splice_buff[splice_loc-3] == 0x01 &&
                    splice_buff[splice_loc-4] == 0x14 &&
                    splice_buff[splice_loc-5] == 0xAC)
                {
                    splice_loc = 0;
                    
                        //AC 00 00 05 00 00 00 81 AD
                    msg[0] = 0xAC;
                    msg[1] = 0x00;
                    msg[2] = 0x00;
                    msg[3] = 0x05;
                    msg[4] = 0x00;
                    msg[5] = 0x00;
                    msg[6] = 0x00;
                    msg[7] = 0x81;
                    msg[8] = 0xAD;

                    if (abs(_speed) > 0) {

                        msg[5] = (_speed & 0x0000FF00) >> 8;
                        msg[6] = (_speed & 0x000000FF) >> 0;
                        
                        crc_t crc;
                        crc = crc_init();
                        crc = crc_update(crc, (unsigned char*)(&msg[1]), 6);
                        crc = crc_finalize(crc);
                        
                        msg[7] = crc;  
                                                
                    }
                    len=9;

                    maskDelimeters(msg,len);

#ifdef DEBUG
                    printf("sending: ");
                    for (int i=0; i<len; i++)
                        printf("%02X ", msg[i]);
                    printf("\n");
#endif
                    
                    usleep(5000);
                    res = write(_fd, msg, len);
                }
            }
        }
    }
}

// Len is the length of the header+body 
crc_t CiTorqeedo::getCRC(unsigned char* msg, unsigned int len) {
    crc_t crc;
    crc = crc_init();
    crc = crc_update(crc, msg, len);
    return crc_finalize(crc);
}

bool CiTorqeedo::getPacket(unsigned char* msg, int &len) {
    len = 0;

    int totalBytesRead = 0;
    fd_set set;
    struct timeval timeout;
    int numTries = 0;
    bool packetFound = false;
    
    // total time slots are 25ms (25000) 
    // this works out to 5 loops of the timeout below
    while(numTries<5 && !packetFound) {

        FD_ZERO(&set); // clear the set
        FD_SET(_fd, &set); // add file to the set
        
        // set a timeout of 0.005 seconds (5000 microseconds, API implies a response to start within 1560) 
        timeout.tv_sec = 0;
        timeout.tv_usec = 7000;

        int res = select(_fd+1, &set, NULL, NULL, &timeout);
        if (res == -1) {
            std::cerr << "get packet select error" << std::endl;
            if (errno==EINTR) continue;
            return packetFound;
        } else if (res == 0) {
            std::cerr << "read timeout" << std::endl;
            numTries++;
            continue;
        } 
     
        res = read(_fd, &recv_buff[totalBytesRead], 499-totalBytesRead);
        if (res<=0) {
            std::cerr << "Nothing to read" << std::endl;
            numTries++;
            continue;
        }

        totalBytesRead+=res;
        //std::cerr << "read this loop=" << res << std::endl;
        //std::cerr << "total read=" << totalBytesRead << std::endl;

        //printf("Recv_buff=\n");
        //for (int i=0; i<totalBytesRead; i++)
        //    printf("%02X ", recv_buff[i]);
        //printf("\n\n");
        
        // Need at least five bytes
        if (totalBytesRead<5)
            continue;

        // Seach for start packet byte         
        int startByte=-1;
        for (int i=0; i<totalBytesRead; i++) {
            if (recv_buff[i]==PACKET_START) {
                startByte=i;
                break;
            }
        }

        // no start byte found
        if (startByte<0) {
            std::cerr << "Failed to find start byte" << std::endl;
            continue;
        }

        // Search for end packet byte
        int endByte = -1;
        for (int i=startByte+1; i<totalBytesRead; i++) {
            if (recv_buff[i]==PACKET_END) {
                endByte=i;
                break;
            }
        }

        if (endByte<0) {
            std::cerr << "Failed to find end byte" << std::endl;
            continue;
        }
        packetFound=true;
        
        len=(endByte-startByte)+1;
        
        // Copy the packet out
        for (unsigned int i=0; i<len; i++) {
            msg[i]=recv_buff[startByte+i];
        }

    }

    if (packetFound) {
        // Packet was found, check if there are any delimeter masks and remove them
        unMaskDelimeters(msg,len);
    }

    return packetFound;


}

// return time elapsed since lastTime in seconds
double CiTorqeedo::timeElapsed(timeval lastTime, timeval now) {
    return (now.tv_sec - lastTime.tv_sec) + ((now.tv_usec - lastTime.tv_usec)/1000000.0);
}

void CiTorqeedo::emulateBatteryBox() {

    timeval now;
    gettimeofday(&now, NULL);

    // Try every second to retrieve the motor info, once retrieved, don't request again
    // and move on to sending all the required messsages
    if (!infoReceived) {
        if (timeElapsed(infoTime,now)>1.0) {
            infoReceived = getMotorInfo();
            if (!infoReceived)
                gettimeofday(&infoTime, NULL);
        }
        return;
    }

    // Send drive commands ~10 times/sec (slightly less, so the avg will be closer to 10hz)

    if (timeElapsed(driveTime,now)>0.095) {
        setMotorDrive();
        //temporaryTrickBatteryBox();
        gettimeofday(&driveTime, NULL);
    }

    // Get status about the motor every ~400ms
    if (timeElapsed(statusTime,now)>0.395) {
        getMotorStatus();
        gettimeofday(&statusTime, NULL);
    }

    // Get dynamic parameters about the motor every ~400ms
    if (timeElapsed(paramTime,now)>0.395) {
        getMotorParams();
        gettimeofday(&paramTime, NULL);
    }


}



bool CiTorqeedo::getMotorInfo() {
    //std::cerr << "Getting motor info" << std::endl;
    unsigned char msg[12];
    int len=5;
    
    // Start byte
    msg[0]=PACKET_START;
    // Header
    msg[1]=MOTOR;
    msg[2]=INFO;
    // No body
    // CRC
    msg[3]=getCRC((unsigned char*)(&msg[1]),2);
    // End byte
    msg[4]=PACKET_END;
    
    maskDelimeters(msg,len);
    
    /*
    std::cerr << "writing motor info request" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", msg[i]);
    printf("\n\n");
    */

    int res = write(_fd, msg, len);
    if (res <= 0)
        MOOSFail("write returned %d\n", res);


    unsigned char response[64];
    
    if (!getPacket(response,len)) {
        std::cerr << "Failed to get motor info response" << std::endl;
        return false;
    }

    /*
    std::cerr << "motor info:" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", response[i]);
    printf("\n\n");
    fflush(stdout);
    */

    // Check the packet size
    if (len!=17) {
        std::cerr << "Motor info packet length is incorrect" << std::endl;
        return false;
    }


    //unsigned char response2[17]={0xAC, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x22, 0x00, 0x10, 0x13, 0x10, 0x20, 0x00, 0x02, 0x9B, 0xAD};
    //memcpy(response,response2,17);

    
    // packet_start+header=3 bytes

    /*
    uint16_t motor_type_i = (uint16_t)msg[3];
    uint16_t serial = (uint16_t)msg[5];
    uint16_t pcb_version=(uint16_t)msg[7];
    uint16_t software_version=(uint16_t)msg[9];
    uint16_t param_version=(uint16_t)msg[11];
    uint16_t hours_in_use=(uint16_t)msg[13];
    */


    uint16_t motor_type_i = ((uint16_t)response[3] << 8) | (uint16_t)response[4];        
    uint16_t serial = ((uint16_t)response[5] << 8) | (uint16_t)response[6];    

    //    uint16_t pcb_version=((uint16_t)response[7] << 8) | (uint16_t)response[8];        
    std::stringstream pcb_ss;
    pcb_ss << ((response[7] & 0xF0) >> 4) << "." << (response[7] & 0x0F) << ".";
    pcb_ss << ((response[8] & 0xF0) >> 4) << "." << (response[8] & 0x0F);
    
    std::string pcb_version = pcb_ss.str();

    //uint16_t software_version=((uint16_t)response[9] << 8) | (uint16_t)response[10];        
    std::stringstream swv_ss;
    swv_ss << ((response[9] & 0xF0) >> 4) << "." << (response[9] & 0x0F) << ".";
    swv_ss << ((response[10] & 0xF0) >> 4) << "." << (response[10] & 0x0F);
    
    std::string software_version = swv_ss.str();

    //uint16_t param_version=((uint16_t)response[11] << 8) | (uint16_t)response[12];        
    std::stringstream pv_ss;
    pv_ss << ((response[11] & 0xF0) >> 4) << "." << (response[11] & 0x0F) << ".";
    pv_ss << ((response[12] & 0xF0) >> 4) << "." << (response[12] & 0x0F);
    
    std::string param_version = pv_ss.str();


    uint16_t hours_in_use=((uint16_t)response[13] << 8) | (uint16_t)response[14];        
    

    std::string motor_type="Unknown";

    if (motor_type_i==1)
        motor_type="Ultralight";
    else if (motor_type_i==2)
        motor_type="Cruise 2.0";
    else if (motor_type_i==3)
        motor_type="Cruise 4.0";
    else if (motor_type_i==6)
        motor_type="Travel 503";

    /*
    std::cerr << "Motor type=" << motor_type << std::endl;
    std::cerr << "Motor type i=" << motor_type_i << std::endl;
    std::cerr << "Serial=" << serial << std::endl;
    std::cerr << "PCB Version=" << pcb_version << std::endl;
    std::cerr << "Software Version=" << software_version << std::endl;
    std::cerr << "Param Version=" << param_version << std::endl;
    std::cerr << "Hours in use=" << hours_in_use << std::endl;
    */

    //m_Comms.Register(_name + "_CMD", 0);
    std::stringstream ss;
    ss << "Type=" << motor_type << ",Serial=" << serial << ",PCB_Version=" << pcb_version;
    ss << ",Software_Version=" << software_version << ",Param_Version=" << param_version;
    ss << ",Hours_In_Use=" << hours_in_use;
    m_Comms.Notify(_name+"_Info",ss.str());

    return true;
}


bool CiTorqeedo::setMotorDrive() {

    //std::cerr << "Setting motor drive" << std::endl;
    unsigned char msg[20];
    int len=9;
    
    // Start byte
    msg[0]=PACKET_START;
    // Header
    msg[1]=MOTOR;
    msg[2]=DRIVE;
    // Body (default to zero throttle message);
    msg[3]=0x08;  // mystery bit 3
    msg[4]=0x00;
    msg[5]=0x00;
    msg[6]=0x00;    
    // CRC
    msg[7]=0xB9;
    // End byte
    msg[8]=PACKET_END;

    // If clearing a motor error was requested, set the clear bit
    if (_clearError) {
        std::cerr << "Clearing motor error" << std::endl;
        // Clear bit 
        msg[3]=msg[3] | 0x04;
        _clearError=false;

        // Calculate crc
        msg[7]=getCRC((unsigned char*)(&msg[1]),6);

        // mask any delimeters present in the message
        maskDelimeters(msg,len);

    } else if (abs(_speed) >0) {
        //std::cerr << "Setting non-zero speed" << std::endl;
        msg[3]=0x09; // mystery bit 3, plus drive bit 0
        msg[4]=0x64; // 100% power, always 100% when commanding any non-zero speed
        
        msg[5] = (_speed & 0x0000FF00) >> 8;
        msg[6] = (_speed & 0x000000FF) >> 0;

        // Calculate crc
        msg[7]=getCRC((unsigned char*)(&msg[1]),6);

        maskDelimeters(msg,len);
    
    }

    /*
    std::cerr << "writing motor drive request" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", msg[i]);
    printf("\n\n");
    */
    int res = write(_fd, msg, len);
    if (res <= 0)
        MOOSFail("write returned %d\n", res);


    unsigned char response[16];
    
    if (!getPacket(response,len)) {
        std::cerr << "Failed to get motor drive response" << std::endl;
        return false;
    }

    /*
    std::cerr << "motor drive response:" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", response[i]);
    printf("\n\n");
    fflush(stdout);
    */
    // Check the packet size
    if (len!=5) {
        std::cerr << "Motor drive response packet length is incorrect" << std::endl;
        return false;
    }
    
    return true;

}

bool CiTorqeedo::getMotorStatus() {

    //std::cerr << "Getting motor status" << std::endl;
    unsigned char msg[12];
    int len=5;
    
    // Start byte
    msg[0]=PACKET_START;
    // Header
    msg[1]=MOTOR;
    msg[2]=MOTOR_STATUS;
    // No body
    // CRC
    msg[3]=getCRC((unsigned char*)(&msg[1]),2);
    // End byte
    msg[4]=PACKET_END;
    
    maskDelimeters(msg,len);
    
    /*
    std::cerr << "writing motor status request" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", msg[i]);
    printf("\n\n");
    */

    int res = write(_fd, msg, len);
    if (res <= 0)
        MOOSFail("write returned %d\n", res);


    unsigned char response[16];
    
    if (!getPacket(response,len)) {
        std::cerr << "Failed to get motor status response" << std::endl;
        return false;
    }

    /*
    unsigned char response2[8]={0xAC, 0x00, 0x00, 0x00, 0x00, 0x20, 0x23, 0xAD};
    memcpy(response,response2,8);
    len=8;
    */

    /*
    std::cerr << "motor status:" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", response[i]);
    printf("\n\n");
    fflush(stdout);
    */

    // Check the packet size
    if (len!=8) {
        std::cerr << "Motor status packet length is incorrect" << std::endl;
        return false;
    }


    uint8_t status=response[3];
    uint16_t errors = ((uint16_t)response[4] << 8) | (uint16_t)response[5];
   


    bool temp_limit_motor =  (status & (1 << 0));
    bool temp_limit_pcb =    (status & (1 << 1));
    bool emergency_stop =    (status & (1 << 2));
    bool running =           (status & (1 << 3));
    bool power_limit =       (status & (1 << 4));
    bool low_voltage_limit = (status & (1 << 5));
    bool tilt_status =       (status & (1 << 6));
    bool reserved =          (status & (1 << 7));


    bool overcurrent =          (errors & (1 << 0));
    bool blocked =              (errors & (1 << 1));
    bool overvoltage_static =   (errors & (1 << 2));
    bool undervoltage_static =  (errors & (1 << 3));
    bool overvoltage_current =  (errors & (1 << 4));
    bool undervoltage_current = (errors & (1 << 5));
    bool overtemp_motor =       (errors & (1 << 6));
    bool overtemp_pcb =         (errors & (1 << 7));
    bool timeout_rs485 =        (errors & (1 << 8));
    bool temp_sensor_error =    (errors & (1 << 9));
    bool tilt_error =           (errors & (1 << 10));


    /*
    std::cerr << "temp_limit_motor=" << temp_limit_motor << std::endl;
    std::cerr << "temp_limit_pcb=" << temp_limit_pcb << std::endl;
    std::cerr << "emergency_stop=" << emergency_stop << std::endl;
    std::cerr << "running=" << running << std::endl;
    std::cerr << "power_limit=" << power_limit << std::endl;
    std::cerr << "low_voltage_limit=" << low_voltage_limit << std::endl;
    std::cerr << "tilt_status=" << tilt_status << std::endl;
    std::cerr << "reserved=" << reserved << std::endl;
    std::cerr << "overcurrent=" << overcurrent << std::endl;
    std::cerr << "blocked=" << blocked << std::endl;
    std::cerr << "overvoltage_static=" << overvoltage_static << std::endl;
    std::cerr << "undervoltage_static=" << undervoltage_static << std::endl;
    std::cerr << "overvoltage_current=" << overvoltage_current << std::endl;
    std::cerr << "undervoltage_current=" << undervoltage_current << std::endl;
    std::cerr << "overtemp_motor=" << overtemp_motor << std::endl;
    std::cerr << "overtemp_pcb=" << overtemp_pcb << std::endl;
    std::cerr << "timeout_rs485=" << timeout_rs485 << std::endl;
    std::cerr << "temp_sensor_error=" << temp_sensor_error << std::endl;
    std::cerr << "tilt_error=" << tilt_error << std::endl;
    */

    std::stringstream ss_status;

    if (temp_limit_motor)
        ss_status << "temp_limit_motor, ";
            
    if (temp_limit_pcb)
        ss_status << "temp_limit_pcb, ";
    
    if (emergency_stop)
        ss_status << "emergency_stop, ";

    if (running)
        ss_status << "running, ";

    if (power_limit)
        ss_status << "power_limit, ";

    if (low_voltage_limit)
        ss_status << "low_voltage_limit, ";

    if (tilt_status)
        ss_status << "tilt_status, ";

    if (reserved)
        ss_status << "reserved, ";

    std::stringstream ss_errors;
    
    if (overcurrent)
        ss_errors << "overcurrent, ";

    if (blocked)
        ss_errors << "blocked, ";

    if (overvoltage_static)
        ss_errors << "overvoltage_static, ";
    
    if (undervoltage_static)
        ss_errors << "undervoltage_static, ";
    
    if (overvoltage_current)
        ss_errors << "overvoltage_current, ";

    if (undervoltage_current)
        ss_errors << "undervoltage_current, ";

    if (overtemp_motor)
        ss_errors << "overtemp_motor, ";

    if (overtemp_pcb)
        ss_errors << "overtemp_pcb, ";
    
    if (timeout_rs485)
        ss_errors << "timeout_rs485, ";

    if (temp_sensor_error)
        ss_errors << "temp_sensor_error, ";

    if (tilt_error)
        ss_errors << "tilt_error";

    std::string statusStr = ss_status.str();
    std::string errorsStr = ss_errors.str();

    if (statusStr.size()>0)
        m_Comms.Notify(_name+"_Status",statusStr);

    if (errorsStr.size()>0)
        m_Comms.Notify(_name+"_Errors",errorsStr);




    return true;

}

bool CiTorqeedo::getMotorParams() {


    //std::cerr << "Getting motor params" << std::endl;
    unsigned char msg[12];
    int len=5;
    
    // Start byte
    msg[0]=PACKET_START;
    // Header
    msg[1]=MOTOR;
    msg[2]=PARAM;
    // No body
    // CRC
    msg[3]=getCRC((unsigned char*)(&msg[1]),2);
    // End byte
    msg[4]=PACKET_END;
    
    maskDelimeters(msg,len);
    
    /*
    std::cerr << "writing motor param request" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", msg[i]);
    printf("\n\n");
    */

    int res = write(_fd, msg, len);
    if (res <= 0)
        MOOSFail("write returned %d\n", res);


    unsigned char response[16];
    
    /*
    //AC 00 00 05 02 00 07 01 4D 00 03 00 80 00 AE 2E AD 
    unsigned char data[17]={0xAC, 0x00, 0x00, 0x05, 0x02, 0x00, 0x07, 0x01, 0x4d, 0x00, 0x03, 0x00, 0x80, 0x00, 0xAE, 0x2E, 0xAD};
    int len2=17;
    unMaskDelimeters(data,len2);
    std::cerr << "len2 after=" << len2 << std::endl;
    std::cerr << "msg after=" << std::endl;
    for (int i=0; i<len2; i++)
        printf("%02X ", data[i]);
    printf("\n\n");
    fflush(stdout);
    return false;
    */

    if (!getPacket(response,len)) {
        std::cerr << "Failed to get motor param response" << std::endl;
        return false;
    }

    /*
    std::cerr << "motor params:" << std::endl;
    for (int i=0; i<len; i++)
        printf("%02X ", response[i]);
    printf("\n\n");
    fflush(stdout);
    */

    // Check the packet size
    if (len!=17) {
        std::cerr << "Motor params packet length is incorrect" << std::endl;
        return false;
    }


    int16_t speed = ((int16_t)response[3] << 8) | (int16_t)response[4];        
    uint16_t power = ((uint16_t)response[5] << 8) | (uint16_t)response[6];    
    uint16_t voltage = ((uint16_t)response[7] << 8) | (uint16_t)response[8];    
    uint16_t current = ((uint16_t)response[9] << 8) | (uint16_t)response[10];    
    int16_t pcb_temp = ((int16_t)response[11] << 8) | (int16_t)response[12];    
    int16_t stator_temp = ((int16_t)response[13] << 8) | (int16_t)response[14];    
    /*
    std::cerr << "speed=" << speed/100.0 << "%" << std::endl;
    std::cerr << "power=" << power << "W" << std::endl;
    std::cerr << "voltage=" << voltage/100.0 << "V" << std::endl;
    std::cerr << "current=" << current/10.0 << "A" << std::endl;
    std::cerr << "pcb_temp=" << pcb_temp/10.0 << " degrees C" << std::endl;
    std::cerr << "stator_temp=" << stator_temp/10.0 << " degrees C" << std::endl;
    */

    std::stringstream ss;

    ss << "Speed=" << speed/100.0 << ",Power=" << power << ",Voltage=" << voltage/100.0;
    ss << ",Current=" << current/10.0 << ",PcbTemp=" << pcb_temp/10.0 << ",StatorTemp=";
    ss << stator_temp/10.0;
        
    m_Comms.Notify(_name+"_Params",ss.str());

        

    return true;
}


void CiTorqeedo::temporaryTrickBatteryBox() {
    

    int splice_loc = 0;
    
    unsigned char msg[20];
    memset(msg, 0, 20);
    int len = 0;
    
    
    
    //AC 00 00 05 00 00 00 81 AD
    msg[0] = 0xAC;
    msg[1] = 0x00;
    msg[2] = 0x00;
    msg[3] = 0x05;
    msg[4] = 0x00;
    msg[5] = 0x00;
    msg[6] = 0x00;
    msg[7] = 0x81;
    msg[8] = 0xAD;

    if (abs(_speed) > 0) {
        msg[5] = (_speed & 0x0000FF00) >> 8;
        msg[6] = (_speed & 0x000000FF) >> 0;
        
        crc_t crc;
        crc = crc_init();
        crc = crc_update(crc, (unsigned char*)(&msg[1]), 6);
        crc = crc_finalize(crc);
        
        msg[7] = crc;  
        
    }
    len=9;
    
    maskDelimeters(msg,len);
    
    int res = write(_fd, msg, len);
    
}

void CiTorqeedo::emulateBatteryBoxOld() {
    //timeval now;
    //gettimeofday(&now, NULL);
    //double delta_usec = 1000000*(now.tv_sec - _lastCmd.tv_sec) + (now.tv_usec - _lastCmd.tv_usec);
    // if (_controlRate > 0 && delta_usec > 1000000 / _controlRate)
    //     _speed = 0;

    unsigned char msg[20];
    int len = 0;
    
    // msg[0] = 0xAC;
    // msg[1] = 0x30;
    // msg[2] = 0x82;
    // msg[3] = 0x08;
    // msg[4] = 0x00;
    // msg[5] = 0x00;
    // msg[6] = 0x00;
    // msg[7] = 0xB9;
    // msg[8] = 0xAD;
    // len = 9;
    // if (abs(_speed) > 0)
    // {
    //     msg[3] = 0x09;
    //     msg[4] = 0x64;
    //     msg[5] = (_speed & 0x0000FF00) >> 8;
    //     msg[6] = (_speed & 0x000000FF) >> 0;
        
    //     crc_t crc;
    //     crc = crc_init();
    //     crc = crc_update(crc, (unsigned char*)(&msg[1]), 6);
    //     crc = crc_finalize(crc);
        
    //     msg[7] = crc;
    // }


    switch (_iteration % 5)
    {
        case 0:
            msg[0] = 0xAC;
            msg[1] = 0x30;
            msg[2] = 0x82;
            msg[3] = 0x08;
            msg[4] = 0x00;
            msg[5] = 0x00;
            msg[6] = 0x00;
            msg[7] = 0xB9;
            msg[8] = 0xAD;
            len = 9;
            if (abs(_speed) > 0)
            {
                msg[3] = 0x09;
                msg[4] = 0x64;
                msg[5] = (_speed & 0x0000FF00) >> 8;
                msg[6] = (_speed & 0x000000FF) >> 0;
        
                crc_t crc;
                crc = crc_init();
                crc = crc_update(crc, (unsigned char*)(&msg[1]), 6);
                crc = crc_finalize(crc);
        
                msg[7] = crc;
            }
            break;

        case 1:
            msg[0] = 0xAC;
            msg[1] = 0x30;
            msg[2] = 0x03;
            msg[3] = 0xCF;
            msg[4] = 0xAD;
            len = 5;
            break;

        case 2:
            msg[0] = 0xAC;
            msg[1] = 0x30;
            msg[2] = 0x01;
            msg[3] = 0x73;
            msg[4] = 0xAD;
            len = 5;
            break;

        case 3:
            msg[0] = 0xAC;
            msg[1] = 0x30;
            msg[2] = 0x00;
            msg[3] = 0x2D;
            msg[4] = 0xAD;
            len = 5;
            break;

        case 4:
        defualt:
            msg[0] = 0xAC;
            msg[1] = 0x20;
            msg[2] = 0x42;
            msg[3] = 0x01;
            msg[4] = 0x06;
            msg[5] = 0x10;
            msg[6] = 0x13;
            msg[7] = 0x00;
            msg[8] = 0x12;
            msg[9] = 0x56;
            msg[10] = 0x01;
            msg[11] = 0x10;
            msg[12] = 0x33;
            msg[13] = 0x59;
            msg[14] = 0xAD;
            len=15;
            break;
    }

    maskDelimeters(msg,len);

#ifdef DEBUG
    printf("sending: ");
    for (int i=0; i<len; i++)
        printf("%02X ", msg[i]);
    printf("\n");
#endif

    int res = write(_fd, msg, len);
    if (res <= 0)
        MOOSFail("write returned %d\n", res);

}

/** Called by the base class periodically. This is where you place code
which does the work of the application **/
bool CiTorqeedo::Iterate()
{
    _iteration++;

    if (_sendHandController)
        emulateHandController();
    else
        emulateBatteryBox();

    return true;
}

/** called by the base class before the first ::Iterate is called. Place
startup code here - especially code which reads configuration data from the 
mission file **/
bool CiTorqeedo::OnStartUp()
{
#ifdef DEBUG
    printf("on start up\n");
#endif

    m_MissionReader.GetConfigurationParam("Name", _name);
    MOOSTrace("Torqeedo Name: %s\n", _name.c_str());

    std::string devicename;
    m_MissionReader.GetConfigurationParam("Device", devicename);
    MOOSTrace("Device: %s\n", devicename.c_str());

    m_MissionReader.GetConfigurationParam("ControlRate", _controlRate);
    MOOSTrace("Control Rate: %f\n", _controlRate);

    m_MissionReader.GetConfigurationParam("MaxOutput", _maxOutput);
    MOOSTrace("Max Output: %f\n", _maxOutput);
    
    m_MissionReader.GetConfigurationParam("EmulateHandController", _sendHandController);
    MOOSTrace("Emulate Hand Controller: %d\n", _sendHandController);

    // First, open the device and toggle CLOCAL to disable the blocking on the 
    // next open call
    MOOSTrace("Opening device\n");
    _fd = open(devicename.c_str(), O_RDWR | O_NONBLOCK);
    if (_fd < 0) 
    {
        MOOSFail("Cannot open device\n");
        exit(-1); 
    }
    
    struct termios term;
    // Get the attributes of the serial device
    if ((tcgetattr(_fd, &term)) < 0) {
        MOOSFail("Couldn't get attributes for %s\n",devicename.c_str());
        close(_fd);
        exit(-1);
    }

    // Set the CLOCAL flag, this disables the hardware carrier detect signal
    term.c_cflag |= CLOCAL;
    if ((tcsetattr(_fd, TCSANOW, &term)) == -1) {
        // If we fail, might as well try and open the device anyway
        MOOSFail("Couldn't set attributes for `%s`\n", devicename.c_str());
    }
    close(_fd);



    

    MOOSTrace("Re-opening device\n");
    int flags;
    // If we're emulating the hand controller, reads should block as we only
    // send commands in response to requests from the battery box
    //if (_sendHandController)
    //	flags = O_RDWR | O_NOCTTY;
    //else
        flags = O_RDWR | O_NOCTTY | O_NDELAY; //non-blocking

    _fd = open(devicename.c_str(), flags);  
    if (_fd < 0) 
    {
        MOOSFail("Cannot re-open device\n");
        exit(-1); 
    }
    
    /* set input mode (non-canonical, no echo,...) */
    struct termios tio;
    bzero(&tio, sizeof(tio));
    tio.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
    tio.c_iflag = IGNPAR;
    tio.c_oflag = 0;
    tio.c_lflag = 0;
    tio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
    tio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
    tcflush(_fd, TCIFLUSH);
    tcsetattr(_fd,TCSANOW,&tio);
    
    MOOSTrace("Device ready\n");
    
    

    DoRegistrations();
    
    return true;
}

void CiTorqeedo::DoRegistrations()
{
#ifdef DEBUG
    printf("registering for '%s'\n", _name.c_str());
#endif
    
    m_Comms.Register(_name + "_CMD", 0);
    m_Comms.Register(_name + "_RESET_CMD", 0);

    
    return;
}

