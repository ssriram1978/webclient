/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   one_second_timer.cpp
 * Author: ssridhar
 * 
 * Created on October 31, 2017, 1:06 PM
 */

#include "one_second_timer.h"

extern uint8_t is_webclient_alive();

webclient::one_second_timer_factory* webclient::one_second_timer_factory::p_one_second_timer_factory = NULL;

webclient::one_second_timer_factory *webclient::one_second_timer_factory::Instance()
{
    if (!webclient::one_second_timer_factory::p_one_second_timer_factory)
    {
        webclient::one_second_timer_factory::p_one_second_timer_factory=new webclient::one_second_timer_factory();
    }
    return webclient::one_second_timer_factory::p_one_second_timer_factory;
}

void* webclient::one_second_timer_factory::one_sec_timer(void *args)
{
    pthread_setname_np(pthread_self(),
        "ONE_SEC_TIMER_THREAD");
   
    std::map<std::string,call_back_fn_ptr>::iterator iterator;
    
    while(is_webclient_alive())
    {

        for (iterator = webclient::one_second_timer_factory::Instance()->timer_map.begin(); 
             iterator != webclient::one_second_timer_factory::Instance()->timer_map.end(); 
             ++iterator)
        {
            std::string name = iterator->first;
            call_back_fn_ptr fn_ptr=iterator->second;
             
            long return_value=(*fn_ptr)((void *)&name);
             
            printf("Invoking %s callback returned %ld\n",
                    name.c_str(),return_value);
        }

        sleep(1);
    }
   
    VLOG_DEBUG("ONE_SEC_TIMER_THREAD thread exiting");
   
   return NULL;
}

webclient::one_second_timer_factory::one_second_timer_factory()
{
    timer_map={};
    //start a one second timer thread.
    pthread_attr_t attr;
    pthread_t pthread_ptr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    

    pthread_create(
        &pthread_ptr,
        &attr,
        &webclient::one_second_timer_factory::one_sec_timer,
        NULL);

}

webclient::one_second_timer_factory::~one_second_timer_factory()
{
    
}

long temp(void *args)
{
    return 1;
}

void webclient::one_second_timer_factory::register_for_one_second_timer(std::string name,
        long (*fn_pointer)(void*))
{
    if(name.empty() || (fn_pointer == NULL))
    {
        VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FUNCTION__,__LINE__);
        return;
    }
    timer_map[name]=fn_pointer;
    
}


