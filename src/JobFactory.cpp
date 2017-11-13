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

uint8_t pipeline_framework::Job_Factory::get_max_state() {
    return p_Executor->get_max_state();
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
 * run_Job It invokes the state factory to run a job on this current state.
 * @param p_job_obj
 * @return
 */
int pipeline_framework::Job_Factory::run_Job(uint64_t job_id) {
    return p_Executor->process_job(job_id);
}

/**
 * Enqueue_All_Jobs_to_specified_queue Invokes enqueue on the specified queue.
 * @param p_queue_id
 */
void pipeline_framework::Job_Factory::get_all_jobs(std::vector<uint64_t> &job_identifiers) {

    p_Executor->get_all_jobs(job_identifiers);
}

void pipeline_framework::Job_Factory::set_current_job_state(void *p_job_id,
        uint8_t state) {
    uint64_t job_id = (uint64_t) p_job_id;

    if (job_id <= 0) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    p_Executor->set_current_job_state(job_id, p_Executor->get_init_state());

}

void pipeline_framework::Job_Factory::increment_iteration_count(void *p_job_id) {
    uint64_t job_id = (uint64_t) p_job_id;

    if (job_id <= 0) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return;
    }

    p_Executor->increment_iteration_count(job_id);
}

uint64_t pipeline_framework::Job_Factory::get_iteration_count(void *p_job_id) {
    uint64_t job_id = (uint64_t) p_job_id;

    if (job_id <= 0) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return p_Executor->get_iteration_count(job_id);
}

uint8_t pipeline_framework::Job_Factory::get_current_job_state(void *p_job_id) {
    if (p_job_id <= 0) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FUNCTION__, __LINE__);
        return -1;
    }

    return p_Executor->get_current_state((uint64_t) p_job_id);
}
