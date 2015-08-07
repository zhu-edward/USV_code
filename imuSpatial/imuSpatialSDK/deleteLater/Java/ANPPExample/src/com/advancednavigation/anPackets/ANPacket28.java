/*******************************************************************
 *                                                                 *
 *   Copyright © 2012, Xavier Orr, Advanced Navigation Pty Ltd     *
 *                                                                 *
 *******************************************************************/

package com.advancednavigation.anPackets;

public class ANPacket28
{
	public float[] accelerometers;
	public float[] gyroscopes;
	public float[] magnetometers;
	public float imuTemperature;
	public float pressure;
	public float pressureTemperature;
	
	public ANPacket28()
	{
		accelerometers = new float[3];
		accelerometers[0] = 0;
		accelerometers[1] = 0;
		accelerometers[2] = 0;
		gyroscopes = new float[3];
		gyroscopes[0] = 0;
		gyroscopes[1] = 0;
		gyroscopes[2] = 0;
		magnetometers = new float[3];
		magnetometers[0] = 0;
		magnetometers[1] = 0;
		magnetometers[2] = 0;
		imuTemperature = 0;
		pressure = 0;
		pressureTemperature = 0;
	}
	
	public ANPacket28(ANPacket packet)
	{
		accelerometers = new float[3];
		accelerometers[0] = TypeConversion.bytesToFloat(packet.data, 0);
		accelerometers[1] = TypeConversion.bytesToFloat(packet.data, 4);
		accelerometers[2] = TypeConversion.bytesToFloat(packet.data, 8);
		gyroscopes = new float[3];
		gyroscopes[0] = TypeConversion.bytesToFloat(packet.data, 12);
		gyroscopes[1] = TypeConversion.bytesToFloat(packet.data, 16);
		gyroscopes[2] = TypeConversion.bytesToFloat(packet.data, 20);
		magnetometers = new float[3];
		magnetometers[0] = TypeConversion.bytesToFloat(packet.data, 24);
		magnetometers[1] = TypeConversion.bytesToFloat(packet.data, 28);
		magnetometers[2] = TypeConversion.bytesToFloat(packet.data, 32);
		imuTemperature = TypeConversion.bytesToFloat(packet.data, 36);
		pressure = TypeConversion.bytesToFloat(packet.data, 40);
		pressureTemperature = TypeConversion.bytesToFloat(packet.data, 44);
	}
}
