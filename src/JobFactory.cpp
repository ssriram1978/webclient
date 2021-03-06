/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   JobFactory.cpp
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#include "JobFactory.h"
#include "StateFactory.h"
#include "QueueFactory.h"

webclient::Job_Factory* webclient::Job_Factory::m_pInstance = NULL;

uint16_t webclient::Job_Factory::total_number_of_jobs = 0;


webclient::Job* webclient::Job_Factory::p_jobs = NULL;

/**
 * Job_Factory Constructor.
 */
webclient::Job_Factory::Job_Factory() {
    //Nothing
}

/**
 * ~Job_Factory Destructor. Clean up all the jobs created in the job factory.
 */
webclient::Job_Factory::~Job_Factory() {
    if (webclient::Job_Factory::p_jobs && webclient::Job_Factory::total_number_of_jobs > 0) {
        free(webclient::Job_Factory::p_jobs);
        webclient::Job_Factory::total_number_of_jobs = 0;
    }
}

/**
 * create_Jobs This function creates jobs based upon the specified input parameters.
 * @param starting_port Starting local port on the local host where to bind.
 * @param ending_port Ending local port on the host local where to bind.
 * @param local_ipv4_address Local IPv4 address on the local host where to bind.
 * @param remote_ipv4_address Remote IPv4 address of the remote host where to connect.
 * @param remote_address Remove host domain name.
 */
void webclient::Job_Factory::create_Jobs(
        uint16_t starting_port,
        uint16_t ending_port,
        uint32_t local_ipv4_address,
        uint32_t remote_ipv4_address,
        char remote_address[]) {
    webclient::Job_Factory::total_number_of_jobs = ending_port - starting_port + 1;

    webclient::Job_Factory::p_jobs = (webclient::Job *) calloc(total_number_of_jobs, sizeof (webclient::Job));

    for (int index = 0; index < total_number_of_jobs; index++) {
        webclient::Job *p_job = (webclient::Job_Factory::p_jobs + index);

        if (p_job) {
            p_job->set_Job(starting_port + index,
                    local_ipv4_address,
                    remote_ipv4_address,
                    remote_address);
        }
    }
}

/**
 * Enqueue_All_Jobs_to_specified_queue Invokes enqueue on the specified queue.
 * @param p_queue_id
 */
void webclient::Job_Factory::Enqueue_All_Jobs_to_specified_queue(void *p_queue_id) {
    uint64_t thread_data = (uint64_t) p_queue_id;
    uint8_t queue_id = (uint8_t) thread_data;

    if (queue_id < 0 || queue_id > webclient::State_Factory::get_total_number_of_states()) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    for (int index = 0; index < total_number_of_jobs; index++) {
        webclient::Job *p_job = (webclient::Job_Factory::p_jobs + index);

        if (p_job) {
            LOG_DEBUG("\n%s:%d queue_id=%d\n", __FUNCTION__, __LINE__,
                    queue_id);
            webclient::Queue_Factory::Instance()->enqueue(queue_id,
                    (void *) p_job, sizeof (p_job));
        }
    }
}

/**
 * run_Job It invokes the state factory to run a job on this current state.
 * @param p_job_obj
 * @return
 */
int webclient::Job_Factory::run_Job(webclient::Job *p_job_obj) {
    uint8_t current_state = p_job_obj->return_current_job_state();
    return webclient::State_Factory::run_job_on_this_current_state(current_state, p_job_obj);
}

/**
 * move_Job It changes the current state to the next state and enqueues the job to the next state.
 * @param p_job
 */
void webclient::Job_Factory::move_Job(void *p_job) {
    Job *p_job_obj = (Job *) p_job;

    if (!p_job_obj) {
        LOG_ERROR("%s:%d  Input parameter(p_job) is invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    uint8_t current_state = p_job_obj->return_current_job_state();
    uint8_t next_state = webclient::State_Factory::get_next_state(current_state);
    uint8_t init_state = webclient::State_Factory::get_init_state();

    if (next_state == init_state) {
        LOG_DEBUG("%s:%s:%d  Reached one loop of iteration..\n", __FILE__, __FUNCTION__, __LINE__);
        p_job_obj->increment_iteration_count();
    }

    p_job_obj->set_current_job_state(next_state);
    LOG_DEBUG("%s:%s:%d  Setting next state \
   to=%d..\n", __FILE__, __FUNCTION__, __LINE__,
            p_job_obj->return_current_job_state());

    //Enqueue it to the next queue.
    webclient::Queue_Factory::Instance()->enqueue(p_job_obj->return_current_job_state(),
            (void *) p_job_obj, sizeof (p_job_obj));
}

/**
 * Instance It returns the job factory instance.
 * @return
 */
webclient::Job_Factory* webclient::Job_Factory::Instance() {
    if (!m_pInstance) {
        m_pInstance = new webclient::Job_Factory();
    }

    return m_pInstance;
}

