#include <iostream>
#include <list>
#include <vector>
#include <random>
#include "packet.h"
#include "destination.h"
#include "event.h"
#include "transmission.h"

/* A faire
 * - traiter les evenements 1 a 1 --> transmission (schedule next Transmission) arrival (schedule nex arrival)
 */ 

using namespace std;

//Global variables 
double currentTime=0.0;
double startTxTime=0.0, stopTxTime=0.0; 

list<event> events;					//The events (discrete event simulator) - it is not a queue because it is not excatly FIFO (we dequeue at the fornt but insertion of event is not neceessarily at the back/end)
vector<destination> destinations;	//The destinations
list<packet> buffer;				//The buffer

mt19937 engine;						//The random number generator (the discrete one)

//Functions

double getSampleExpDistribution(double lambda)//lambda is 1/E[X] 
{
  std::exponential_distribution<> randomFlot(lambda);
  return(randomFlot(engine));
}

void printEvents()
{
  std::list<event>::iterator it;

  for(it=events.begin(); it!=events.end(); it++) cout << *it; 
}

void insertEvent(class event anEvent)
{
  std::list<event>::iterator it;

  it=events.begin();

  while(anEvent.m_time > it->m_time && it!=events.end()) it++;

  cout << "DEBUG: An event is added: " << anEvent << endl;
  events.insert(it,anEvent);
}

//Add the next arrival to the list of events for a given destination
void addNextArrival(class destination dest)
{
	double timeCalculate;
//  event newArrival;	

//  newArrival.m_type=0; 		//arrival
//  newArrival.m_dest=dest;

  switch(dest.m_arrivalDistribution)
  {
    case 0: timeCalculate=currentTime+1.0/dest.m_arrivalRate; break;
    case 1: 
    {//Les accolades sont necessaires car elle fixe la portee de la declaration de la variable ci-dessous
	    double value=getSampleExpDistribution(dest.m_arrivalRate);
	    timeCalculate=currentTime+value; 
	    cout << "DEBUG: rand exp value: " << value << endl;
	    break;
    }
    default: cerr << "Error in addNextArrival(): the destination m_arrivalDistribution member is incorrect (=" << dest.m_arrivalDistribution << ")" << endl;
	     exit(1);
  }
 
  //Insert this event in the list
  event newArrival(0, dest, timeCalculate);
  insertEvent(newArrival);
}

//Schedule the next transmission 
void scheduleNextTransmission(double time)
{
  event newTrans(1,-1,time);	

//  newTrans.m_type=1; 		//transmission
//  newTrans.m_dest=-1;		//it is not relevent here
//  newTrans.m_time=time;

  //Insert this event in the list
  insertEvent(newTrans);

}

void simulateQueue(list<packet> &buffer, int discipline)
{
  int nbOfTransmissions=0;
  int maxTransmission=100;
  double timeToTransmit;


  //Start of the simulation: we add an arrival for each destination
  if(events.empty()) 
  {
     for(auto it = std::begin(destinations); it != std::end(destinations); ++it) 
     {
       cout << "DEBUG: add new arrival\n";
       addNextArrival(*it);
     } 
  }

  printEvents();

  //The simulator that processes the events
  while (nbOfTransmissions < maxTransmission)
  {
    std::list<event>::iterator it;

    //What is the next event
	cout<<"[INFO]: Queue component, current list of events"<<endl;
	printEvents();
    it=events.begin();
    currentTime=it->m_time;
    event thisEvent=*it;		//We copy this event as we remove it from the list
    events.pop_front();			//We remove this event as it is process	
	cout<<"[INFO]: Queue component, we are processing ";
	thisEvent.print();
	printEvents();

    switch(thisEvent.m_type)  
    {
	//arrival
	case 0: 
	{
		if(buffer.empty() && (currentTime > stopTxTime)) scheduleNextTransmission(currentTime); else scheduleNextTransmission(stopTxTime);
		//If the buffer is empty we transmits at the arrival of this packet, but we have to check for are we transmitting or not?
		cout<<"[INFO]: Queue component, an Arrival event comes. Before addNextArrival(), number of Packet in Buffer = "<<buffer.size()<<endl;
		addNextArrival(thisEvent.m_dest);//schedule the next arrival for this destination
  		class packet newPacket(1000,currentTime,thisEvent.m_dest);//1000 bytes (needs to be changed)
  		buffer.push_back(newPacket);
		cout<<"[INFO]: Queue component, Current number of Packet in Buffer = "<<buffer.size()<<endl;
		break;
	}
	//transmission
	case 1: if(buffer.empty()) break;//If the buffer is empty there is no transmission
			if(currentTime < stopTxTime) { scheduleNextTransmission(stopTxTime); break; } //We have to defer this transmission because we are being in another transmission  
			else {
		cout<<"[INFO]: Queue component, buffer size ="<<buffer.size()<<" start tx time = "<<currentTime<<endl;
		startTxTime = currentTime;
		timeToTransmit = currentTime + transmitNextPackets(discipline, buffer, currentTime);
		//scheduleNextTransmission(timeToTransmit);
	 	nbOfTransmissions++;
		stopTxTime = timeToTransmit;
		cout<<"INFO: Queue component, a packet has been transmitted, buffer size ="<<buffer.size()<<" stop tx time = "<<stopTxTime<<endl;
		break; }
	default: cerr << "Error in simulateQueue(): the event type is incorrect (=" << thisEvent.m_type <<")\n";
		 exit(1);
    }

  }

  printEvents();
}

int main(int argc, char* argv[])
{
  int nbOfDest=2;	
  
  /* initialize random seed: */
  srand (time(NULL));

  //Set destination 
  class destination dest1(1,1,700.0,1), dest2(2,1,700.0,1), dest3(3,1,700.0,1); 
  destinations.push_back(dest1);
  destinations.push_back(dest2);
  destinations.push_back(dest3);
   
  //class packet myPacket(1,2,3);
  //cout << myPacket.m_destination << endl;
  //buffer.push_back(myPacket);

  //The simulator
  simulateQueue(buffer, 1);

  return(0);
}
