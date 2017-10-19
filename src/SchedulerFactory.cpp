#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"
#include "SchedulerFactory.h"

using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

Scheduler_Factory* Scheduler_Factory::m_pInstance = NULL;


Scheduler_Factory::Scheduler_Factory()
{
  //Nothing
}


Scheduler_Factory::~Scheduler_Factory()
{
  //Nothing
}

Scheduler_Factory* Scheduler_Factory::Instance()
{       
    if(!m_pInstance)
    {
       m_pInstance = new Scheduler_Factory();
    }
    
    return m_pInstance;
}

void Scheduler_Factory::initialize(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address)
{

   //State Factory is already initialized. (All static functions)
   
   //Initialize Queue Factory
   Queue_Factory::Instance()->set_total_number_of_queues(State_Factory::get_total_number_of_states());
   
   //Initialize Job Factory
   Job_Factory::Instance()->create_Jobs(starting_port,ending_port,local_ipv4_address,remote_ipv4_address);

   
   //Initialize Thread Factory   
}


void Scheduler_Factory::run()
{
   //Enqueue all the Jobs to the queue listed on the first state.

}

void Scheduler_Factory::stop()
{
    //Delete all the Jobs from all the queues.
}

