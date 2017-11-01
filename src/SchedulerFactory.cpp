/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchedulerFactory.cpp
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */
#include "SchedulerFactory.h"
#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"
#include "MutexFactory.h"
#include "one_second_timer.h"

webclient::Scheduler_Factory* webclient::Scheduler_Factory::m_pInstance = NULL;


webclient::Scheduler_Factory::Scheduler_Factory()
{
  //Nothing
}


webclient::Scheduler_Factory::~Scheduler_Factory()
{
  //Nothing
}

webclient::Scheduler_Factory* webclient::Scheduler_Factory::Instance()
{       
    if(!m_pInstance)
    {
       m_pInstance = new webclient::Scheduler_Factory();
    }
    
    return m_pInstance;
}

#define SCHEDULER_JOB "SCHEDULER_JOB-"

void webclient::Scheduler_Factory::initialize(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address,
    char remote_address[])
{

   //State Factory is already initialized. (All static functions)
   VLOG_DEBUG("%s:%s:%d State_Factory::Instance()->get_total_number_of_states() returned %d\n",
           __FILE__,__FUNCTION__,__LINE__,
           webclient::State_Factory::get_total_number_of_states());

   //Initialize Queue Factory
   webclient::Queue_Factory::Instance()->set_total_number_of_queues(
   webclient::State_Factory::get_total_number_of_states());

   //VLOG_DEBUG("%s:%s:%d queue::Instance()->is_empty(1) returned
   //%d\n",__FILE__,__FUNCTION__,__LINE__,
   //       webclient::Queue_Factory::Instance()->is_empty(1)); 

      //Initialize Job Factory
   webclient::Job_Factory::Instance()->create_Jobs(starting_port,
                                                   ending_port,
                                                   local_ipv4_address,
                                                   remote_ipv4_address,
                                                   remote_address);

   //Register for one second timer event callback.
   for (int index=0; 
           index < webclient::State_Factory::get_total_number_of_states(); 
           index++)
   {
        long count=0;
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));
        enqueue_job_count.push_back(count);
        one_second_timer_factory::Instance()->register_for_one_second_timer(
        str_count,
        webclient::Scheduler_Factory::Instance()->return_current_dequeue_done_count);   

        std::string str_count2(SCHEDULER_JOB);
        str_count2.append("enqueue_count-");
        str_count2.append(webclient::State_Factory::convert_state_to_name(index));
        dequeue_job_count.push_back(count);
        
        one_second_timer_factory::Instance()->register_for_one_second_timer(
        str_count2,
        webclient::Scheduler_Factory::Instance()->return_current_enqueue_done_count);   
   }

   
#if 0  
   webclient::Queue_Factory::Instance()->enqueue(1,(void *)"sriram",strlen("sriram"));

   
   printf("%s:%s:%d queue::Instance()->is_empty(1) returned \
   %d\n",__FILE__,__FUNCTION__,__LINE__,
        webclient::Queue_Factory::Instance()->is_empty(1));

   char *pmsg = (char *) calloc(1,100);
   uint32_t length = 0;
   webclient::Queue_Factory::Instance()->dequeue(1,(void **)&pmsg,&length);

   printf("%s:%s:%d pmsg=%s,length=%d\n",__FILE__,__FUNCTION__,__LINE__,
           pmsg,length);
   
   printf("%s:%s:%d queue::Instance()->is_empty() returned %d\n",
           __FILE__,__FUNCTION__,__LINE__,
           webclient::Queue_Factory::Instance()->is_empty(1));
   
   webclient::Job_Factory::Instance()->create_Jobs(1,64000,123456,123456);
   webclient::Job job_obj;
   job_obj.set_Job(1,123456,123456);
   printf("Job_Factory::Instance()->run_Job()\n");
   webclient::Job_Factory::Instance()->run_Job(&job_obj);
   printf("Job_Factory::Instance()->move_Job()\n");
   webclient::Job_Factory::Instance()->move_Job(&job_obj);
   
   for(int index=0; index < webclient::State_Factory::get_total_number_of_states(); index++)
   {
       webclient::State_Factory::run_job_on_this_current_state(webclient::State_Factory::get_next_state(index),NULL);
   }
#endif   
   
   //Initialize Thread Factory   
}

long webclient::Scheduler_Factory::return_current_dequeue_done_count(void *arg)
{
    std::string *p_thread_name=(std::string *) arg;
    long return_value = -1;
    
    if(!arg)
    {
        VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
        return return_value;
    }
    
    uint8_t match_found = 0;
    int index=0;
    
    for(index=0; 
            index < webclient::State_Factory::get_total_number_of_states();
            index++)
    {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));
        
        if (p_thread_name->compare(str_count) == 0)
        {
            match_found=1;
            break;
        }
    }
    
    if(match_found)
    {
#if 0
        webclient::Thread_Factory::Pthread_variables *p_thread_var = 
                webclient::Thread_Factory::Instance()->thread_var[index];
              
        if(p_thread_var)
        {
           return_value = p_thread_var->current_job_count[index];
           p_thread_var->current_job_count[index] = 0;
        }
#endif      
        return_value = webclient::Scheduler_Factory::Instance()->dequeue_job_count[index];
        webclient::Scheduler_Factory::Instance()->dequeue_job_count[index]=0;
    }
    
    return return_value;
}

long webclient::Scheduler_Factory::return_current_enqueue_done_count(void *arg)
{
    std::string *p_thread_name=(std::string *) arg;
    long return_value = -1;
    
    if(!arg)
    {
        VLOG_ERROR("%s:%d Invalid input parameters.\n",__FILE__,__LINE__);
        return return_value;
    }
    
    uint8_t match_found = 0;
    int index=0;
    
    for(index=0; 
            index < webclient::State_Factory::get_total_number_of_states();
            index++)
    {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("enqueue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));
        
        if (p_thread_name->compare(str_count) == 0)
        {
            match_found=1;
            break;
        }
    }
    
    if(match_found)
    {
#if 0        
        webclient::Thread_Factory::Pthread_variables *p_thread_var = 
                webclient::Thread_Factory::Instance()->thread_var[index];
              
        if(p_thread_var)
        {
           return_value = p_thread_var->current_job_count[index];
           p_thread_var->current_job_count[index] = 0;
        }
#endif
        return_value = webclient::Scheduler_Factory::Instance()->enqueue_job_count[index];
        webclient::Scheduler_Factory::Instance()->enqueue_job_count[index]=0;
    }
    
    return return_value;
}

void webclient::Scheduler_Factory::run()
{
    uint8_t state = webclient::State_Factory::get_init_state();
    uint64_t *ptr_to_state = (uint64_t *) &state;    
    
    VLOG_DEBUG("\n%s:%s:%d Going to enqueue all jobs to the first state (%d)\n",
            __FILE__,__FUNCTION__,__LINE__,
           state);
    
    webclient::Mutex_Factory::Instance()->condition_signal(
                webclient::State_Factory::get_init_state(),
                webclient::Job_Factory::Instance()->Enqueue_All_Jobs_to_specified_queue,
                (void *)*ptr_to_state);
    
   //Enqueue all the Jobs to the queue listed on the first state.
   //webclient::Job_Factory::Instance()->Enqueue_All_Jobs_to_specified_queue(
   //webclient::State_Factory::get_init_state());
}

void webclient::Scheduler_Factory::Perform_a_Job(uint8_t state_id)
{
    uint64_t *ptr_to_state = (uint64_t *) &state_id;
    
    VLOG_DEBUG("\n%s:%s:%d Before invoking condition_wait(). state=(%d)\n",
            __FILE__,__FUNCTION__,__LINE__,
           state_id);
    webclient::Job *p_job = (webclient::Job *) webclient::Mutex_Factory::Instance()->condition_wait(
                state_id,
                webclient::Scheduler_Factory::Dequeue_Job,
                (void *)*ptr_to_state);    
    if(p_job)
    {
        VLOG_DEBUG("\n%s:%s:%d Found a job. state=(%d)\n",
            __FILE__,__FUNCTION__,__LINE__,
           state_id);
        webclient::Scheduler_Factory::Instance()->dequeue_job_count[state_id]+=1;
        int return_code = Scheduler_Factory::Execute_Job(p_job);
        if(return_code == SUCCESS)
        {
            Scheduler_Factory::Move_Job(p_job);
        }
        else
        {
            //Move all jobs to init state.
            //Scheduler_Factory::run();
        }
        
    }
    else
    {
        VLOG_DEBUG("\n%s:%s:%d p_job is NULL. state=(%d)\n",
            __FILE__,__FUNCTION__,__LINE__,
           state_id);
    }
}

void* webclient::Scheduler_Factory::Dequeue_Job(void *p_state)
{
    webclient::Job *p_job = NULL;
    uint8_t state_id = 0;
    uint64_t arg = (uint64_t) p_state;
    state_id = (uint8_t) arg;
    
    if(state_id < webclient::State_Factory::get_init_state() ||
       state_id > webclient::State_Factory::get_total_number_of_states())
    {
        VLOG_ERROR("\n%s:%d Invalid input parameter\n",
                __FUNCTION__,__LINE__);
        return p_job;
    }
    
    VLOG_DEBUG("%s:%s:%d state=%d,is_empty=%d\n",__FILE__,__FUNCTION__,__LINE__,
           state_id,webclient::Queue_Factory::Instance()->is_empty(state_id));
    
    if(!webclient::Queue_Factory::Instance()->is_empty(state_id))
    {
        //p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
        p_job = NULL;
        webclient::uint32_t length = 0;
        
        webclient::Queue_Factory::Instance()->dequeue(state_id,
                (void **)&p_job,
                &length);

        VLOG_DEBUG("%s:%s:%d p_job=%p,length=%d\n",__FILE__,__FUNCTION__,__LINE__,
           p_job,length);
    }
    else
    {
        VLOG_DEBUG("%s:%s:%d state=%d. Queue is empty.\n",__FILE__,__FUNCTION__,__LINE__,
           state_id);
    }
    
    return (void *)p_job;
}

int webclient::Scheduler_Factory::Execute_Job(webclient::Job *p_job)
{
    if(!p_job)
    {
        VLOG_ERROR("%s:%s:%d p_job is NULL\n",__FILE__,__FUNCTION__,__LINE__);
        return FAILURE;
    }
    
    VLOG_DEBUG("Scheduler_Factory Job_Factory::Instance()->run_Job()\n");
    return webclient::Job_Factory::Instance()->run_Job(p_job);
}

void webclient::Scheduler_Factory::Move_Job(webclient::Job *p_job)
{
    if(!p_job)
    {
        VLOG_ERROR("%s:%s:%d p_job is NULL\n",__FILE__,__FUNCTION__,__LINE__);
        return;
    }
    
    VLOG_DEBUG("Scheduler_Factory Job_Factory::Instance()->move_Job()\n");
    
    uint8_t next_state = webclient::State_Factory::get_next_state(p_job->return_current_job_state());
    
    webclient::Mutex_Factory::Instance()->condition_signal(
        next_state,
        webclient::Job_Factory::Instance()->move_Job,
        (void *)p_job);   
    webclient::Scheduler_Factory::Instance()->enqueue_job_count[p_job->return_current_job_state()]+=1;
}

void webclient::Scheduler_Factory::stop()
{
    //Delete all the Jobs from all the queues.
}
