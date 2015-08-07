/*******************************************************************
 *                                                                 *
 *   Copyright © 2012, Xavier Orr, Advanced Navigation Pty Ltd     *
 *                                                                 *
 *******************************************************************/

package com.advancednavigation.anPackets;

public class ANPacket30
{
	public int gpsSatellites;
	public int glonassSatellites;
	public int sbasSatellites;
	public int compassSatellites;
	public int galileoSatellites;
	public float hdop;
	public float vdop;
	
	public ANPacket30()
	{
		gpsSatellites = 0;
		glonassSatellites = 0;
		sbasSatellites = 0;
		compassSatellites = 0;
		galileoSatellites = 0;
		hdop = 0;
		vdop = 0;
	}
	
	public ANPacket30(ANPacket packet)
	{
		gpsSatellites = packet.data[8]&0xFF;
		glonassSatellites = packet.data[9]&0xFF;
		compassSatellites = packet.data[10]&0xFF;
		galileoSatellites = packet.data[11]&0xFF;
		sbasSatellites = packet.data[12]&0xFF;
		hdop = TypeConversion.bytesToFloat(packet.data, 0);
		vdop = TypeConversion.bytesToFloat(packet.data, 4);
	}
}
