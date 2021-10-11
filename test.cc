#include <iostream>
#include <list>
#include <vector>
#include <random>
#include "packet.h"
#include "destination.h"
#include "event.h"
#include "transmission.h"

using namespace std;

int main(int argc, char* argv[])
{
  //Set destination 
  class destination dest1(1,1,2.0,1), dest2(2,2,4.0,1); 
  std::list<destination> dest_list;
  dest_list.push_back(dest1);
  dest_list.push_back(dest2);
   
  class packet pkt1(10000,2,dest2), pkt2(1000,1,dest1), pkt3(1500,0,dest2);
  std::list<packet> buffer;
  buffer.push_back(pkt1);
  buffer.push_back(pkt2);
  buffer.push_back(pkt3);

  double time;
  cout<<"[INFO]: TEST component, buffer size before calling fifo_aggregation: "<<buffer.size()<<endl;
  time = fifo_aggregation(buffer);
  cout<<"[INFO]: TEST component, time calculated = "<<time<<endl;
  cout<<"[INFO]: TEST component, buffer size after calling fifo_aggregation: "<<buffer.size()<<endl;
  return(0);
}
