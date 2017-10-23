/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MutexFactory.cpp
 * Author: ssridhar
 * 
 * Created on October 23, 2017, 10:39 AM
 */

#include <map>
#include <pthread.h>
#include "MutexFactory.h"
#include "StateFactory.h"

webclient::Mutex_Factory* webclient::Mutex_Factory::p_mutex_factory = NULL;

webclient::Mutex_Factory *webclient::Mutex_Factory::Instance()
{
    if (!webclient::Mutex_Factory::p_mutex_factory)
    {
        webclient::Mutex_Factory::p_mutex_factory=new webclient::Mutex_Factory();
    }
    return webclient::Mutex_Factory::p_mutex_factory;
}

webclient::Mutex_Factory::Mutex_Factory() 
{
    webclient::Mutex_Factory::mutex_var={};
    webclient::Mutex_Factory::cond_var={};
}

webclient::Mutex_Factory::~Mutex_Factory() 
{
    uint8_t total_number_of_mutex = 0;
    
    total_number_of_mutex = webclient::State_Factory::get_total_number_of_states();
    
    for(int index=0; index < total_number_of_mutex; index++)
    {
        pthread_mutex_t *p_mutex = webclient::Mutex_Factory::mutex_var[index];
        pthread_cond_t *p_cond = webclient::Mutex_Factory::cond_var[index];
        
        if(p_mutex)
        {
            free(p_mutex);
            webclient::Mutex_Factory::mutex_var.erase(index);
        }
        
        if(p_cond)
        {
            free(p_cond);
            webclient::Mutex_Factory::cond_var.erase(index);
        }
    }
}

void webclient::Mutex_Factory::initialize() 
{
    
    uint8_t total_number_of_mutex = 0;
    
    total_number_of_mutex = webclient::State_Factory::get_total_number_of_states();
            
    if(total_number_of_mutex < 0)
    {
        printf("\nInvalid number of states");
        return;
    }
    
    for(int index=0; index < total_number_of_mutex; index++)
    {
        pthread_mutex_t *p_mutex = (pthread_mutex_t *) calloc(1,sizeof(pthread_mutex_t));
        *p_mutex = PTHREAD_MUTEX_INITIALIZER;        
        webclient::Mutex_Factory::mutex_var[index]= p_mutex;
        pthread_cond_t *p_cond = (pthread_cond_t *) calloc(1,sizeof(pthread_cond_t));
        *p_cond = PTHREAD_COND_INITIALIZER;
        webclient::Mutex_Factory::cond_var[index]= p_cond;
    }
}

void webclient::Mutex_Factory::condition_signal(uint8_t state_type,
        void (*p_call_back_function)(void*),
        void *p_job) 
{
    uint8_t status = 0;
    
    if(state_type < webclient::State_Factory::get_init_state() ||
       state_type > webclient::State_Factory::get_total_number_of_states())
    {
        printf("\nInvalid input parameter");
        return;
    }
    
    pthread_mutex_t *p_mutex = webclient::Mutex_Factory::mutex_var[state_type];
    pthread_cond_t *p_cond = webclient::Mutex_Factory::cond_var[state_type];
    
    if(!p_mutex || !p_cond)
    {
        printf("\nUnable to find mutex/cond for this state=%d\n",state_type);
        return;
    }   
    
    //wake up and dequeue this message when condition is signalled.
    status = pthread_mutex_lock(p_mutex);
    if (status != 0)
    {
        printf("[%s:%d] pthread_mutex_lock failed\n",__FILE__,__LINE__);
        return;
    }

    status = pthread_cond_signal(p_cond);
    if (status != 0)
    {
        printf("[%s:%d] pthread_cond_wait failed\n",__FILE__,__LINE__);
        return;
    }
    
    //Execute the callback function
    (*p_call_back_function)(p_job);
    
    status = pthread_mutex_unlock(p_mutex);
    if (status != 0)
    {
        printf("[%s:%d] pthread_mutex_unlock failed\n",__FILE__,__LINE__);
        return;
    }
}

void webclient::Mutex_Factory::condition_wait(uint8_t state_type,
        void (*p_call_back_function)(void*),
        void *p_job) 
{
    uint8_t status = 0;
    
    if(state_type < webclient::State_Factory::get_init_state() ||
       state_type > webclient::State_Factory::get_total_number_of_states())
    {
        printf("\nInvalid input parameter");
        return;
    }
    
    pthread_mutex_t *p_mutex = webclient::Mutex_Factory::mutex_var[state_type];
    pthread_cond_t *p_cond = webclient::Mutex_Factory::cond_var[state_type];
    
    if(!p_mutex || !p_cond)
    {
        printf("\nUnable to find mutex/cond for this state=%d\n",state_type);
        return;
    }   
    
    //wake up and dequeue this message when condition is signalled.
    status = pthread_mutex_lock(p_mutex);
    if (status != 0)
    {
        printf("[%s:%d] pthread_mutex_lock failed\n",__FILE__,__LINE__);
        return;
    }

    status = pthread_cond_wait(p_cond,p_mutex);
    if (status != 0)
    {
        printf("[%s:%d] pthread_cond_wait failed\n",__FILE__,__LINE__);
        return;
    }
    
    //Execute the callback function
    (*p_call_back_function)(p_job);
    
    status = pthread_mutex_unlock(p_mutex);
    if (status != 0)
    {
        printf("[%s:%d] pthread_mutex_unlock failed\n",__FILE__,__LINE__);
        return;
    }
}