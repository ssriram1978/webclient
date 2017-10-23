/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MutexFactory.h
 * Author: ssridhar
 *
 * Created on October 23, 2017, 10:39 AM
 */

#ifndef MUTEX_FACTORY_H
#define MUTEX_FACTORY_H
#include <map>
#include <pthread.h>

namespace webclient 
{
class Mutex_Factory 
{
private:
    Mutex_Factory();    
    virtual ~Mutex_Factory();
    typedef std::map<uint8_t,pthread_mutex_t*> Mutex_map;
    Mutex_map mutex_var;
    typedef std::map<uint8_t,pthread_cond_t*>  Cond_map;
    Cond_map cond_var;
    static Mutex_Factory *p_mutex_factory;
    
public:
    static Mutex_Factory *Instance();
    void initialize();
    void condition_wait(uint8_t mutex_id,
        void (*p_call_back_function)(void*),
        void *p_job);
    void condition_signal(uint8_t mutex_id,
        void (*p_call_back_function)(uint8_t),
        uint8_t state_id);   
};
}

#endif /* MUTEX_FACTORY_H */

