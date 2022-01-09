#ifndef TRANSMISSION_H
#define TRANSMISSION_H

#include <list>
#include <vector>
#include <set>
#include "packet.h"
#include "destination.h"

double alpha_i(packet pkt);

double getMacTransmissionTime(packet aPacket, int nbOfBytesMac, int ru);

int newDest(destination aDest, std::list<destination> destList);

double getTransmissionTimeOfdmaFrame(std::list<packet> packetsList, std::vector<int> ruList);

int * ruAllocation(std::list<packet> pkt_set);

void ruBruteForce(std::vector<int> ru, int noSta, std::list<std::vector<int>> &ruList);

std::vector<int> ruAllocationOptimal(std::list<packet> packetsList);

double objectiveFunction(std::list<packet> packetsList, std::vector<int> ruList);

double getTransmissionTimePayload(packet aPacket, int ru);

double fifo(std::list<packet> &buffer, double currentTime);

double fifoAggregation(std::list<packet> &buffer, double currentTime);

double fifoOFDMAOptimal(std::list<packet> &buffer, double currentTime);

double fifo_ofdma(std::list<packet> &buffer);

double transmitNextPackets(int discipline, std::list<packet> &buffer, double currentTime);

#endif
