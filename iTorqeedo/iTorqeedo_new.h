//
// file: iTorqeedo.h
// author: Ben Axelrod
// date: June 18, 2013
// 
// interface for the CiTorqeedo class.
//

#ifndef _CiTorqeedo_
#define _CiTorqeedo_

#include "MOOS/libMOOS/App/MOOSApp.h"

#include <iostream>
#include <fstream>
#include <string.h>
#include "crc.h"

class CiTorqeedo : public CMOOSApp  
{
public:
    //standard construction and destruction
    CiTorqeedo();
    virtual ~CiTorqeedo();

protected:
    //where we handle new mail
    bool OnNewMail(MOOSMSG_LIST &NewMail);
    //where we do the work
    bool Iterate();
    //called when we connect to the server
    bool OnConnectToServer();
    //called when we disconnect from the server
    bool OnDisconnectFromServer();
    //called when we are starting up..
    bool OnStartUp();
    //register subscriptions
    void DoRegistrations();

    // Send messages acting as the battery box
    void emulateBatteryBox();
    void emulateBatteryBoxOld();
    // Send messages acting as the hand controller
    void emulateHandController();
    
    // Searches for special bytes in the message adding a flag to the message
    // and changing to special byte 
    void maskDelimeters(unsigned char* msg, int &len);


    // Searches for masked special bytes in the message, and restores them to
    // their actual value
    void unMaskDelimeters(unsigned char* msg, int &len);


    // Get info about the motor (as needed)
    bool getMotorInfo();

    // Send drive command
    bool setMotorDrive();

    // Get status about the motor
    bool getMotorStatus();

    // Get dynamic parameters about the motor
    bool getMotorParams();

    void temporaryTrickBatteryBox();
    
    // crc calculation
    crc_t getCRC(unsigned char* msg, unsigned int len);

    // read a packet 
    bool getPacket(unsigned char* msg, int &len);

    // Return time elapsed
    double timeElapsed(timeval lastTime, timeval now);

    std::string _name;
    int _fd;
    int _speed;
    double _controlRate;
    timeval _lastCmd;
    int _iteration;
    std::ifstream _stream;
    double _maxOutput;
    bool _sendHandController;
    bool _clearError;


    // Buffers for reading and parsing
    unsigned char recv_buff[500];
    unsigned char splice_buff[500];

    // Track times for messages to ensure proper frequencies are maintained
    bool infoReceived;
    timeval infoTime;
    timeval driveTime;
    timeval statusTime;
    timeval paramTime;

    

};

#endif 

