#include<iostream>
#include<list>
#include<vector>
#include<random>
#include"packetAndDestination.h"

//IMPORTANT: All the times in this file are given in micro seconds (mu seconds)

/* Discipline
 * 0: FIFO 
 * 1: FIFO with aggregation
 *
 *
 *
 */ 

//1 spatial stream and GI=800microsec
//Line: MCS (from 0 to 11) 
//7 Column: RU-26 RU-52 RU-106 RU-242(20MHz) RU-484 (40MHz) RU-996 (80MHz) RU-2x996 (160MHz)
double phyRate[11][7] = {{0.9,1.8,3.8,8.6,17.2,36.0,72.1},{1.8,3.5,7.5,17.2,34.4,72.1,144.1},{2.6,5.3,11.3,25.8,51.6,108.1,216.2},{3.5,7.1,15.0,34.4,68.8,144.1,288.2},{5.3,10.6,22.5,51.6,103.2,216.2,432.4},{7.1,14.1,30.0,68.8,137.6,288.2,576.5},{7.9,15.9,33.8,77.4,154.9,324.3,648.5},{8.8,17.6,37.5,86.0,172.1,360.3,720.6},{10.6,21.2,45.0,103.2,206.5,432.4,864.7},{11.8,23.5,50.0,114.7 229.4 480.4 960.7},{-1.0,-1.0,-1.0,129.0,258.1 540.4,1080.9}, {-1.0,-1.0,-1.0,143.4,286.8,600.4,1201.0}}


using namespace std;

/* Global variables */

double accessTimeAndAck=132.0; //Time are given in the google sheets 



/* functions */

double getTransmissionTimePayload(packet aPacket, int ru)//ru: ressource unit (number of tones)
{

   switch(ru) 
   {

   }
}

//Return the transmission time (time to access the medium + payload + queue + SIFS + BACK)
double transmitNextPackets(int discipline, list<packet> buffer)
{
  switch(discipline)
  {
    case 0: fifo(buffer); break;
    default: 
	    cerr << "Error transmitNextPackets: the discipline is not supported (disc=" << discipline << endl;
	    exit(1);
  }

}

void fifo(list<packet> buffer)
{	
  //std::list<packet>::iterator it;
  packet pckt; 

  pckt=buffer.front();
   
  

}


