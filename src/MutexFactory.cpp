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

#include "MutexFactory.h"
#include "JobFactory.h"

pipeline_framework::Mutex_Factory* pipeline_framework::Mutex_Factory::p_mutex_factory = NULL;

/**
 * Instance Returns the Mutex factory object.
 * @return
 */
pipeline_framework::Mutex_Factory *pipeline_framework::Mutex_Factory::Instance() {
    if (!pipeline_framework::Mutex_Factory::p_mutex_factory) {
        pipeline_framework::Mutex_Factory::p_mutex_factory = new pipeline_framework::Mutex_Factory();
    }
    return pipeline_framework::Mutex_Factory::p_mutex_factory;
}

/**
 * Mutex_Factory Constructor. Initializes mutex_var and cond_var and invokes initialize().
 */
pipeline_framework::Mutex_Factory::Mutex_Factory() {
    pipeline_framework::Mutex_Factory::mutex_var = {};
    pipeline_framework::Mutex_Factory::cond_var = {};
    //Initialize Mutex Factory.
    pipeline_framework::Mutex_Factory::initialize();
}

/**
 * ~Mutex_Factory Destructor. Cleans up mute_var and cond_var.
 */
pipeline_framework::Mutex_Factory::~Mutex_Factory() {
    uint8_t total_number_of_mutex = 0;
    total_number_of_mutex = pipeline_framework::Job_Factory::get_total_number_of_states();

    for (int index = 0; index < total_number_of_mutex; index++) {
        pthread_mutex_t *p_mutex = pipeline_framework::Mutex_Factory::mutex_var[index];
        pthread_cond_t *p_cond = pipeline_framework::Mutex_Factory::cond_var[index];

        if (p_mutex) {
            free(p_mutex);
            pipeline_framework::Mutex_Factory::mutex_var.erase(index);
        }

        if (p_cond) {
            free(p_cond);
            pipeline_framework::Mutex_Factory::cond_var.erase(index);
        }
    }
}

/**
 * initialize Allocates mutex and cond variables based upon the total number of states.
 */
void pipeline_framework::Mutex_Factory::initialize() {
    uint8_t total_number_of_mutex = 0;
    total_number_of_mutex = pipeline_framework::Job_Factory::get_total_number_of_states();

    if (total_number_of_mutex < 0) {
        LOG_ERROR("\n%s:%d  Invalid number of states(%d)\n",
                __FUNCTION__, __LINE__, total_number_of_mutex);
        return;
    }

    LOG_DEBUG("%s:%d total_number_of_mutex=%d\n",
            __FUNCTION__, __LINE__, total_number_of_mutex);

    for (int index = 0; index < total_number_of_mutex; index++) {
        pthread_mutex_t *p_mutex = (pthread_mutex_t *) calloc(1, sizeof (pthread_mutex_t));
        *p_mutex = PTHREAD_MUTEX_INITIALIZER;
        pipeline_framework::Mutex_Factory::mutex_var[index] = p_mutex;
        pthread_cond_t *p_cond = (pthread_cond_t *) calloc(1, sizeof (pthread_cond_t));
        *p_cond = PTHREAD_COND_INITIALIZER;
        pipeline_framework::Mutex_Factory::cond_var[index] = p_cond;
    }
}

/**
 * condition_signal Grabs the mutex and condition variables and invokes the passed in call back fn.
 * @param state_type
 * @param p_call_back_function
 * @param p_job
 */
void pipeline_framework::Mutex_Factory::condition_signal(uint8_t state_type,
        void (*p_call_back_function)(void *),
        void *p_job) {

    uint8_t status = 0;
    if (state_type < pipeline_framework::Job_Factory::get_init_state() ||
            state_type > pipeline_framework::Job_Factory::get_total_number_of_states()) {
        LOG_ERROR("\n%s:%d state=%d Invalid input parameter\n",
                __FUNCTION__, __LINE__, state_type);
        return;
    }

    pthread_mutex_t *p_mutex = pipeline_framework::Mutex_Factory::mutex_var[state_type];
    pthread_cond_t *p_cond = pipeline_framework::Mutex_Factory::cond_var[state_type];

    if (!p_mutex || !p_cond) {
        LOG_ERROR("\n%s:%d Unable to find mutex/cond for this state=%d\n",
                __FUNCTION__, __LINE__, state_type);
        return;
    }

    //wake up and dequeue this message when condition is signalled.
    status = pthread_mutex_lock(p_mutex);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_mutex_lock failed\n",
                __FUNCTION__, __LINE__, state_type);
        return;
    }

    if (p_call_back_function) {
        //Execute the callback function
        LOG_DEBUG("%s:%d state=%d Executing the callback function.\n",
                __FUNCTION__, __LINE__, state_type);

        (*p_call_back_function)(p_job);
    }

    LOG_DEBUG("%s:%d state=%d Invoking pthread_cond_signal()\n",
            __FUNCTION__, __LINE__, state_type);

    status = pthread_cond_signal(p_cond);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_cond_wait failed\n",
                __FUNCTION__, __LINE__, state_type);
        return;
    }

    status = pthread_mutex_unlock(p_mutex);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_mutex_unlock failed\n",
                __FUNCTION__, __LINE__, state_type);
        return;
    }
}

/**
 * condition_wait Tries to grab the mutex and waits for the condition variable to be set.
 * Once it gets access to the critical region, it executes the passed in call back fn.
 * @param state_type
 * @param p_call_back_function
 * @param p_job
 * @return
 */
void* pipeline_framework::Mutex_Factory::condition_wait(uint8_t state_type,
        void* (*p_call_back_function)(void*),
        void *p_job) {
    void *return_argument = NULL;

    uint8_t status = 0;
    if (state_type < pipeline_framework::Job_Factory::get_init_state() ||
            state_type > pipeline_framework::Job_Factory::get_total_number_of_states()) {
        LOG_ERROR("\n%s:%d state=%d Invalid input parameter\n",
                __FUNCTION__, __LINE__, state_type);
        return return_argument;
    }

    pthread_mutex_t *p_mutex = pipeline_framework::Mutex_Factory::mutex_var[state_type];
    pthread_cond_t *p_cond = pipeline_framework::Mutex_Factory::cond_var[state_type];

    if (!p_mutex || !p_cond) {
        LOG_ERROR("\n%s:%d state=%d Unable to find mutex/cond.\n",
                __FUNCTION__, __LINE__, state_type);
        return return_argument;
    }

    //wake up and dequeue this message when condition is signalled.
    status = pthread_mutex_lock(p_mutex);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_mutex_lock failed\n",
                __FUNCTION__, __LINE__, state_type);
        return return_argument;
    }

    LOG_DEBUG("%s:%d state=%d Invoking pthread_cond_wait()\n",
            __FUNCTION__, __LINE__, state_type);

    status = pthread_cond_wait(p_cond, p_mutex);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_cond_wait failed\n",
                __FUNCTION__, __LINE__, state_type);
        return return_argument;
    }

    //Execute the callback function
    LOG_DEBUG("%s:%d state=%d Executing the callback function.\n",
            __FUNCTION__, __LINE__, state_type);

    return_argument = (*p_call_back_function)(p_job);

    status = pthread_mutex_unlock(p_mutex);
    if (status != 0) {
        LOG_ERROR("%s:%d state=%d pthread_mutex_unlock failed\n",
                __FUNCTION__, __LINE__, state_type);
        return return_argument;
    }

    return return_argument;
}