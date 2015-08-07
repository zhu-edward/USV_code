/****************************************************************/
/*                                                              */
/*          Advanced Navigation Packet Protocol Library         */
/*          C Language Dynamic Spatial SDK, Version 3.0         */
/*   Copyright 2013, Xavier Orr, Advanced Navigation Pty Ltd    */
/*                                                              */
/****************************************************************/
/*
 * Copyright (C) 2013 Advanced Navigation Pty Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232/rs232.h"
#include "an_packet_protocol.h"
#include "spatial_packets.h"

#include <stdbool.h>

#define RADIANS_TO_DEGREES (180.0/M_PI)

int an_packet_transmit(an_packet_t *an_packet)
{
	an_packet_encode(an_packet);
	return SendBuf(an_packet_pointer(an_packet), an_packet_size(an_packet));
}

/*
 * This is an example of sending a configuration packet to Spatial.
 *
 * 1. First declare the structure for the packet, in this case sensor_ranges_packet_t.
 * 2. Set all the fields of the packet structure
 * 3. Encode the packet structure into an an_packet_t using the appropriate helper function
 * 4. Send the packet
 * 5. Free the packet
 */
void set_sensor_ranges()
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

void request_packet(uint8_t requested_packet_id)
{
    an_packet_t *an_packet = encode_request_packet(requested_packet_id);
    /*send the packet request to spatial*/
    an_packet_transmit(an_packet);

    an_packet_free(&an_packet);
}

void request_unix_time_packets()
{
    /*create the custom packet request*/
    an_packet_t *an_packet = encode_request_packet(packet_id_unix_time);

    /*send the packet request to spatial*/
    an_packet_transmit(an_packet);

    an_packet_free(&an_packet);
}

int main(int argc, char *argv[])
{
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
    int unique_packet_count;

	if (argc != 3)
	{
		printf("Usage - program com_port baud_rate\nExample - packet_example.exe COM1 115200\n");
		exit(EXIT_FAILURE);
	}
	

	/* open the com port */
	if (OpenComport(argv[1], atoi(argv[2])))
	{
		exit(EXIT_FAILURE);
	}
	
	an_decoder_initialise(&an_decoder);


	while (1)
	{

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

        unique_packet_count = 0;

        if ((bytes_received = PollComport(an_decoder_pointer(&an_decoder), an_decoder_size(&an_decoder))) > 0)
        {
            //printf("polling success. bytes_received: %d\n", bytes_received);

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
                    {
                        //printf("System State Packet:\n");
                        //printf("\tLatitude = %f, Longitude = %f, Height = %f\n", system_state_packet.latitude * RADIANS_TO_DEGREES, system_state_packet.longitude * RADIANS_TO_DEGREES, system_state_packet.height);
                        //m_Comms.Notify("Nav1_Lat", system_state_packet.latitude * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("Nav2_Long", system_state_packet.longitude * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("Nav3_Height", system_state_packet.height * RADIANS_TO_DEGREES);
                        //printf("\tRoll = %f, Pitch = %f, Heading = %f\n", system_state_packet.orientation[0] * RADIANS_TO_DEGREES, system_state_packet.orientation[1] * RADIANS_TO_DEGREES, system_state_packet.orientation[2] * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("Orientation1_Roll", system_state_packet.orientation[0] * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("Orientation2_Pitch", system_state_packet.orientation[1] * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("Orientation3_Yaw", system_state_packet.orientation[2] * RADIANS_TO_DEGREES);

                        //m_Comms.Notify("VelocityAngular1_x", system_state_packet.angular_velocity[0] * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("VelocityAngular2_y", system_state_packet.angular_velocity[1] * RADIANS_TO_DEGREES);
                        //m_Comms.Notify("VelocityAngular3_z", system_state_packet.angular_velocity[2] * RADIANS_TO_DEGREES);

                        //m_Comms.Notify("VelocityNED1_N", system_state_packet.velocity[0]);
                        //m_Comms.Notify("VelocityNED2_E", system_state_packet.velocity[1]);
                        //m_Comms.Notify("VelocityNED3_D", system_state_packet.velocity[2]);

                        //m_Comms.Notify("Acceleration1_x", system_state_packet.body_acceleration[0]);
                        //m_Comms.Notify("Acceleration2_y", system_state_packet.body_acceleration[1]);
                        //m_Comms.Notify("Acceleration3_z", system_state_packet.body_acceleration[2]);

                        //m_Comms.Notify("G_Force", system_state_packet.g_force);



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
                        //m_Comms.Notify("Temperature_imu", raw_sensors_packet.imu_temperature);
                        //m_Comms.Notify("Pressure", raw_sensors_packet.pressure);
                        //m_Comms.Notify("Pressure_temperature", raw_sensors_packet.pressure_temperature);
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
                            printf("Seconds: %d\n", unix_time_packet.unix_time_seconds);
                        //printf("MicroSeconds: %d\n", unix_time_packet.microseconds);
                        //m_Comms.Notify("TimeUnix1_Seconds", unix_time_packet.unix_time_seconds);
                        //m_Comms.Notify("TimeUnix2_Microseconds", unix_time_packet.microseconds);
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
                        //printf("Position_ECEF1_x: %lf\n", ecef_position_packet.position[0]);
                        //m_Comms.Notify("Position_ECEF1_x", ecef_position_packet.position[0]);
                        //m_Comms.Notify("Position_ECEF2_y", ecef_position_packet.position[1]);
                        //m_Comms.Notify("Position_ECEF3_z", ecef_position_packet.position[2]);
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
                        //m_Comms.Notify("Velocity1_x", body_velocity_packet.velocity[0]);
                        //m_Comms.Notify("Velocity2_y", body_velocity_packet.velocity[1]);
                        //m_Comms.Notify("Velocity3_z", body_velocity_packet.velocity[2]);

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
                        //m_Comms.Notify("AccelerationWithG_x", acceleration_packet.acceleration[0]);
                        //m_Comms.Notify("AccelerationWithG_y", acceleration_packet.acceleration[1]);
                        //m_Comms.Notify("AccelerationWithG_z", acceleration_packet.acceleration[2]);
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
                        //m_Comms.Notify("Quaternion_q0", quaternion_orientation_packet.orientation[0]);
                        //m_Comms.Notify("Quaternion_q1", quaternion_orientation_packet.orientation[1]);
                        //m_Comms.Notify("Quaternion_q2", quaternion_orientation_packet.orientation[2]);
                        //m_Comms.Notify("Quaternion_q3", quaternion_orientation_packet.orientation[3]);
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

                        //snprintf(buffer, 1024, "DCM[[%lf,%lf,%lf][%lf,%lf,%lf][%lf,%lf,%lf])",
                        //         dcm_orientation_packet.orientation[0][0], dcm_orientation_packet.orientation[0][1], dcm_orientation_packet.orientation[0][2],
                        //         dcm_orientation_packet.orientation[1][0], dcm_orientation_packet.orientation[1][1], dcm_orientation_packet.orientation[1][2],
                        //         dcm_orientation_packet.orientation[2][0], dcm_orientation_packet.orientation[2][1], dcm_orientation_packet.orientation[2][2]);

                        //m_Comms.Notify("DirectionCosineMatrix", buffer);

                        //m_Comms.Notify("DirectionCosineMatrix_00", dcm_orientation_packet.orientation[0][0]);
                        //m_Comms.Notify("DirectionCosineMatrix_01", dcm_orientation_packet.orientation[0][1]);
                        //m_Comms.Notify("DirectionCosineMatrix_02", dcm_orientation_packet.orientation[0][2]);
                        //m_Comms.Notify("DirectionCosineMatrix_10", dcm_orientation_packet.orientation[1][0]);
                        //m_Comms.Notify("DirectionCosineMatrix_11", dcm_orientation_packet.orientation[1][1]);
                        //m_Comms.Notify("DirectionCosineMatrix_12", dcm_orientation_packet.orientation[1][2]);
                        //m_Comms.Notify("DirectionCosineMatrix_20", dcm_orientation_packet.orientation[2][0]);
                        //m_Comms.Notify("DirectionCosineMatrix_21", dcm_orientation_packet.orientation[2][1]);
                        //m_Comms.Notify("DirectionCosineMatrix_22", dcm_orientation_packet.orientation[2][2]);

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
                        //m_Comms.Notify("AccelerationAngular_x", angular_acceleration_packet.angular_acceleration[0]);
                        //m_Comms.Notify("AccelerationAngular_y", angular_acceleration_packet.angular_acceleration[1]);
                        //m_Comms.Notify("AccelerationAngular_z", angular_acceleration_packet.angular_acceleration[2]);
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
                        //m_Comms.Notify("TimeRunningTime1_seconds", running_time_packet.seconds);
                        //m_Comms.Notify("TimeRunningTime2_microseconds", running_time_packet.microseconds);
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
                        //m_Comms.Notify("MagneticFieldLocal_x", local_magnetics_packet.magnetic_field[0]);
                        //m_Comms.Notify("MagneticFieldLocal_y", local_magnetics_packet.magnetic_field[1]);
                        //m_Comms.Notify("MagneticFieldLocal_z", local_magnetics_packet.magnetic_field[2]);
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
                        //m_Comms.Notify("Odometry1_distance", odometer_state_packet.distance);
                        //m_Comms.Notify("Odometry2_speed", odometer_state_packet.speed);
                        //m_Comms.Notify("Odometry3_slip", odometer_state_packet.slip);
                        //m_Comms.Notify("Odometry4_isActive", odometer_state_packet.active);
                        //m_Comms.Notify("Odometry5_pulseCount", odometer_state_packet.pulse_count);
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
                        //m_Comms.Notify("Height_geoid", geoid_height_packet.geoid_height);
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
                        //m_Comms.Notify("Heave_p1", heave_packet.heave_point_1);
                        //m_Comms.Notify("Heave_p2", heave_packet.heave_point_2);
                        //m_Comms.Notify("Heave_p3", heave_packet.heave_point_3);
                        //m_Comms.Notify("Heave_p4", heave_packet.heave_point_4);
                    }
                }

				else
				{
                    printf("Packet ID %u of Length %u\n\n\n\n\n\n\n\n", an_packet->id, an_packet->length);
				}
				
				/* Ensure that you free the an_packet when your done with it or you will leak memory */
				an_packet_free(&an_packet);


			}
            //printf("Num of unique packets scanned: %d\n", unique_packet_count);

		}
#ifdef _WIN32
    Sleep(10);
#else
    usleep(10000);
#endif
	}
}
