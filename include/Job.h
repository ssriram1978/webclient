#ifndef Job_H
#define Job_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

//#include "JobFactory.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

class Job {
  private:
     uint16_t local_port;
     uint32_t local_ipv4;
     uint32_t remote_ipv4;
     uint8_t job_current_state;
     uint64_t total_number_of_iterations;
     
     //friend Job_Factory::move_Job();
     
  public:

    void set_Job(uint16_t local_port,uint32_t local_ipv4,uint32_t remote_ipv4);
    void increment_iteration_count();
    void set_current_job_state(uint8_t new_state);
    
    ~Job();

    uint64_t return_iteration_count();

    uint8_t return_current_job_state();
};
#endif
