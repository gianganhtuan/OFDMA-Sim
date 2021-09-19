#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <list>
#include "packet.h"
#include "destination.h"

double getTransmissionTimePayload(packet aPacket, int ru);

void fifo(std::list<packet> &buffer);

double transmitNextPackets(int discipline, std::list<packet> &buffer);

#endif
