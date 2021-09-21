#ifndef EVENT_H
#define EVENT_H

#include "destination.h"

class event {
	public:
		int m_type; 		//0: arrival 1: transmission	  
		destination m_dest;   //for an arrival: the corresponding destination 
		double m_time;		//time of this even
		friend std::ostream& operator<<(std::ostream& os, const event& evt);
	
		event(int type, destination dest, double time);
		~event();
		void print();
}; 

#endif
