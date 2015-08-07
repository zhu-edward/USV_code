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

#include "iTorqeedo.h"
#include "crc.h"

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

void CiTorqeedo::adjustMsg(unsigned char* msg, int &len) {
    // The first and last byte of the message is always AC, AD respectively
    // so ignore them

    //unsigned char* beforemsg = new unsigned char[len];
    //int beforelen = len;
    //memcpy(beforemsg,msg,len);
    
    //bool adjusted=false;

    for (unsigned int i=1; i<len-1; i++) {
        if (msg[i]>0xAB && msg[i]<0xAF) {
            //std::cerr << "Found bad message at " << i;;
            //printf(" of value %02X",msg[i]);
            //fflush(stdout);
            //std::cerr << "Before: ";
            //for (int k=0; k<beforelen; k++)
            //    printf("%02X ", beforemsg[k]);
            //printf("\n");
            //fflush(stdout);

            //std::cerr << "here" << std::endl;
            //adjusted=true;

            //printf("changing %02X to ",msg[i]);

            unsigned char prev=msg[i];
           
            // Toggle the high bit for the special byte
            prev = (prev ^ 0x80);

            //printf("%02X\n",prev);
            //fflush(stdout);

            // Add the special flag
            msg[i]=0xAE;
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

    /*
    if (adjusted) {
        printf("After: ");
        for (int i=0; i<len; i++)
            printf("%02X ", msg[i]);
        printf("\n\n");
        fflush(stdout);
    }
    */

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

                    adjustMsg(msg,len);

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

void CiTorqeedo::emulateBatteryBox() {
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

    adjustMsg(msg,len);

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
    if (_sendHandController)
        flags = O_RDWR | O_NOCTTY;
    else
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
    
    return;
}

