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
#include "one_second_timer.h"

//extern int is_webclient_alive();

webclient::Thread_Factory* webclient::Thread_Factory::p_thread_factory = NULL;

/**
 * Instance Returns the thread factory instance.
 */
webclient::Thread_Factory *webclient::Thread_Factory::Instance() {
    if (!webclient::Thread_Factory::p_thread_factory) {
        webclient::Thread_Factory::p_thread_factory = new webclient::Thread_Factory();
    }
    return webclient::Thread_Factory::p_thread_factory;
}

/**
 * Thread_Factory Constructor. Initializes thread_var.
 */
webclient::Thread_Factory::Thread_Factory() {
    thread_var = {};
}

/**
 * Thread_Factory Destructor. Cleans up thread_var.
 */
webclient::Thread_Factory::~Thread_Factory() {
    uint8_t total_number_of_mutex = 0;

    total_number_of_mutex = webclient::State_Factory::get_total_number_of_states();

    for (int index = 0; index < total_number_of_mutex; index++) {
        Pthread_variables *pthread_var = webclient::Thread_Factory::thread_var.at(index);

        if (pthread_var) {
            for (int vector_index = 0; vector_index < pthread_var->total_number_of_pthreads; vector_index++) {
                pthread_var->thread_array_var.erase(
                        pthread_var->thread_array_var.begin() + vector_index);
                pthread_var->current_job_count.erase(
                        pthread_var->current_job_count.begin() + vector_index);
            }
        }
        webclient::Thread_Factory::thread_var.erase(index);
    }
}

#define THREAD_PROCESSING_COUNT "thread-processing-count-"

/**
 * thread_main_job This is the main thread processing logic.
 * As long as this application is alive, invoke the job scheduler to perform a job.
 * For the first time, register a call back function with the one second timer to provide statistical data.
 */
void *webclient::Thread_Factory::thread_main_job(void *arg) {
    uint64_t thread_data = -1;
    pthread_t tid = 0;
    uint8_t thread_id = -1;
    string thread_name;

    if (!arg) {
        LOG_ERROR("%s:%d Argument is NULL.Returning...\n", __FUNCTION__, __LINE__);
        return 0;
    }

    tid = pthread_self();

    thread_data = (uint64_t) arg;

    thread_id = (uint8_t) thread_data;


    thread_name = webclient::State_Factory::convert_state_to_name(thread_id);

    LOG_NOTICE("%s:%d tid=%ld,thread_id=%d,thread_name=%s\n",
            __FUNCTION__, __LINE__, tid, thread_id, thread_name.c_str());

    pthread_setname_np(pthread_self(),
            thread_name.c_str());

    //Register for one second timer event callback.
    std::string str_count(THREAD_PROCESSING_COUNT);
    str_count.append(thread_name);

    one_second_timer_factory::Instance()->register_for_one_second_timer(
            str_count,
            webclient::Thread_Factory::Instance()->return_current_thread_count);

    while (webclient::is_webclient_alive()) {
        webclient::Thread_Factory::Pthread_variables *p_thread_var =
                webclient::Thread_Factory::Instance()->thread_var[thread_id];

        webclient::Scheduler_Factory::Perform_a_Job(thread_id);

        if (p_thread_var) {
            //p_thread_var->current_job_count[thread_id]+=1;
        }
    }

    LOG_DEBUG("%s thread exiting", thread_name.c_str());

    return NULL;
}

/**
 * return_current_thread_count This gets called by the one second timer.
 * Provide meaningful statistical data about how this thread is currently performing.
 */
long webclient::Thread_Factory::return_current_thread_count(void *arg) {
    std::string *p_thread_name = (std::string *) arg;
    long return_value = -1;

    if (!arg) {
        LOG_ERROR("%s:%d Invalid input parameters.\n", __FILE__, __LINE__);
        return return_value;
    }

    uint8_t match_found = 0;
    int index = 0;

    for (index = 0;
            index < webclient::State_Factory::get_total_number_of_states();
            index++) {
        std::string str_count(THREAD_PROCESSING_COUNT);
        str_count.append(webclient::State_Factory::convert_state_to_name(index));

        if (p_thread_name->compare(str_count) == 0) {
            match_found = 1;
            break;
        }
    }

    if (match_found) {
        webclient::Thread_Factory::Pthread_variables *p_thread_var =
                webclient::Thread_Factory::Instance()->thread_var[index];

        if (p_thread_var) {
            //return_value = p_thread_var->current_job_count[index];
            //p_thread_var->current_job_count[index] = 0;
        }
    }

    return return_value;
}

/**
 * Create_Thread This is a helper function to create a thread for the specified state.
 * If there is at least one thread already spawned in the current state,
 * then use the vector to append the new thread identifier and bump up the
 * current thread count by one.
 */
void webclient::Thread_Factory::Create_Thread(uint8_t index) {
    pthread_attr_t attr;
    uint64_t *p_l_thread_identifer = (uint64_t *) & index;
    //long running_count = 0;
    pthread_t *pthread_ptr = (pthread_t *) calloc(1, sizeof (pthread_t));
    Pthread_variables *pthread_var = NULL;

    pthread_var = webclient::Thread_Factory::thread_var[index];

    if (!pthread_var) {
        pthread_var = (Pthread_variables *) calloc(1, sizeof (Pthread_variables));
    }

    pthread_var->thread_array_var.push_back(pthread_ptr);
    //pthread_var->current_job_count.push_back(running_count);
    pthread_var->total_number_of_pthreads++;

    LOG_ERROR("%s:%d For state(%d),Incrementing total number of pthreads to %d\n",
            __FUNCTION__, __LINE__,
            index,
            pthread_var->total_number_of_pthreads);

    webclient::Thread_Factory::thread_var[index] = pthread_var;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(
            pthread_ptr,
            &attr,
            &webclient::Thread_Factory::thread_main_job,
            (void *) *p_l_thread_identifer);
}

/**
 * Initialize_Thread_Factory Create one thread for each state.
 */
void webclient::Thread_Factory::Initialize_Thread_Factory() {
    uint8_t total_number_of_threads = 0;

    total_number_of_threads = webclient::State_Factory::get_total_number_of_states();

    if (total_number_of_threads < 0) {
        LOG_ERROR("\nInvalid number of states");
        return;
    }

    for (uint8_t index = 0; index < total_number_of_threads; index++) {
        Create_Thread(index);
    }
}

/**
 * Add_more_threads This is a helper function to add more threads dynamically based upon the
 * total number of jobs in the current thread queue.
 */
void webclient::Thread_Factory::Add_more_threads(uint8_t thread_identifier) {
    Create_Thread(thread_identifier);
}

/**
 * Delete_threads This is a helper function to delete the threads.
 */
void webclient::Thread_Factory::Delete_threads(uint8_t thread_identifier) {

}

/**
 * return_total_number_of_threads_per_queue This function returns the total number of threads spawned
 * per queue id.
 */
int webclient::Thread_Factory::return_total_number_of_threads_per_queue(uint8_t thread_identifier) {
    Pthread_variables *pthread_var = NULL;
    pthread_var = webclient::Thread_Factory::thread_var[thread_identifier];
    int total_number_of_threads_per_queue = 0;

    if (pthread_var) {
        total_number_of_threads_per_queue = pthread_var->total_number_of_pthreads;
    }

    return total_number_of_threads_per_queue;
}