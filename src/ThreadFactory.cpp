/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreadFactory.cpp
 * Author: ssridhar
 * 
 * Created on October 31, 2017, 1:06 PM
 */
#include "ThreadFactory.h"
#include "StateFactory.h"
#include "SchedulerFactory.h"

extern int is_webclient_alive();

webclient::Thread_Factory* webclient::Thread_Factory::p_thread_factory = NULL;

webclient::Thread_Factory *webclient::Thread_Factory::Instance()
{
    if (!webclient::Thread_Factory::p_thread_factory)
    {
        webclient::Thread_Factory::p_thread_factory=new webclient::Thread_Factory();
    }
    return webclient::Thread_Factory::p_thread_factory;
}

webclient::Thread_Factory::Thread_Factory()
{
    thread_var={};
}

webclient::Thread_Factory::~Thread_Factory()
{
    uint8_t total_number_of_mutex = 0;
    
    total_number_of_mutex = webclient::State_Factory::get_total_number_of_states();
    
    for(int index=0; index < total_number_of_mutex; index++)
    {
        Pthread_variables *pthread_var = webclient::Thread_Factory::thread_var.at(index);
        
        if(pthread_var)
        {
            for(int vector_index=0;vector_index < pthread_var->total_number_of_pthreads; vector_index++)
            {
                pthread_var->thread_array_var.erase(
                pthread_var->thread_array_var.begin()+vector_index);
            }         
        }
        webclient::Thread_Factory::thread_var.erase(index);
    }
}

void *webclient::Thread_Factory::thread_main_job(void *arg)
{
    uint64_t thread_data = -1;
    pthread_t tid =0;
    uint8_t thread_id = -1;
    string thread_name;
     
    if(!arg)
    {
       VLOG_ERROR("%s:%d Argument is NULL.Returning...\n",__FUNCTION__,__LINE__);
       return 0;
    }

   tid = pthread_self();

   thread_data = (uint64_t) arg;

   thread_id = (uint8_t) thread_data;
   
   
   thread_name = webclient::State_Factory::convert_state_to_name(thread_id);
   
   VLOG_NOTICE("%s:%d tid=%ld,thread_id=%d,thread_name=%s\n",
           __FUNCTION__,__LINE__,tid,thread_id,thread_name.c_str());
   
   pthread_setname_np(pthread_self(),
           thread_name.c_str());
   
   
   while(is_webclient_alive())
   {
        webclient::Scheduler_Factory::Perform_a_Job(thread_id);
   }
   
   VLOG_DEBUG("%s thread exiting",thread_name.c_str());
   
   return NULL;
}

void webclient::Thread_Factory::Initialize_Thread_Factory()
{
    uint8_t total_number_of_threads = 0;
    
    total_number_of_threads = webclient::State_Factory::get_total_number_of_states();
            
    if(total_number_of_threads < 0)
    {
        VLOG_ERROR("\nInvalid number of states");
        return;
    }
    
    for(uint8_t index=0; index < total_number_of_threads; index++)
    {
       pthread_attr_t attr;
       uint64_t *p_l_thread_identifer = (uint64_t *) &index;
       Pthread_variables *pthread_var  = (Pthread_variables *)calloc(1,sizeof(Pthread_variables)); 
       pthread_t *pthread_ptr = (pthread_t *)calloc(1,sizeof(pthread_t));
       pthread_var->thread_array_var.push_back(pthread_ptr);
       pthread_var->total_number_of_pthreads++;
       VLOG_DEBUG("%s:%d For state(%d),Incrementing total number of pthreads to %d\n",
               __FUNCTION__,__LINE__,
               index,
               pthread_var->total_number_of_pthreads);
       
       webclient::Thread_Factory::thread_var[index]=pthread_var;
       
       pthread_attr_init(&attr);
       pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

       pthread_create(
               pthread_ptr, 
               &attr, 
               &webclient::Thread_Factory::thread_main_job, 
               (void *)*p_l_thread_identifer);
    }
}

void webclient::Thread_Factory::Add_more_threads(uint8_t thread_identifier)
{
    
}

void webclient::Thread_Factory::Delete_threads(uint8_t thread_identifier)
{
    
}