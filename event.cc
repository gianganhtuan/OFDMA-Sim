#include <iostream>
#include "event.h"

using namespace std;

std::ostream& operator<<(std::ostream& os, const event& evt)
{
    if( evt.m_type==0 ) os << "Event: arrival / ";	
    else os << "Event: transmission / ";	

    os << "dest= " << evt.m_dest.m_no << " / time = " << evt.m_time << endl;;
    return os;
};

event::event(int type, destination dest, double time)
{
	m_type	= type;
	m_dest	= dest;
	m_time	= time;
};

event::~event() {};

void event::print() 
{
	if (m_type == 0) cout<<"Arrival event / "; else cout<<"Transmission event / ";
	cout<<"dest = "<<m_dest.m_no<<" /time = "<<m_time<<endl;
};