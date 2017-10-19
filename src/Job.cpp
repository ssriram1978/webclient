#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#include "Job.h"


void Job::set_Job(uint16_t local_port,uint32_t local_ipv4,uint32_t remote_ipv4)
{
   this->local_port=local_port;
   this->local_ipv4=local_ipv4;
   this->remote_ipv4=remote_ipv4;
   this->job_current_state=0;
   this->total_number_of_iterations=0;
}    

Job::~Job()
{
  //Do nothing
}

uint64_t Job::return_iteration_count()
{
   return this->total_number_of_iterations;
}

uint8_t Job::return_current_job_state()
{
   return this->job_current_state;
}

void Job::increment_iteration_count()
{
   this->total_number_of_iterations++;
}

void Job::set_current_job_state(uint8_t new_state)
{
   this->job_current_state=new_state;
}

