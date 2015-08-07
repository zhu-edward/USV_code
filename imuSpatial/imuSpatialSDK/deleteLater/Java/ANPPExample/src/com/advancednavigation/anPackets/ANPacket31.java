/*******************************************************************
 *                                                                 *
 *   Copyright © 2012, Xavier Orr, Advanced Navigation Pty Ltd     *
 *                                                                 *
 *******************************************************************/

package com.advancednavigation.anPackets;

public class ANPacket31
{
	public int gpsSatellites;
	public int glonassSatellites;
	public int sbasSatellites;
	public int compassSatellites;
	public int galileoSatellites;
	public Satellite[] satellites;
	
	public ANPacket31()
	{
		gpsSatellites = 0;
		glonassSatellites = 0;
		sbasSatellites = 0;
		compassSatellites = 0;
		galileoSatellites = 0;
		satellites = new Satellite[0];
	}
	
	public ANPacket31(ANPacket packet)
	{
		gpsSatellites = 0;
		glonassSatellites = 0;
		sbasSatellites = 0;
		compassSatellites = 0;
		galileoSatellites = 0;
		satellites = new Satellite[packet.length/7];
		for(int i=0; i < satellites.length; i++)
		{
			satellites[i] = new Satellite();
			satellites[i].navigationSystem = packet.data[7*i]&0xFF;
			satellites[i].number = packet.data[7*i+1]&0xFF;
			satellites[i].frequency = packet.data[7*i+2];
			satellites[i].elevation = packet.data[7*i+3]&0xFF;
			satellites[i].azimuth = TypeConversion.bytesToUInt16(packet.data, 7*i+4);
			satellites[i].snr = packet.data[7*i+6]&0xFF;
			if(satellites[i].navigationSystem == 1) gpsSatellites++;
			else if(satellites[i].navigationSystem == 2) glonassSatellites++;
			else if(satellites[i].navigationSystem == 4) sbasSatellites++;
			else if(satellites[i].navigationSystem == 6) compassSatellites++;
			else if(satellites[i].navigationSystem == 5) galileoSatellites++;
		}
	}
	
	public class Satellite
	{
		public int navigationSystem;
		public int number;
		public int frequency;
		public int elevation;
		public int azimuth;
		public int snr;
	}
}
