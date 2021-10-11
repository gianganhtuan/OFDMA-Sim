#include <iostream>
#include <list>
#include <vector>
#include <random>
#include "transmission.h"

using namespace std;

//IMPORTANT: All the times in this file are given in micro seconds (mu seconds)

/* Discipline
 * 0: FIFO 
 * 1: FIFO with aggregation
 * 2: FIFO with OFDMA
 * 3: FIFO with hybrid aggregation and OFDMA
 *
 */ 

//1 spatial stream and GI=800microsec
//Line: MCS (from 0 to 11) 
//7 Column: RU-26 RU-52 RU-106 RU-242(20MHz) RU-484 (40MHz) RU-996 (80MHz) RU-2x996 (160MHz)
double phyRate[12][7] = {
			{0.9,	1.8,	3.8,	8.6,	17.2,	36.0,	72.1},
			{1.8,	3.5,	7.5,	17.2,	34.4,	72.1,	144.1},
			{2.6,	5.3,	11.3,	25.8,	51.6,	108.1,	216.2},
			{3.5,	7.1,	15.0,	34.4,	68.8,	144.1,	288.2},
			{5.3,	10.6,	22.5,	51.6,	103.2,	216.2,	432.4},
			{7.1,	14.1,	30.0,	68.8,	137.6,	288.2,	576.5},
			{7.9,	15.9,	33.8,	77.4,	154.9,	324.3,	648.5},
			{8.8,	17.6,	37.5,	86.0,	172.1,	360.3,	720.6},
			{10.6,	21.2,	45.0,	103.2,	206.5,	432.4,	864.7},
			{11.8,	23.5,	50.0,	114.7, 	229.4, 	480.4, 	960.7},
			{-1.0,	-1.0,	-1.0,	129.0,	258.1, 	540.4,	1080.9},
			{-1.0,	-1.0,	-1.0,	143.4,	286.8,	600.4,	1201.0}
			};

/* Global variables */

double accessTimeAndAck=132.0; //Time are given in the google sheets 



/* functions */

double getTransmissionTimePayload(packet aPacket, int ru)//ru: ressource unit (number of tones)
{
   switch(ru) 
   {
	   case 26:   return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][0]*1000000); break;
	   case 52:   return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][1]*1000000); break;
	   case 106:  return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][2]*1000000); break;
	   case 242:  return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][3]*1000000); break;
	   case 484:  return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][4]*1000000); break;
	   case 996:  return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][5]*1000000); break;
	   case 1992: return (8*aPacket.m_size) / (phyRate[aPacket.m_destination.m_mcs][6]*1000000); break;
       default: 
   	    cerr << "Error getTransmissionTimePayload: Number of tones is not supported (ru=" << ru << endl;
   	    exit(1);
   }
};

double fifo(list<packet> &buffer)
{
  //std::list<packet>::iterator it;
  cout<<"[INFO]: FIFO component, before pop out packet, buffer size = "<<buffer.size()<<endl;
  packet pckt = buffer.front();
  buffer.pop_front();
  cout<<"[INFO]: FIFO component, after pop out packet, buffer size = "<<buffer.size()<<endl;
  cout<<"[INFO]: FIFO component, destination no  = "<<pckt.m_destination.m_no<<endl;
  cout<<"[INFO]: FIFO component, packet size = "<<pckt.m_size<<endl;
  cout<<"[INFO]: FIFO component, destination phyRate = "<<pckt.m_destination.m_phyRate<<endl;
  return (8*pckt.m_size)/(pckt.m_destination.m_phyRate*1000000);
};

double fifo_aggregation(std::list<packet> &buffer) 
{
	double txOP = 10.5;	//TxOP in miliseconds
	double aggregation_frame_size = 11454;	//Max aggregation frame size in Bytes
	double max_frame_size, current_frame_size=0.0;
	int no_aggregated_pkt = 0;
	bool aggregation = 0;
	
	//Get the first packet in buffer to see the destination
	cout<<"[INFO]: FIFO_Aggregation component, buffer size before pop out the first packet = "<<buffer.size()<<endl;
	packet pckt = buffer.front();
	buffer.pop_front();
	cout<<"[INFO]: FIFO_Aggregation component, buffer size after pop out the first packet = "<<buffer.size()<<endl;
	max_frame_size = min(aggregation_frame_size, pckt.m_destination.m_phyRate*txOP*1000/8);
    current_frame_size = pckt.m_size;
	cout<<"[INFO]: FIFO_Aggregation component, max frame size for this packet destination = "<<max_frame_size
		<<" current frame size = "<<current_frame_size<<" packet destination = "<<pckt.m_destination.m_no<<endl;
	
	// Now, scan the buffer to see packet with the same destination with the first one above
	// If there is one, verify the frame size then aggregate it into the first packet, 
	// pop out the aggregated packet out of the buffer, count the number of packet to be aggregated,
	// if aggregation happens then no need to increase iterator since after erase() the iterator 
	// automatically point to the next item.
	std::list<packet>::iterator it;
	it=buffer.begin();
	int loop = buffer.size();
    while(loop > 0)
	{
		cout<<"[INFO]: FIFO_Aggregation component, processing a packet with destination = "<<it->m_destination.m_no
			<<", buffer size = "<<buffer.size()<<endl;
		aggregation = 0;
		if (it->m_destination.m_no == pckt.m_destination.m_no)
		{
			if (current_frame_size + it->m_size <= max_frame_size)
			{
				current_frame_size += it->m_size;
				no_aggregated_pkt += 1;
				buffer.erase(it);
				aggregation = 1;
				cout<<"[INFO]: FIFO_Aggregation component, aggregated a packet, current frame size = "<<current_frame_size
					<<", no of aggregated pkt = "<<no_aggregated_pkt<<", now buffer size = "<<buffer.size()<<endl;
			}
		}
		if (aggregation == 0) 
		{
			cout<<"[INFO]: FIFO_Aggregation component, aggregation not happen, buffer size = "<<buffer.size()<<endl;
			it++;
		}
		loop--;
	}
	
	return (8*current_frame_size)/(pckt.m_destination.m_phyRate*1000000);	//to do: frame header is also needed to add
};

double fifo_ofdma(std::list<packet> &buffer)
{
	double txOP = 10.5;	//TxOP in miliseconds
	int max_52tones_sta, no_tones, bandwidth = 20; //Bandwidth 20 - 160 Mhz
	bool ru = 1; //Still have RU to be allocated
	
	switch (bandwidth)
	{
		case 20:  no_tones = 242;  max_52tones_sta = 4;  break;
		case 40:  no_tones = 484;  max_52tones_sta = 8;  break;
		case 80:  no_tones = 996;  max_52tones_sta = 16; break;
		case 160: no_tones = 1992; max_52tones_sta = 32; break;
	    default: 
		    cerr << "Error fifo_ofdma: the bandwidth is not supported (bandwidth=" << bandwidth << endl;
		    exit(1);
	}
	
	std::list<destination> destination_list;
	std::list<destination>::iterator d_it;
	
	double ru_allocation, min, diff[7];
	bool new_destination = 1;
	
	std::list<packet>::iterator it;
	it=buffer.begin();
    while((it!=buffer.end())&&(ru==1))
	{
		//Scan packet, check if this is new destination then try to put in OFDMA frame
		new_destination = 1;
		d_it = destination_list.begin();
		while (d_it!=destination_list.end())
		{
			if (d_it->m_no == it->m_destination.m_no) new_destination = 0; 
			d_it++;
		}
		
		//New destination, consider to put in OFDMA frame
		if (new_destination == 1) 
		{
			//Compute the best RU allocation for this packet
			ru_allocation = 0;
			min = txOP;
			diff[0] = txOP - getTransmissionTimePayload(*it, 26);
			diff[1] = txOP - getTransmissionTimePayload(*it, 52);
			diff[2] = txOP - getTransmissionTimePayload(*it, 106);
			diff[3] = txOP - getTransmissionTimePayload(*it, 242);
			diff[4] = txOP - getTransmissionTimePayload(*it, 484);
			diff[5] = txOP - getTransmissionTimePayload(*it, 996);
			diff[6] = txOP - getTransmissionTimePayload(*it, 1992);
			
			for (int i=0;i<7;i++)
			{
				if (diff[i]>0 && diff[i]<min) min = diff[i];
			}
			
			int k=99;
			
			for (int i=0;i<7;i++)
			{
				if (min == diff[i]) k = i;
			}
			
			switch(k)
			{
				case 0: ru_allocation = 26; break;
				case 1: ru_allocation = 52; break;
				case 2: ru_allocation = 106; break;
				case 3: ru_allocation = 242; break;
				case 4: ru_allocation = 484; break;
				case 5: ru_allocation = 996; break;
				case 6: ru_allocation = 1992; break;
				default: ru_allocation = 2000; break;	//Packet size is too large, cannot fit in any RU
			}
			
			if (no_tones >= ru_allocation)	//Remain RU is sufficient for this packet
			{
				no_tones -= ru_allocation;
				buffer.erase(it);
				it--;
			} 
			
			if (no_tones >= 26)	//Remain RU is not sufficient for this packet, but may be for another one
			{
				it++;
			} else {			//Run out of RU
				ru = 0;	
			}
			
		} else {
			it++; //Not a new destination, so skip this packet, move to the next one
		}
	}
	return txOP;	//to-do: add time for the header, remember take into account the number of destination
};

//Return the transmission time (time to access the medium + payload + queue + SIFS + BACK)
double transmitNextPackets(int discipline, list<packet> &buffer)
{
  double txTime=0.0;
  switch(discipline)
  {
    case 0: txTime = fifo(buffer); break;
    default: 
	    cerr << "Error transmitNextPackets: the discipline is not supported (disc=" << discipline << endl;
	    exit(1);
  }
  return txTime;
};
