/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ThreadFactory.h
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */
#ifndef THREAD_FACTORY_H
#define THREAD_FACTORY_H

#include "namespace.h"

namespace webclient {   
    class Thread_Factory {        
    private:
        typedef std::vector<pthread_t*> Pthread_array;
        
        typedef struct __pthread_variables
        {
            uint8_t total_number_of_pthreads;            
            Pthread_array thread_array_var;
        }Pthread_variables;
        
        typedef std::map<uint8_t,Pthread_variables*> Thread_map;
        Thread_map thread_var;
                
        static void* thread_main_job(void *arg);
        static Thread_Factory *p_thread_factory;
        
        Thread_Factory();
    
    public:
        static Thread_Factory *Instance();
        void Initialize_Thread_Factory();
        void Add_more_threads(uint8_t thread_identifier);
        void Delete_threads(uint8_t thread_identifier);
        virtual ~Thread_Factory();
    };
}

#endif