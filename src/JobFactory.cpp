#include "JobFactory.h"
#include "StateFactory.h"
#include "QueueFactory.h"

webclient::Job_Factory* webclient::Job_Factory::m_pInstance = NULL;

uint16_t webclient::Job_Factory::total_number_of_jobs = 0;


webclient::Job* webclient::Job_Factory::p_jobs = NULL;


webclient::Job_Factory::Job_Factory()
{
  //Nothing
}


webclient::Job_Factory::~Job_Factory()
{
  if(webclient::Job_Factory::p_jobs && webclient::Job_Factory::total_number_of_jobs > 0 )
  {
     free(webclient::Job_Factory::p_jobs);
     webclient::Job_Factory::total_number_of_jobs=0;
  }  
}

void webclient::Job_Factory::create_Jobs(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address)
{
   webclient::Job_Factory::total_number_of_jobs=ending_port-starting_port;
   
   webclient::Job_Factory::p_jobs = (webclient::Job *) calloc(total_number_of_jobs,sizeof(webclient::Job));

   for(int index=0; index < total_number_of_jobs; index++)
   {
      webclient::Job *p_job = (webclient::Job_Factory::p_jobs+index);

      if(p_job)
      {
         p_job->set_Job(starting_port+index,local_ipv4_address,remote_ipv4_address);
      }
   }
}

void webclient::Job_Factory::Enqueue_All_Jobs_to_specified_queue(uint8_t queue_id)
{
    if(queue_id < 0 || queue_id > webclient::State_Factory::get_total_number_of_states())
    {
        printf("%s:%d  Input parameters are invalid.\n",__FUNCTION__,__LINE__);
        return;
    }
    
    for(int index=0; index < total_number_of_jobs; index++)
    {
           webclient::Job *p_job = (webclient::Job_Factory::p_jobs+index);

           if(p_job)
           {
               printf("%s:%d enqueue to queue_id=%d",__FUNCTION__,__LINE__,
                       queue_id);
              webclient::Queue_Factory::Instance()->enqueue(queue_id,
                      (void *)p_job,sizeof(webclient::Job));
           } 
    }
}

void webclient::Job_Factory::run_Job(webclient::Job *p_job_obj)
{
   uint8_t current_state = p_job_obj->return_current_job_state();
   webclient::State_Factory::run_job_on_this_current_state(current_state,p_job_obj);
}

void webclient::Job_Factory::move_Job(Job *p_job_obj)
{
   uint8_t current_state = p_job_obj->return_current_job_state();
   uint8_t next_state = webclient::State_Factory::get_next_state(current_state);
   uint8_t init_state = webclient::State_Factory::get_init_state();

   if(next_state == init_state)
   {
      printf("%s:%s:%d  Reached one loop of iteration..\n",__FILE__,__FUNCTION__,__LINE__);
      p_job_obj->increment_iteration_count();
   }
   
   p_job_obj->set_current_job_state(next_state);
   printf("%s:%s:%d  Setting next state \
   to=%d..\n",__FILE__,__FUNCTION__,__LINE__,next_state);
   //Enqueue it to the next queue.
   webclient::Queue_Factory::Instance()->enqueue(next_state,(void *)p_job_obj,sizeof(Job));
}

webclient::Job_Factory* webclient::Job_Factory::Instance()
{       
  if(!m_pInstance)
  {
     m_pInstance = new webclient::Job_Factory();
  }
  
  return m_pInstance;
}

