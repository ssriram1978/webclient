/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   one_second_timer.h
 * Author: ssridhar
 *
 * Created on October 31, 2017, 1:06 PM
 */

#ifndef ONE_SECOND_TIMER_H
#define ONE_SECOND_TIMER_H
#include "namespace.h"

namespace webclient {   
class one_second_timer_factory {
private:
    one_second_timer_factory();
    typedef long (*call_back_fn_ptr)(void*);
    typedef std::map<std::string,call_back_fn_ptr> one_sec_timer_map;
    one_sec_timer_map timer_map;
    static one_second_timer_factory *p_one_second_timer_factory;
    
public:
    static one_second_timer_factory *Instance();
    static void* one_sec_timer(void *args);
    void one_second_timer_Factory();
    virtual ~one_second_timer_factory();
    void register_for_one_second_timer(std::string name,
        long (*fn_pointer)(void*));
    
};
}
#endif /* ONE_SECOND_TIMER_H */
