/*******************************************************************
 *                                                                 *
 *   Copyright © 2012, Xavier Orr, Advanced Navigation Pty Ltd     *
 *                                                                 *
 *******************************************************************/

package com.advancednavigation.anPackets;

import java.util.Date;

public class ANPacket20
{
	public ANPacket23 status;

	public long utc_time;
	public long microseconds;
	public Date time;
	
	public double[] position;
	public float[] velocity;
	public float[] acceleration;
	public float gForce;
	public float[] orientation;
	public float[] angularVelocity;
	public float[] positionStandardDeviation;
	
	public ANPacket20()
	{
		status = new ANPacket23();
		utc_time = 0;
		microseconds = 0;
		time = new Date(0);
		position = new double[3];
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
		velocity = new float[3];
		velocity[0] = 0;
		velocity[1] = 0;
		velocity[2] = 0;
		acceleration = new float[3];
		acceleration[0] = 0;
		acceleration[1] = 0;
		acceleration[2] = 0;
		gForce = 0;
		orientation = new float[3];
		orientation[0] = 0;
		orientation[1] = 0;
		orientation[2] = 0;
		angularVelocity = new float[3];
		angularVelocity[0] = 0;
		angularVelocity[1] = 0;
		angularVelocity[2] = 0;
		positionStandardDeviation = new float[3];
		positionStandardDeviation[0] = (float)1000000.0;
		positionStandardDeviation[1] = (float)1000000.0;
		positionStandardDeviation[2] = (float)1000000.0;
	}

	public ANPacket20(ANPacket packet)
	{
		status = new ANPacket23(packet.data,0);
		
		utc_time = TypeConversion.bytesToUInt32(packet.data, 4);
		microseconds = TypeConversion.bytesToUInt32(packet.data, 8);
		time = new Date(utc_time*1000 + microseconds/1000);
		
		position = new double[3];
		position[0] = TypeConversion.bytesToDouble(packet.data, 12);
		position[1] = TypeConversion.bytesToDouble(packet.data, 20);
		position[2] = TypeConversion.bytesToDouble(packet.data, 28);
		
		velocity = new float[3];
		velocity[0] = TypeConversion.bytesToFloat(packet.data, 36);
		velocity[1] = TypeConversion.bytesToFloat(packet.data, 40);
		velocity[2] = TypeConversion.bytesToFloat(packet.data, 44);
		
		acceleration = new float[3];
		acceleration[0] = TypeConversion.bytesToFloat(packet.data, 48);
		acceleration[1] = TypeConversion.bytesToFloat(packet.data, 52);
		acceleration[2] = TypeConversion.bytesToFloat(packet.data, 56);
		gForce = TypeConversion.bytesToFloat(packet.data, 60);
		
		orientation = new float[3];
		orientation[0] = TypeConversion.bytesToFloat(packet.data, 64);
		orientation[1] = TypeConversion.bytesToFloat(packet.data, 68);
		orientation[2] = TypeConversion.bytesToFloat(packet.data, 72);
		
		angularVelocity = new float[3];
		angularVelocity[0] = TypeConversion.bytesToFloat(packet.data, 76);
		angularVelocity[1] = TypeConversion.bytesToFloat(packet.data, 80);
		angularVelocity[2] = TypeConversion.bytesToFloat(packet.data, 84);
		
		positionStandardDeviation = new float[3];
		positionStandardDeviation[0] = TypeConversion.bytesToFloat(packet.data, 88);
		positionStandardDeviation[1] = TypeConversion.bytesToFloat(packet.data, 92);
		positionStandardDeviation[2] = TypeConversion.bytesToFloat(packet.data, 96);
	}
}
