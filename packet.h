#ifndef PACKET_H
#define PACKET_H

#include "destination.h"

class packet {
	public:
		int			m_size; 		   //size in bytes
		double 		m_arrival; 		   //arrival time
		destination m_destination; //integer that identifies the destination
	
  		packet(int size, double arrival, destination dest);
		bool operator==(packet &aPacket);
		~packet();
}; 

#endif
