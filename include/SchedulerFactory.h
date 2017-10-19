#ifndef Scheduler_Factory_H
#define Scheduler_Factory_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

class Scheduler_Factory {
  private:
  Scheduler_Factory();

  static Scheduler_Factory* m_pInstance;
  
  public:
   ~Scheduler_Factory();   
   static Scheduler_Factory* Instance();
   void initialize(    
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address);
   void run();
   void stop();

};
#endif
