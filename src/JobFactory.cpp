#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"
#include "Job.h"

using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;


Job_Factory* Job_Factory::m_pInstance = NULL;

uint16_t Job_Factory::total_number_of_jobs = 0;


Job* Job_Factory::p_jobs = NULL;


Job_Factory::Job_Factory()
{
  //Nothing
}


Job_Factory::~Job_Factory()
{
  if(Job_Factory::p_jobs && Job_Factory::total_number_of_jobs > 0 )
  {
     free(Job_Factory::p_jobs);
     Job_Factory::total_number_of_jobs=0;
  }  
}

void Job_Factory::create_Jobs(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address)
{
   Job_Factory::total_number_of_jobs=ending_port-starting_port;
   
   Job_Factory::p_jobs = (Job *) calloc(total_number_of_jobs,sizeof(Job));

   for(int index=0; index < total_number_of_jobs; index++)
   {
      Job *p_job = (Job_Factory::p_jobs+index);

      if(p_job)
      {
         p_job->set_Job(starting_port+index,local_ipv4_address,remote_ipv4_address);
      }
   }
}

void Job_Factory::run_Job(Job *p_job_obj)
{
   uint8_t current_state = p_job_obj->return_current_job_state();
   State_Factory::run_job_on_this_current_state(current_state,p_job_obj);
}

void Job_Factory::move_Job(Job *p_job_obj)
{
   uint8_t current_state = p_job_obj->return_current_job_state();
   uint8_t next_state = State_Factory::get_next_state(current_state);
   uint8_t init_state = State_Factory::get_init_state();

   if(next_state == init_state)
   {
      printf("%s:%s:%d  Reached one loop of iteration..\n",__FILE__,__FUNCTION__,__LINE__);
      p_job_obj->increment_iteration_count();
   }
   
   p_job_obj->set_current_job_state(next_state);
   printf("%s:%s:%d  Setting next state \
   to=%d..\n",__FILE__,__FUNCTION__,__LINE__,next_state);
   //Enqueue it to the next queue.
   Queue_Factory::Instance()->enqueue(next_state,(void *)p_job_obj,sizeof(Job));
}

Job_Factory* Job_Factory::Instance()
{       
  if(!m_pInstance)
  {
     m_pInstance = new Job_Factory();
  }
  
  return m_pInstance;
}

