#include <iostream>
#include "packet.h"

using namespace std;

packet::packet (int size, double arrival, destination dest) 
{
	m_size			= size; 
	m_arrival		= arrival; 
	m_destination	= dest;
};

bool packet::operator==(packet &aPacket) 
{
        if ((this->m_size == aPacket.m_size)&&(this->m_arrival==aPacket.m_arrival)&&(this->m_destination==aPacket.m_destination))
        { 
          return true;
        } else {
          return false;
        }
};

packet::~packet() {};

