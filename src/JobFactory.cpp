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
#include "QueueFactory.h"
#include "Executor.h"

extern LogLevel gCommonLogLevel;

pipeline_framework::Job_Factory* pipeline_framework::Job_Factory::m_pInstance = NULL;
pipeline_framework::Executor* pipeline_framework::Job_Factory::p_Executor = NULL;

/**
 * Instance It returns the job factory instance.
 * @return
 */
pipeline_framework::Job_Factory* pipeline_framework::Job_Factory::Instance() {
    if (!m_pInstance) {
        m_pInstance = new pipeline_framework::Job_Factory();
    }

    return m_pInstance;
}

uint64_t pipeline_framework::Job_Factory::get_total_number_of_Jobs() {
    return p_Executor->get_total_number_of_jobs();
}

/**
 * Job_Factory Constructor.
 */
pipeline_framework::Job_Factory::Job_Factory() {
    p_Executor = new Executor();
}

/**
 * ~Job_Factory Destructor. Clean up all the jobs created in the job factory.
 */
pipeline_framework::Job_Factory::~Job_Factory() {
    delete p_Executor;
}

uint8_t pipeline_framework::Job_Factory::get_total_number_of_states() {
    return p_Executor->get_max_state();
}

std::string pipeline_framework::Job_Factory::convert_state_to_name(uint8_t state_val) {
    return p_Executor->convert_state_to_name(state_val);
}

uint8_t pipeline_framework::Job_Factory::get_init_state() {
    return p_Executor->get_init_state();
}

uint8_t pipeline_framework::Job_Factory::get_next_state(uint8_t state) {
    return p_Executor->get_next_state(state);
}

/**
 * create_Jobs This function creates jobs based upon the specified input parameters.
 * @param starting_port Starting local port on the local host where to bind.
 * @param ending_port Ending local port on the host local where to bind.
 * @param local_ipv4_address Local IPv4 address on the local host where to bind.
 * @param remote_ipv4_address Remote IPv4 address of the remote host where to connect.
 * @param remote_address Remove host domain name.
 */
void pipeline_framework::Job_Factory::create_Jobs(int argc, char **argv) {
    p_Executor->initialize(argc, argv);
}

/**
 * Enqueue_All_Jobs_to_specified_queue Invokes enqueue on the specified queue.
 * @param p_queue_id
 */
void pipeline_framework::Job_Factory::Enqueue_All_Jobs_to_specified_queue(void *p_queue_id) {
    uint64_t thread_data = (uint64_t) p_queue_id;
    uint8_t queue_id = (uint8_t) thread_data;

    if (queue_id < p_Executor->get_init_state() ||
            queue_id > p_Executor->get_max_state()) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    std::vector<uint64_t> job_identifiers;

    p_Executor->get_all_jobs(job_identifiers);

    while (!job_identifiers.empty()) {
        uint64_t job_id = job_identifiers.back();
        if (job_id > 0) {
            LOG_DEBUG("\nqueue_id=%d,job_id=%ld\n", queue_id, job_id);
            pipeline_framework::Queue_Factory::Instance()->enqueue(queue_id,
                    (void *) job_id, sizeof (uint64_t));
        }
        job_identifiers.pop_back();
    }
}

void pipeline_framework::Job_Factory::move_current_job_to_init_state(void *p_job_id) {
    uint64_t job_id = (uint64_t) p_job_id;

    if (job_id <= 0) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    p_Executor->set_current_job_state(job_id, p_Executor->get_init_state());

    LOG_DEBUG("\nmove job_id=%ld to init(%d) state.\n", job_id,
            p_Executor->get_init_state());

    pipeline_framework::Queue_Factory::Instance()->enqueue(p_Executor->get_init_state(),
            (void *) job_id, sizeof (uint64_t));
}

/**
 * run_Job It invokes the state factory to run a job on this current state.
 * @param p_job_obj
 * @return
 */
int pipeline_framework::Job_Factory::run_Job(uint64_t job_id) {
    return p_Executor->process_job(job_id);
}

/**
 * move_Job It changes the current state to the next state and enqueues the job to the next state.
 * @param p_job
 */
void pipeline_framework::Job_Factory::move_Job(void *p_job_id) {
    uint64_t job_id = (uint64_t) p_job_id;

    if (job_id <= 0) {
        LOG_ERROR("Input parameter(job_id=%p) is invalid.\n", p_job_id);
        return;
    }
    uint8_t current_state = p_Executor->get_current_state(job_id);
    uint8_t next_state = p_Executor->get_next_state(current_state);
    uint8_t init_state = p_Executor->get_init_state();

    if (next_state == init_state) {
        LOG_DEBUG("Reached one loop of iteration..\n");
        p_Executor->increment_iteration_count(job_id);
    }

    p_Executor->set_current_job_state(job_id, next_state);

    LOG_DEBUG("Setting next state "
            "to=%d.Enqueuing the job to the next state.\n",
            p_Executor->get_current_state(job_id));

    //Enqueue it to the next queue.
    pipeline_framework::Queue_Factory::Instance()->enqueue(
            p_Executor->get_current_state(job_id),
            (void *) job_id,
            sizeof (uint64_t));
}

