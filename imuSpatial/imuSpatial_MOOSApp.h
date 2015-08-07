
#ifndef USM_MOOSAPP_HEADER
#define USM_MOOSAPP_HEADER

#include <string>
#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

//#include "USM_Model.h"
//#include "MOOSGeodesy.h"
//#include "MOOS/Compatibility/Essentials/MOOSUtilityLib/MOOSGeodesy.h"
//#include "CommonSense/drivers/microstrain3DM/Microstrain3DM.h"
//#include "CommonSense/system/System.h"
//#include "CommonSense/system/SampleApp.h"
//#include "Microstrain3DMApp.h"
//#include "CommonSense/core/util/PosePrinter.h"
//#include "MoosdbPrinter.h"
//#include "MoosdbGPSPrinter.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "imuSpatialSDK/rs232/rs232.h"
#include "imuSpatialSDK/an_packet_protocol.h"
#include "imuSpatialSDK/spatial_packets.h"

#define RADIANS_TO_DEGREES (180.0/M_PI)


class imuSpatial_MOOSApp : public CMOOSApp
{
public:

  imuSpatial_MOOSApp();
  virtual ~imuSpatial_MOOSApp();


  // virtual overide of base class CMOOSApp member. 
  // Here we do all the processing and IO 
  bool Iterate();

  // virtual overide of base class CMOOSApp member. 
  // Here we register for data we wish be informed about
  bool OnConnectToServer();
  bool OnStartUp();
  bool OnNewMail(MOOSMSG_LIST &NewMail);

  //boost::shared_ptr<CommonSense::Core::MoosdbPrinter> _moosdbPrinter;
  //boost::shared_ptr<CommonSense::Core::MoosdbGPSPrinter> _moosdbGPSPrinter;

  //boost::shared_ptr<CommonSense::Drivers::Microstrain3DM> sensor;
private:


    //boost::shared_ptr<CommonSense::Core::PosePrinter> _posePrinter;
    //boost::shared_ptr<CommonSense::Core::GPSPrinter> _gpsPrinter;

    an_decoder_t an_decoder;
    an_packet_t *an_packet;

    system_state_packet_t system_state_packet;
    raw_sensors_packet_t raw_sensors_packet;
    unix_time_packet_t unix_time_packet;
    ecef_position_packet_t ecef_position_packet;
    body_velocity_packet_t body_velocity_packet;
    acceleration_packet_t acceleration_packet;
    quaternion_orientation_packet_t quaternion_orientation_packet;
    dcm_orientation_packet_t dcm_orientation_packet;
    angular_acceleration_packet_t angular_acceleration_packet;
    running_time_packet_t running_time_packet;
    local_magnetics_packet_t local_magnetics_packet;
    odometer_state_packet_t odometer_state_packet;
    geoid_height_packet_t geoid_height_packet;
    heave_packet_t heave_packet;

    bool system_state_packet_received;
    bool raw_sensors_packet_received;
    bool unix_time_packet_received;
    bool ecef_position_packet_received;
    bool body_velocity_packet_received;
    bool acceleration_packet_received;
    bool quaternion_orientation_packet_received;
    bool dcm_orientation_packet_received;
    bool angular_acceleration_packet_received;
    bool running_time_packet_received;
    bool local_magnetics_packet_received;
    bool odometer_state_packet_received;
    bool geoid_height_packet_received;
    bool heave_packet_received;

    int bytes_received;
    char buffer[1024];

    CMOOSGeodesy geodesy;
    float lat_origin, long_origin;
    double time_mission;
    double time_mission_prev;
    double time_mission_gps_lock;

    double dt;
    float acceleration_mag_XY;
    float velocity_mag_XY;

    float yaw;
    float vel_x;
    float vel_y;

    float vel_X;
    float vel_Y;

    float lat_init;
    float lon_init;
    bool waiting_for_gps_lock;
    bool first_state_packet;
    bool gps_imu_stable;


/*
    float latitude;
    float longitude;

    float position_x;
    float position_y;
    float position_z;
    float position_roll;
    float position_pitch;
    float position_yaw;

    float velocity_x;
    float velocity_y;
    float velocity_z;
    float velocity_roll;
    float velocity_pitch;
    float velocity_yaw;

    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float acceleration_roll;
    float acceleration_pitch;
    float acceleration_yaw;

    float deltaVelocityVector_x;
    float deltaVelocityVector_y;
    float deltaVelocityVector_z;

    float quaternion_q0;
    float quaternion_q1;
    float quaternion_q2;
    float quaternion_q3;

    double ECEF_x;
    double ECEF_y;
    double ECEF_z;

    float ECEFVel_x;
    float ECEFVel_y;
    float ECEFVel_z;

    float NEDVel_north;
    float NEDVel_east;
    float NEDVel_down;
    float NEDVel_speed;
    float NEDVel_groundSpeed;
    float NEDVel_heading;

    double GPSTime_timeOfWeek;
    unsigned short GPSTime_weekNumber;
*/
 protected:
  //void postNodeRecordUpdate(std::string, const NodeRecord&, double);
  void registerVariables();
  int an_packet_transmit(an_packet_t *an_packet);
  void request_packet(uint8_t requested_packet_id);
  void request_unix_time_packets();
  void set_sensor_ranges();

  //void handleThrustMapping(std::string);

protected:
//  std::string  m_sim_prefix;
//  USM_Model    m_model;
//  unsigned int m_reset_count;

//  CMOOSGeodesy m_geodesy;
//  bool         m_geo_ok;
};

#endif
