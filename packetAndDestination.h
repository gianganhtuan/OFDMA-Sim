#ifndef HEADER_PACKET
#define HEADER_PACKET

class destination{
 public:
   int 	  m_no;				//destination address: 1, 2, etc.
   int 	  m_mcs; 	 		//MCS index
   double m_phyRate; 			//Physical Trans Rate (Mbit/s) 
   double m_arrivalRate; 		//Number of packet per second for this source
   int    m_arrivalDistribution; 	//Type of distribution for the interarrival: Deterministic (0), Poisson(1), etc. 

   //Constructors
   destination(int no=0, int mcs=0, double arrivalRate=1.0, int arrivalType=0);  		 //phyRate will be set according to the mcs in the constructor (in the .cc file)
   //MCS is not specified here (we can put whatever we want for the phyRate) //A recoder pour prendre en compte les autres...
   destination(double phyRate, int no=0, double arrivalRate=1.0, int arrivalType=0) { m_phyRate=phyRate; m_no=no; m_mcs=-1; m_arrivalRate=arrivalRate; m_arrivalDistribution=arrivalType; }  

};

class packet{
 public: 
  int 	 m_size; 		   //size in bytes
  double m_arrival; 		   //arrival time
  class destination m_destination; //integer that identifies the destination

  packet(int size, double arrival, destination dest) { m_size=size; m_arrival=arrival; m_destination=dest; }

}; 


#endif
