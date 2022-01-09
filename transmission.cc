#include <iostream>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
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
int bandwidth = 20, maxTones = 242, maxClient = 9, RuType[4] = {26, 52, 106, 242};
//int bandwidth = 40, maxTones = 484, maxClient = 18, maxRuType = 5;
//int bandwidth = 80, maxTones = 996, maxClient = 37, maxRuType = 6;
//int bandwidth = 160, maxTones = 1992, maxClient = 73, maxRuType = 7;

/* functions */
double getMacTransmissionTime(packet aPacket, int nbOfBytesMac, int ru)//ru: ressource unit (number of tones)
{
   switch(ru) 
   {
	   case 26:   return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][0]*1000000); break;
	   case 52:   return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][1]*1000000); break;
	   case 106:  return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][2]*1000000); break;
	   case 242:  return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][3]*1000000); break;
	   case 484:  return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][4]*1000000); break;
	   case 996:  return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][5]*1000000); break;
	   case 1992: return (8* (aPacket.m_size+nbOfBytesMac) ) / (phyRate[aPacket.m_destination.m_mcs][6]*1000000); break;
       default: 
   	    cerr << "Error getTransmissionTimePayload: Number of tones is not supported (ru=" << ru << endl;
   	    exit(1);
   }
};

int newDest(destination aDest, std::list<destination> destList)
{
  int i=0;
  for(auto it=destList.begin();it!=destList.end(); it++)
  {
    if( it->m_no == aDest.m_no ) return(i);
    i++;
  }
  return(-1);
}

double alpha_i(packet pkt)
{
	return (pkt.m_size*8.0)/(pkt.m_destination.m_phyRate*1000000);
};

double getTransmissionTimeOfdmaFrame(std::list<packet> packetsList, std::vector<int> ruList)
{
  double AIFS=0.000043; //musec
  double SIFS=0.000016; //musec
  double backoff=0.0000675; // Slot_time=9musec * 7.5 (backoff in [0-15])
  double phyHeader=0.000044; 

  //Algorithm: we have a single function for OFDMA, Aggreg and OFDMA+Aggreg
  //So, we may have different packets for the same destination. In this case they are sent in one MPDU one after the other.
  std::list<double> macTime;
  std::vector<int>::iterator ruListIt;

  ruListIt = ruList.begin();
  for(auto it=packetsList.begin(); it!=packetsList.end(); it++) 
  {
//  cout<<"Test ruList value = "<<*ruListIt<<", tx time = "<<getMacTransmissionTime(*it,30+32,RuType[*ruListIt])<<endl;
    macTime.push_back(getMacTransmissionTime(*it,30+32,RuType[*ruListIt]));
    ruListIt++;
  }
  
  double transmissionTime;

  //Two phyHeader: one for the frame and one for the BACK
  transmissionTime=AIFS+backoff+2*phyHeader+SIFS;
  transmissionTime+=*max_element(macTime.begin(), macTime.end()); //return the iterator on the max elt  
  
  return transmissionTime; 
}


int * ruAllocation(std::list<packet> pkt_set) 
{
	int *ru = new int[pkt_set.size()];
	double sum = 0;
	
	for (std::list<packet>::iterator it = pkt_set.begin(); it!=pkt_set.end(); it++) sum += alpha_i(*it);
	
	int i = 0;
	double tonesfloat[pkt_set.size()];
	
	for (std::list<packet>::iterator it = pkt_set.begin(); it!=pkt_set.end(); it++)
	{
		tonesfloat[i] = maxTones * alpha_i(*it) / sum;
		
		int distance[2] = {maxTones,0};
		double tmp = 0;
		for (int j=0; j< sizeof(RuType)/sizeof(RuType[0]); j++)
		{
			tmp = abs(tonesfloat[i] - RuType[j]);
			if (distance[0] > tmp) { distance[0] = tmp; distance[1] = j; }
		}
		ru[i] = distance[1];
		cout<<tonesfloat[i]<<" "<<ru[i]<<" "<<RuType[ru[i]]<<endl;
		i++;
	}
	
	int totalTones = 0;
	i = 0;
	for (int j=0; j < pkt_set.size(); j++) totalTones += RuType[ru[j]];
	cout<<"[INFO] RU Allocation component, after computing the floattones, total tones = "<<totalTones<<" number of ru size = "<<sizeof(ru)/sizeof(ru[0])<<endl;
	while (totalTones > maxTones)
	{
		if (ru[i]>0) ru[i]--;
		totalTones = 0;
		for (int k=0; k < pkt_set.size(); k++) totalTones += RuType[ru[k]];
		i++;
		cout<<"[INFO] RU Allocation component, total tones too large, reduce tones, total tones = "<<totalTones<<endl;
	}
	return ru;
};

void ruBruteForce(std::vector<int> ru, int noSta, std::list<std::vector<int>> &ruList)
{
  for (int i=0; i<sizeof(RuType)/sizeof(RuType[0]); i++)
  {
    std::vector<int> str;
    str = ru;
    str.push_back(i);
    if (noSta >0)
    {
      ruBruteForce(str, noSta-1, ruList);
    } else {
      ruList.push_back(str);
    }
  }
};

std::vector<int> ruAllocationOptimal(std::list<packet> packetsList)
{
  std::vector<double> ofValueList; //a vector of objectiveFunction computed values
  std::vector<int>::iterator ofValueIt; 
  
  std::vector<int> optimalValue; //return ru optimal vector

  int noSta = packetsList.size() - 1; //depth of BruteForce recursive layers

  std::vector<int> ru;
  std::vector<int>::iterator ruIt;
  std::list<std::vector<int>> ruList;
  std::list<std::vector<int>>::iterator ruListIt;

  ruBruteForce(ru, noSta, ruList);

  for (ruListIt=ruList.begin(); ruListIt!=ruList.end(); ruListIt++)
  {
    int totalTones = 0;
    for (ruIt=ruListIt->begin(); ruIt!=ruListIt->end(); ruIt++)
    {
      totalTones += RuType[*ruIt]; //compute total tones given by this combination
    }
    if (totalTones > maxTones) 
    {
      ofValueList.push_back(0.0);  //excess max no of tones, simply put 0 to the ofValueList
    } else {
      ofValueList.push_back(objectiveFunction(packetsList, *ruListIt));  //otherwise, compute objectiveFunction value of this combination, then push to ofValueList
    }
  }

  ruListIt = ruList.begin();
  std::advance(ruListIt, max_element(ofValueList.begin(), ofValueList.end()) - ofValueList.begin()); //find the index of best combination in ofValueList, then shift ruListIt according to that index
  optimalValue = *ruListIt;
  return optimalValue;
};

double objectiveFunction(std::list<packet> packetsList, std::vector<int> ruList)
{
  std::list<packet>::iterator it;
  double size = 0.0;
  
  for (it = packetsList.begin(); it!= packetsList.end(); it++)
  {
    size += it->m_size;
  }

  return size/getTransmissionTimeOfdmaFrame(packetsList, ruList);
};

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

double fifo(list<packet> &buffer, double currentTime)
{
  double transmissionTime;
  double AIFS=0.000043; //musec
  double SIFS=0.000016; //musec
  double backoff=0.0000675; // Slot_time=9musec * 7.5 (backoff in [0-15])
  double phyHeader=0.000044; 
  //std::list<packet>::iterator it;
  //cout<<"[INFO]: FIFO component, before pop out packet, buffer size = "<<buffer.size()<<endl;
  packet pckt = buffer.front();
  buffer.pop_front();
  //cout<<"[INFO]: FIFO component, after pop out packet, buffer size = "<<buffer.size()<<endl;
  //cout<<"[INFO]: FIFO component, destination no  = "<<pckt.m_destination.m_no<<endl;
  //cout<<"[INFO]: FIFO component, packet size = "<<pckt.m_size<<endl;
  cout<<"[INFO]: FIFO component, packet sent with destination = "<<pckt.m_destination.m_no
      <<", phyRate = "<<pckt.m_destination.m_phyRate
      <<", packetSize = "<<pckt.m_size
      <<", arrivalTime = "<<pckt.m_arrival
      <<", transmissionTime = "<<currentTime 
      <<endl;
  transmissionTime = AIFS+backoff+2*phyHeader+SIFS;
  transmissionTime+= (8*pckt.m_size)/(pckt.m_destination.m_phyRate*1000000);
  return transmissionTime;
};

double fifoAggregation(std::list<packet> &buffer, double currentTime) 
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

double fifoOFDMAOptimal(std::list<packet> &buffer, double currentTime)
{
  double txTime;
  std::vector<double> ofValueList;
  std::list<destination> destList;
  std::list<packet> tmpList;
  std::list<packet>::iterator packetIt,bufferIt;
  std::list<std::list<packet>> newDestList;
  std::list<std::list<packet>>::iterator newDestListIt;

  std::cout<<"ofvalue buffersize = "<<buffer.size()<<std::endl;

  packetIt = buffer.begin();
  int index = 0;

  while ((packetIt != buffer.end()) && (index < maxClient))
  {
    if (newDest(packetIt->m_destination.m_no, destList) == -1) //found packet with new destination
    {
      tmpList.push_back(*packetIt);
      ofValueList.push_back(objectiveFunction(tmpList, ruAllocationOptimal(tmpList)));
      destList.push_back(packetIt->m_destination.m_no);
      newDestList.push_back(tmpList);
      std::cout<<"ofvalue = "<<objectiveFunction(tmpList, ruAllocationOptimal(tmpList))<<std::endl;
      index++;
      packetIt++;
    } else {
      packetIt++; //destination already in the list, iterator point to the next packet
    }
  }

  //point iterator to the right list of packet in the new destination list, then pop them out from the buffer
  newDestListIt = newDestList.begin();
  std::advance(newDestListIt, max_element(ofValueList.begin(),ofValueList.end())-ofValueList.begin());
  tmpList.clear();
  for (packetIt = newDestListIt->begin(); packetIt != newDestListIt->end(); packetIt++)
  {
    tmpList.push_back(*packetIt);
    for (bufferIt = buffer.begin(); bufferIt != buffer.end(); bufferIt++)
    {
      if (*packetIt == *bufferIt) { 
        bufferIt = buffer.erase(bufferIt); bufferIt--; 
        cout<<"[INFO]: fifoOFDMAOptimal component, packet sent with destination = "<<packetIt->m_destination.m_no
            <<", phyRate = "<<packetIt->m_destination.m_phyRate
            <<", packetSize = "<<packetIt->m_size
            <<", arrivalTime = "<<packetIt->m_arrival
            <<", transmissionTime = "<<currentTime 
            <<endl;
      }
    }
  }

  txTime = getTransmissionTimeOfdmaFrame(tmpList, ruAllocationOptimal(tmpList));
  return txTime;
};

//Return the transmission time (time to access the medium + payload + queue + SIFS + BACK)
double transmitNextPackets(int discipline, list<packet> &buffer, double currentTime)
{
  double txTime=0.0;
  switch(discipline)
  {
    case 0: txTime = fifo(buffer, currentTime); break;
    case 1: txTime = fifoOFDMAOptimal(buffer, currentTime); break;
    default: 
	    cerr << "Error transmitNextPackets: the discipline is not supported (disc=" << discipline << endl;
	    exit(1);
  }
  return txTime;
};
