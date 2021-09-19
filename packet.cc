#include <iostream>
#include "packet.h"

using namespace std;

packet::packet (int size, double arrival, destination dest) 
{
	m_size			= size; 
	m_arrival		= arrival; 
	m_destination	= dest;
};

packet::~packet() {};

