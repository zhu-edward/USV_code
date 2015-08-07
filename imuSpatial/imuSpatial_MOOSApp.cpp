//#include <iostream>
//#include "CommonSense/system/System.h"
//#include "CommonSense/system/SampleApp.h"
//#include "CommonSense/drivers/microstrain3DM/Microstrain3DM.h"

//#include "CommonSense/core/util/GPSPrinter.h"
//#include "CommonSense/core/util/PosePrinter.h"
//#include "MoosdbPrinter.h"

//#include <boost/make_shared.hpp>
#include "imuSpatial_MOOSApp.h"
//#include "MBUtils.h"
//#include "AngleUtils.h"

//#define USE_UTM
#define GPS_LOCK_WAIT_SEC 15

using namespace std;

//------------------------------------------------------------------------
// Constructor
imuSpatial_MOOSApp::imuSpatial_MOOSApp() 
{
	cout << "\nDEBUG Constructor()\n";
//  m_sim_prefix     = "USM";
//  m_reset_count    = 0;
}

imuSpatial_MOOSApp::~imuSpatial_MOOSApp() 
{
	cout << "\nDEBUG Destructor()\n";
//  m_sim_prefix     = "USM";
//  m_reset_count    = 0;
}
//------------------------------------------------------------------------
// Procedure: OnNewMail
//      Note: 

bool imuSpatial_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{

    MOOSMSG_LIST::iterator p1;

        for (p1 = NewMail.begin(); p1!=NewMail.end(); p1++) {
            CMOOSMsg & rMsg = *p1;


            if (MOOSStrCmp(rMsg.GetKey(), "TIME")) {
                CMOOSMsg &Msg = rMsg;
                if (!Msg.IsDouble())
                    return MOOSFail("\"TIME\" needs to be a double");
                time_mission_prev = time_mission;
                time_mission = Msg.GetDouble();
                dt = time_mission - time_mission_prev;
            }
        }

        return true;



}

//------------------------------------------------------------------------
// Procedure: OnStartUp
//      Note: 

bool imuSpatial_MOOSApp::OnStartUp()
{
    //initializing the origin
    lat_origin = 32.702309;

    //here we extract a double from the configuration file
    if(!m_MissionReader.GetConfigurationParam("latOrigin", lat_origin)){
        MOOSTrace("Warning parameter \"latOrigin\" not specified. Using default of \"%f\"\n",lat_origin);
    }

    long_origin = -117.238329;
    velocity_mag_XY = 0;
    vel_x = 0;
    vel_y = 0;

    gps_imu_stable = false;
    waiting_for_gps_lock = true;
    first_state_packet = true;

        //here we extract a double from the configuration file
    if(!m_MissionReader.GetConfigurationParam("longOrigin", long_origin)){
        MOOSTrace("Warning parameter \"longOrigin\" not specified. Using default of \"%f\"\n",long_origin);
    }

	cout << "\nDEBUG OnStartUp()\n";

    //set frequency higher than 14Hz for best results.
    SetAppFreq(20); //14 through 20 Hz produces no packet loss.

    /* open the com port */
    //TODO might change to not hardcode it. maybe pass from command line
    string port = "/dev/ttyUSB0";
    int baudRate = 500000; //115200
    if (OpenComport(const_cast<char*>(port.c_str()), baudRate))
    {
        exit(EXIT_FAILURE);
    }

    an_decoder_initialise(&an_decoder);
/*
    if(!geodesy.Initialise(lat_origin, long_origin)){
        MOOSTrace("pVehicle: Geodesy init failed.\n");
    }
*/
    return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

bool imuSpatial_MOOSApp::OnConnectToServer()
{
	cout << "\nDEBUG OnConnectToServer()\n";
    Register("TIME",0);

  return(true);
}

//------------------------------------------------------------------------
// Procedure: OnConnectToServer
//      Note: 

void imuSpatial_MOOSApp::registerVariables()
{
	cout << "\nDEBUG registerVariables()\n";

    //no variables to register. only publishing data, not listening
  //m_Comms.Register("DESIRED_RUDDER", 0);

}

//------------------------------------------------------------------------
// Procedure: Iterate
//      Note: This is where it all happens.

bool imuSpatial_MOOSApp::Iterate()
{
    cout << "DEBUG Iterate()\n";

/*
    //request all packets from sensor. system state and raw sensor packets are requested by default
    request_packet(packet_id_unix_time);
    request_packet(packet_id_ecef_position);
    request_packet(packet_id_body_velocity);
    request_packet(packet_id_acceleration);
    request_packet(packet_id_quaternion_orientation);
    request_packet(packet_id_dcm_orientation);
    request_packet(packet_id_angular_acceleration);
    request_packet(packet_id_running_time);
    request_packet(packet_id_local_magnetics);
    request_packet(packet_id_odometer_state);
    request_packet(packet_id_geoid_height);
    request_packet(packet_id_heave);
*/
    //request_packet(packet_id_raw_sensors);
    request_packet(packet_id_system_state);
    request_packet(packet_id_body_velocity);

    //received packet flags
    system_state_packet_received = false;
    raw_sensors_packet_received = false;
    unix_time_packet_received = false;
    ecef_position_packet_received = false;
    body_velocity_packet_received = false;
    acceleration_packet_received = false;
    quaternion_orientation_packet_received = false;
    dcm_orientation_packet_received = false;
    angular_acceleration_packet_received = false;
    running_time_packet_received = false;
    local_magnetics_packet_received = false;
    odometer_state_packet_received = false;
    geoid_height_packet_received = false;
    heave_packet_received = false;

    int unique_packet_count = 0;

    if ((bytes_received = PollComport(an_decoder_pointer(&an_decoder), an_decoder_size(&an_decoder))) > 0)
    {
        cout << "polling success. bytes_received: " << bytes_received << endl;

        /* increment the decode buffer length by the number of bytes received */
        an_decoder_increment(&an_decoder, bytes_received);


        //int count = 0;
        /* decode all the packets in the buffer */
        while ((an_packet = an_packet_decode(&an_decoder)) != NULL)
        {

            if (an_packet->id == packet_id_system_state) /* system state packet */
            {
                if (!system_state_packet_received)
                {
                    unique_packet_count++;
                    system_state_packet_received = true;
                }

                /* copy all the binary data into the typedef struct for the packet */
                /* this allows easy access to all the different values             */
                if(decode_system_state_packet(&system_state_packet, an_packet) == 0)
                {/*
                    //printf("System State Packet:\n");
                    //printf("\tLatitude = %f, Longitude = %f, Height = %f\n", system_state_packet.latitude * RADIANS_TO_DEGREES, system_state_packet.longitude * RADIANS_TO_DEGREES, system_state_packet.height);
                    m_Comms.Notify("Nav1_Lat", system_state_packet.latitude * RADIANS_TO_DEGREES);
                    m_Comms.Notify("Nav2_Long", system_state_packet.longitude * RADIANS_TO_DEGREES);
                    m_Comms.Notify("Nav3_Height", system_state_packet.height * RADIANS_TO_DEGREES);
                    //printf("\tRoll = %f, Pitch = %f, Heading = %f\n", system_state_packet.orientation[0] * RADIANS_TO_DEGREES, system_state_packet.orientation[1] * RADIANS_TO_DEGREES, system_state_packet.orientation[2] * RADIANS_TO_DEGREES);
                    m_Comms.Notify("Orientation1_Roll", system_state_packet.orientation[0] * RADIANS_TO_DEGREES);
                    m_Comms.Notify("Orientation2_Pitch", system_state_packet.orientation[1] * RADIANS_TO_DEGREES);
                    m_Comms.Notify("Orientation3_Yaw", system_state_packet.orientation[2] * RADIANS_TO_DEGREES);

                    m_Comms.Notify("VelocityAngular1_x", system_state_packet.angular_velocity[0] * RADIANS_TO_DEGREES);
                    m_Comms.Notify("VelocityAngular2_y", system_state_packet.angular_velocity[1] * RADIANS_TO_DEGREES);
                    m_Comms.Notify("VelocityAngular3_z", system_state_packet.angular_velocity[2] * RADIANS_TO_DEGREES);

                    m_Comms.Notify("VelocityNED1_N", system_state_packet.velocity[0]);
                    m_Comms.Notify("VelocityNED2_E", system_state_packet.velocity[1]);
                    m_Comms.Notify("VelocityNED3_D", system_state_packet.velocity[2]);

                    m_Comms.Notify("Acceleration1_x", system_state_packet.body_acceleration[0]);
                    m_Comms.Notify("Acceleration2_y", system_state_packet.body_acceleration[1]);
                    m_Comms.Notify("Acceleration3_z", system_state_packet.body_acceleration[2]);

                    m_Comms.Notify("G_Force", system_state_packet.g_force);
                */

                    /*params for Ed's controller*/


                    double lat = system_state_packet.latitude * RADIANS_TO_DEGREES;
                    double lon = system_state_packet.longitude * RADIANS_TO_DEGREES;
                    double x_gps;
                    double y_gps;

                    //check for GPS and IMU stability
                    //wait 1 min after getting GPS stream
                    if (first_state_packet){
                        lat_init = lat;
                        lon_init = lon;
                        first_state_packet = false;
                        m_Comms.Notify("IMU_GPS_Stable", 0.0);
                    }
                    else if (waiting_for_gps_lock){
                        if (lat != lat_init){
                            waiting_for_gps_lock = false;
                            time_mission_gps_lock = time_mission;
                        }
                        m_Comms.Notify("IMU_GPS_Stable", 0.0);
                    }
                    else if (!waiting_for_gps_lock){
                        float wait_time_sec = time_mission - time_mission_gps_lock;
                        if (wait_time_sec > GPS_LOCK_WAIT_SEC){
                            if (!gps_imu_stable){
                                m_Comms.Notify("IMU_GPS_Stable", 1.0);
                                gps_imu_stable = true;
                                m_Comms.Notify("LAT_ORIGIN", lat);
                                m_Comms.Notify("LON_ORIGIN", lon);
                                lat_origin = lat;
                                long_origin = lon;

                                if(!geodesy.Initialise(lat_origin, long_origin)){
                                        MOOSTrace("pVehicle: Geodesy init failed.\n");
                                }
                            }
                            else if (gps_imu_stable){
                                m_Comms.Notify("IMU_GPS_Stable", 1.0);
                            }

                        }
                        else {
                            m_Comms.Notify("IMU_GPS_Stable", 0.0);
                        }
                    }

                    //only publish the location when the gps and imu are stable
                    if (gps_imu_stable){

                        geodesy.LatLong2LocalUTM(lat, lon, y_gps, x_gps);
                        m_Comms.Notify("NAV_X", x_gps);
                        m_Comms.Notify("NAV_Y", y_gps);
                    }
                    else {
                        m_Comms.Notify("NAV_X", 0.0);
                        m_Comms.Notify("NAV_Y", 0.0);
                    }



                    yaw = M_PI/2 - system_state_packet.orientation[2];
                    if (yaw < 0){
                        yaw = yaw + 2*M_PI;
                    }
                    m_Comms.Notify("NAV_YAW", yaw);
		    m_Comms.Notify("NAV_HEADING", system_state_packet.orientation[2]*RADIANS_TO_DEGREES);


                    vel_X = system_state_packet.velocity[1];
                    vel_Y = system_state_packet.velocity[0];

                    float velocity_absolute = sqrt(pow(vel_X,2) + pow(vel_Y,2));

                    float theta = atan2(vel_Y, vel_X);
                    if (theta < 0){
                        theta = theta + 2*M_PI;
                    }

                    float diff = theta - yaw;
                    if (diff > M_PI){
                        diff = diff - 2*M_PI;
                    }
                    else if (diff < -M_PI){
                        diff = diff + 2*M_PI;
                    }

                    float u = cos(diff)*velocity_absolute;
                    float v = sin(diff)*velocity_absolute;

                    m_Comms.Notify("NAV_SPEED", u);
                    m_Comms.Notify("V", v);

                    m_Comms.Notify("R", -system_state_packet.angular_velocity[2]);

                }
            }

            else if (an_packet->id == packet_id_raw_sensors) /* raw sensors packet */
            {
                if (!raw_sensors_packet_received)
                {
                    unique_packet_count++;
                    raw_sensors_packet_received = true;
                }

                /* copy all the binary data into the typedef struct for the packet */
                /* this allows easy access to all the different values             */
                if(decode_raw_sensors_packet(&raw_sensors_packet, an_packet) == 0)
                {
                    //printf("Raw Sensors Packet:\n");
                    /*
                    m_Comms.Notify("Temperature_imu", raw_sensors_packet.imu_temperature);
                    m_Comms.Notify("Pressure", raw_sensors_packet.pressure);
                    m_Comms.Notify("Pressure_temperature", raw_sensors_packet.pressure_temperature);
                    */
                    acceleration_mag_XY = sqrt(pow(raw_sensors_packet.accelerometers[0],2) + pow(raw_sensors_packet.accelerometers[1],2));

                    float dv_x = raw_sensors_packet.accelerometers[0]*dt;
                    float dv_y = raw_sensors_packet.accelerometers[1]*dt;

                    vel_x = vel_x + dv_x;
                    vel_y = vel_y + dv_y;
/*
                    float theta = atan2(vel_y, vel_x);
                    if (theta < 0){
                        theta = theta + 2*M_PI;
                    }

                    float diff = theta - yaw;
                    if (diff > M_PI){
                        diff = diff - 2*M_PI;
                    }
                    else if (diff < -M_PI){
                        diff = diff + 2*M_PI;
                    }
*/



                    m_Comms.Notify("NAV_SPEED_IMU", vel_x);
                    m_Comms.Notify("V_IMU", -vel_y);

                    //m_Comms.Notify("NAV_SPEED_IMU", sin(yaw)*velocity_mag_XY);
                    //m_Comms.Notify("V_IMU", cos(yaw)*velocity_mag_XY);
                }
            }

            else if (an_packet->id == packet_id_unix_time)
            {
                if (!unix_time_packet_received)
                {
                    unique_packet_count++;
                    unix_time_packet_received = true;
                }

                if(decode_unix_time_packet(&unix_time_packet, an_packet) == 0)
                {
                    //printf("Unix Time Packet:\n");
                    //printf("Seconds: %d\n", unix_time_packet.unix_time_seconds);
                    m_Comms.Notify("TimeUnix1_Seconds", unix_time_packet.unix_time_seconds);
                    m_Comms.Notify("TimeUnix2_Microseconds", unix_time_packet.microseconds);
                }
            }

            else if (an_packet->id == packet_id_ecef_position)
            {
                if (!ecef_position_packet_received)
                {
                    unique_packet_count++;
                    ecef_position_packet_received = true;
                }

                if(decode_ecef_position_packet(&ecef_position_packet, an_packet) == 0)
                {
                    //printf("ECEF Position Packet:\n");
                    m_Comms.Notify("Position_ECEF1_x", ecef_position_packet.position[0]);
                    m_Comms.Notify("Position_ECEF2_y", ecef_position_packet.position[1]);
                    m_Comms.Notify("Position_ECEF3_z", ecef_position_packet.position[2]);
                }
            }

            else if (an_packet->id == packet_id_body_velocity)
            {
                if (!body_velocity_packet_received)
                {
                    unique_packet_count++;
                    body_velocity_packet_received = true;
                }

                if(decode_body_velocity_packet(&body_velocity_packet, an_packet) == 0)
                {
                    //printf("Body Velocity Packet:\n");
                    m_Comms.Notify("Velocity1_x", body_velocity_packet.velocity[0]);
                    m_Comms.Notify("Velocity2_y", body_velocity_packet.velocity[1]);
                    m_Comms.Notify("Velocity3_z", body_velocity_packet.velocity[2]);

                }
            }

            else if (an_packet->id == packet_id_acceleration)
            {
                if (!acceleration_packet_received)
                {
                    unique_packet_count++;
                    acceleration_packet_received = true;
                }

                if(decode_acceleration_packet(&acceleration_packet, an_packet) == 0)
                {
                    //printf("Acceleration Packet:\n");
                    printf("AccelerationWithG_x: %lf\n", acceleration_packet.acceleration[0]);
                    m_Comms.Notify("AccelerationWithG_x", acceleration_packet.acceleration[0]);
                    m_Comms.Notify("AccelerationWithG_y", acceleration_packet.acceleration[1]);
                    m_Comms.Notify("AccelerationWithG_z", acceleration_packet.acceleration[2]);
                }
            }

            else if (an_packet->id == packet_id_quaternion_orientation)
            {
                if (!quaternion_orientation_packet_received)
                {
                    unique_packet_count++;
                    quaternion_orientation_packet_received = true;
                }

                if(decode_quaternion_orientation_packet(&quaternion_orientation_packet, an_packet) == 0)
                {
                    //printf("Quaternion Orientation Packet:\n");
                    m_Comms.Notify("Quaternion_q0", quaternion_orientation_packet.orientation[0]);
                    m_Comms.Notify("Quaternion_q1", quaternion_orientation_packet.orientation[1]);
                    m_Comms.Notify("Quaternion_q2", quaternion_orientation_packet.orientation[2]);
                    m_Comms.Notify("Quaternion_q3", quaternion_orientation_packet.orientation[3]);
                }
            }

            else if (an_packet->id == packet_id_dcm_orientation)
            {
                if (!dcm_orientation_packet_received)
                {
                    unique_packet_count++;
                    dcm_orientation_packet_received = true;
                }

                if(decode_dcm_orientation_packet(&dcm_orientation_packet, an_packet) == 0)
                {
                    //printf("DCM Orientation Packet:\n");

                    snprintf(buffer, 1024, "DCM[[%lf,%lf,%lf][%lf,%lf,%lf][%lf,%lf,%lf])",
                             dcm_orientation_packet.orientation[0][0], dcm_orientation_packet.orientation[0][1], dcm_orientation_packet.orientation[0][2],
                             dcm_orientation_packet.orientation[1][0], dcm_orientation_packet.orientation[1][1], dcm_orientation_packet.orientation[1][2],
                             dcm_orientation_packet.orientation[2][0], dcm_orientation_packet.orientation[2][1], dcm_orientation_packet.orientation[2][2]);

                    m_Comms.Notify("DirectionCosineMatrix", buffer);

                    m_Comms.Notify("DirectionCosineMatrix_00", dcm_orientation_packet.orientation[0][0]);
                    m_Comms.Notify("DirectionCosineMatrix_01", dcm_orientation_packet.orientation[0][1]);
                    m_Comms.Notify("DirectionCosineMatrix_02", dcm_orientation_packet.orientation[0][2]);
                    m_Comms.Notify("DirectionCosineMatrix_10", dcm_orientation_packet.orientation[1][0]);
                    m_Comms.Notify("DirectionCosineMatrix_11", dcm_orientation_packet.orientation[1][1]);
                    m_Comms.Notify("DirectionCosineMatrix_12", dcm_orientation_packet.orientation[1][2]);
                    m_Comms.Notify("DirectionCosineMatrix_20", dcm_orientation_packet.orientation[2][0]);
                    m_Comms.Notify("DirectionCosineMatrix_21", dcm_orientation_packet.orientation[2][1]);
                    m_Comms.Notify("DirectionCosineMatrix_22", dcm_orientation_packet.orientation[2][2]);

                }
            }

            else if (an_packet->id == packet_id_angular_acceleration)
            {
                if (!angular_acceleration_packet_received)
                {
                    unique_packet_count++;
                    angular_acceleration_packet_received = true;
                }

                if(decode_angular_acceleration_packet(&angular_acceleration_packet, an_packet) == 0)
                {
                    //printf("Angular Acceleration Packet:\n");
                    m_Comms.Notify("AccelerationAngular_x", angular_acceleration_packet.angular_acceleration[0]);
                    m_Comms.Notify("AccelerationAngular_y", angular_acceleration_packet.angular_acceleration[1]);
                    m_Comms.Notify("AccelerationAngular_z", angular_acceleration_packet.angular_acceleration[2]);
                }
            }

            else if (an_packet->id == packet_id_running_time)
            {
                if (!running_time_packet_received)
                {
                    unique_packet_count++;
                    running_time_packet_received = true;
                }

                if(decode_running_time_packet(&running_time_packet, an_packet) == 0)
                {
                    //printf("Running Time Packet:\n");
                    m_Comms.Notify("TimeRunningTime1_seconds", running_time_packet.seconds);
                    m_Comms.Notify("TimeRunningTime2_microseconds", running_time_packet.microseconds);
                }
            }

            else if (an_packet->id == packet_id_local_magnetics)
            {
                if (!local_magnetics_packet_received)
                {
                    unique_packet_count++;
                    local_magnetics_packet_received = true;
                }

                if(decode_local_magnetics_packet(&local_magnetics_packet, an_packet) == 0)
                {
                    //printf("Local Magnetics Packet:\n");
                    m_Comms.Notify("MagneticFieldLocal_x", local_magnetics_packet.magnetic_field[0]);
                    m_Comms.Notify("MagneticFieldLocal_y", local_magnetics_packet.magnetic_field[1]);
                    m_Comms.Notify("MagneticFieldLocal_z", local_magnetics_packet.magnetic_field[2]);
                }
            }

            else if (an_packet->id == packet_id_odometer_state)
            {
                if (!odometer_state_packet_received)
                {
                    unique_packet_count++;
                    odometer_state_packet_received = true;
                }

                if(decode_odometer_state_packet(&odometer_state_packet, an_packet) == 0)
                {
                    //printf("Odometer State Packet:\n");
                    m_Comms.Notify("Odometry1_distance", odometer_state_packet.distance);
                    m_Comms.Notify("Odometry2_speed", odometer_state_packet.speed);
                    m_Comms.Notify("Odometry3_slip", odometer_state_packet.slip);
                    m_Comms.Notify("Odometry4_isActive", odometer_state_packet.active);
                    m_Comms.Notify("Odometry5_pulseCount", odometer_state_packet.pulse_count);
                }
            }

            else if (an_packet->id == packet_id_geoid_height)
            {
                if (!geoid_height_packet_received)
                {
                    unique_packet_count++;
                    geoid_height_packet_received = true;
                }

                if(decode_geoid_height_packet(&geoid_height_packet, an_packet) == 0)
                {
                    //printf("Geoid Height Packet:\n");
                    m_Comms.Notify("Height_geoid", geoid_height_packet.geoid_height);
                }
            }

            else if (an_packet->id == packet_id_heave)
            {
                if (!heave_packet_received)
                {
                    unique_packet_count++;
                    heave_packet_received = true;
                }

                if(decode_heave_packet(&heave_packet, an_packet) == 0)
                {
                    //printf("Heave Packet:\n");
                    /*
                    m_Comms.Notify("Heave_p1", heave_packet.heave_point_1);
                    m_Comms.Notify("Heave_p2", heave_packet.heave_point_2);
                    m_Comms.Notify("Heave_p3", heave_packet.heave_point_3);
                    m_Comms.Notify("Heave_p4", heave_packet.heave_point_4);
                    */
                }
            }

            else
            {
                printf("Packet ID %u of Length %u\n", an_packet->id, an_packet->length);
            }

            /* Ensure that you free the an_packet when your done with it or you will leak memory */
            an_packet_free(&an_packet);
        }

         cout << "Num of unique packets scanned: " << unique_packet_count << endl;


        return(true);
    }


    return(true);
}
int imuSpatial_MOOSApp::an_packet_transmit(an_packet_t *an_packet)
{
    an_packet_encode(an_packet);
    return SendBuf(an_packet_pointer(an_packet), an_packet_size(an_packet));
}

void imuSpatial_MOOSApp::request_packet(uint8_t requested_packet_id)
{
    an_packet_t *an_packet = encode_request_packet(requested_packet_id);
    /*send the packet request to spatial*/
    an_packet_transmit(an_packet);

    an_packet_free(&an_packet);
}

void imuSpatial_MOOSApp::request_unix_time_packets()
{
    /*create the custom packet request*/
    an_packet_t *an_packet = encode_request_packet(packet_id_unix_time);

    /*send the packet request to spatial*/
    an_packet_transmit(an_packet);

    an_packet_free(&an_packet);
}

void imuSpatial_MOOSApp::set_sensor_ranges()
{
    an_packet_t *an_packet;
    sensor_ranges_packet_t sensor_ranges_packet;

    sensor_ranges_packet.permanent = TRUE;
    sensor_ranges_packet.accelerometers_range = accelerometer_range_4g;
    sensor_ranges_packet.gyroscopes_range = gyroscope_range_500dps;
    sensor_ranges_packet.magnetometers_range = magnetometer_range_2g;

    an_packet = encode_sensor_ranges_packet(&sensor_ranges_packet);

    an_packet_transmit(an_packet);

    an_packet_free(&an_packet);
}
