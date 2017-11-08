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

//extern uint8_t is_webclient_alive();

pipeline_framework::one_second_timer_factory* pipeline_framework::one_second_timer_factory::p_one_second_timer_factory = NULL;

/**
 *Instance Provides the one second timer factory instance.
 * @return
 */
pipeline_framework::one_second_timer_factory *pipeline_framework::one_second_timer_factory::Instance() {
    if (!pipeline_framework::one_second_timer_factory::p_one_second_timer_factory) {
        pipeline_framework::one_second_timer_factory::p_one_second_timer_factory = new pipeline_framework::one_second_timer_factory();
    }
    return pipeline_framework::one_second_timer_factory::p_one_second_timer_factory;
}

/**
 *one_sec_timer This is the main thread processing callback.
 * In this fn, it iterates over the map containing the mapping of module name and
 * the corresponding call back function and invokes them one by one.
 * @param args
 * @return
 */
void* pipeline_framework::one_second_timer_factory::one_sec_timer(void *args) {
    pthread_setname_np(pthread_self(),
            "ONE_SEC_TIMER_THREAD");

    pipeline_framework::one_second_timer_factory::Instance()->print_stats = 0;

    std::map<std::string, call_back_fn_ptr>::iterator iterator;

    sleep(5);

    while (pipeline_framework::is_webclient_alive()) {
        for (iterator = pipeline_framework::one_second_timer_factory::Instance()->timer_map.begin();
                iterator != pipeline_framework::one_second_timer_factory::Instance()->timer_map.end();
                ++iterator) {
            std::string name = iterator->first;
            call_back_fn_ptr fn_ptr = iterator->second;

            if (fn_ptr) {
                long return_value = (*fn_ptr)((void *) &name);

                if (pipeline_framework::one_second_timer_factory::Instance()->print_stats) {
                    printf("Invoking %s callback returned %ld\n",
                            name.c_str(), return_value);
                }
            } else {
                LOG_ERROR("fn_ptr is NULL for %s.\n", name.c_str());
            }
        }
        sleep(1);
    }

    LOG_DEBUG("ONE_SEC_TIMER_THREAD thread exiting");

    return NULL;
}

/**
 *one_second_timer_factory Constructor. It spawns the one second timer pthread.
 */
pipeline_framework::one_second_timer_factory::one_second_timer_factory() {
    timer_map = {};
    //start a one second timer thread.
    pthread_attr_t attr;
    pthread_t pthread_ptr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    pthread_create(
            &pthread_ptr,
            &attr,
            &pipeline_framework::one_second_timer_factory::one_sec_timer,
            NULL);

}

/**
 *~one_second_timer_factory Destructor.
 */
pipeline_framework::one_second_timer_factory::~one_second_timer_factory() {

}

/**
 *register_for_one_second_timer Whoever wants to register their call back functions
 * to provide statistics to this one second timer may use this function to register
 * their name of the variable and the corresponding call back function.
 * @param name
 * @param fn_pointer
 */
void pipeline_framework::one_second_timer_factory::register_for_one_second_timer(std::string name,
        long (*fn_pointer)(void*)) {
    if (name.empty() || (fn_pointer == NULL)) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }
    LOG_ERROR("Registering callback fn for name=%s.\n", name.c_str());
    timer_map[name] = fn_pointer;

}


