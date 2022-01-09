#include <iostream>
#include "destination.h"

/**************************************************************************************************************************************
* Source: Wikipedia
* MCS and PhyRate in 802.11ax - 1 spatial stream 
Modulation and coding schemes for single spatial stream MCS
index[i] 	Modulation
type 	Coding
rate 	Data rate (in Mbit/s)[ii]
				20 MHz channels 			40 MHz channels 		80 MHz channels 		160 MHz channels
				1600 ns GI[iii] 	800 ns GI 	1600 ns GI 	800 ns GI 	1600 ns GI 	800 ns GI 	1600 ns GI 	800 ns GI
0 	BPSK 		1/2 	8 	8.6 	16 	17.2 	34 	36.0 	68 	72
1 	QPSK 		1/2 	16 	17.2 	33 	34.4 	68 	72.1 	136 	144
2 	QPSK 		3/4 	24 	25.8 	49 	51.6 	102 	108.1 	204 	216
3 	16-QAM 		1/2 	33 	34.4 	65 	68.8 	136 	144.1 	272 	282
4 	16-QAM 		3/4 	49 	51.6 	98 	103.2 	204 	216.2 	408 	432
5 	64-QAM 		2/3 	65 	68.8 	130 	137.6 	272 	288.2 	544 	576
6 	64-QAM 		3/4 	73 	77.4 	146 	154.9 	306 	324.4 	613 	649
7 	64-QAM 		5/6 	81 	86.0 	163 	172.1 	340 	360.3 	681 	721
8 	256-QAM 	3/4 	98 	103.2 	195 	206.5 	408 	432.4 	817 	865
9 	256-QAM 	5/6 	108 	114.7 	217 	229.4 	453 	480.4 	907 	961
10 	1024-QAM 	3/4 	122 	129.0 	244 	258.1 	510 	540.4 	1021 	1081
11 	1024-QAM 	5/6 	135 	143.4 	271 	286.8 	567 	600.5 	1134 	1201 
**************************************************************************************************************************************/

//Class destination methods

//Constructor: 1 spatial stream - 800ns GI - 20MHz
destination::destination (int no, int mcs, double arrivalRate, int arrivalType)
{
	m_no					= no;
	m_mcs					= mcs;
	m_arrivalRate			= arrivalRate;
	m_arrivalDistribution	= arrivalType;

	switch(mcs)
	{	
		case 0: m_phyRate=8.6; break;
		case 1: m_phyRate=17.2; break;
		case 2: m_phyRate=25.8; break;
		case 3: m_phyRate=34.4; break;
		case 4: m_phyRate=51.6; break;
		case 5: m_phyRate=68.8; break;
		case 6: m_phyRate=77.4; break;
		case 7: m_phyRate=86.0; break;
		case 8: m_phyRate=103.2; break;
		case 9: m_phyRate=114.7; break;
		case 10: m_phyRate=129.0; break;
		case 11: m_phyRate=143.4; break;
		default: std::cerr << "MCS is not valid: abort." << std::endl;
		exit(1);
	}
};

destination::destination (double phyRate, int no, double arrivalRate, int arrivalType) 
{
	m_phyRate 				= phyRate; 
	m_no					= no; 
	m_mcs					= -1; 
	m_arrivalRate			= arrivalRate; 
	m_arrivalDistribution	= arrivalType;
};

bool destination::operator==(destination &aDest)
{
	if ((this->m_no == aDest.m_no)&&(this->m_mcs==aDest.m_mcs)&&(this->m_phyRate==aDest.m_phyRate)&&(this->m_arrivalRate==aDest.m_arrivalRate)&&(this->m_arrivalDistribution==aDest.m_arrivalDistribution)) 
	{
	  return true;
	} else {
	  return false;
	}
};

destination::~destination () {};
