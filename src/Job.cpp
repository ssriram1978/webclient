/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Job.cpp
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#include "Job.h"

/**
 * Set_Job Allows the user to set the job parameters.
 * @param local_port Local port on the host where you need to bind.
 * @param local_ipv4 Local IPv4 address on the host where you need to bind.
 * @param remote_ipv4 Remote IPv4 address where you need to connect.
 * @param remote_address Remote domain name where you need to connect.
 */
void webclient::Job::set_Job(uint16_t local_port,
        uint32_t local_ipv4,
        uint32_t remote_ipv4,
        char remote_address[]) {
    this->local_port = local_port;
    this->local_ipv4 = local_ipv4;
    this->remote_ipv4 = remote_ipv4;
    //int len_of_remote_address = sizeof(remote_address)/sizeof(remote_address[0]);
    int len_of_remote_address = strlen(remote_address);

    if (len_of_remote_address < MAX_REMOTE_DOMAIN_NAME) {
        memcpy(this->remote_domain_name, remote_address, len_of_remote_address);
    }
    this->job_current_state = 0;
    this->total_number_of_iterations = 0;
}

/**
 * print_Job Prints out the job related parameters.
 * @return
 */
void webclient::Job::print_Job() {
    LOG_NOTICE("%s:%s:%d local_port=%d,local_ipv4=0x%08x,remote_ipv4=0x%08x,current_state=%d,total_no_of_iter=%ld,socket_fd=%d.\n"
            , __FILE__, __FUNCTION__, __LINE__,
            this->local_port,
            this->local_ipv4,
            this->remote_ipv4,
            this->job_current_state,
            this->total_number_of_iterations,
            this->socket_file_descriptor);
}

/**
 * ~Job Destructor.
 */
webclient::Job::~Job() {
    //Do nothing
}

/**
 * return_iteration_count Returns the running count of the number of the cycles the job successfully completed.
 * @return
 */
webclient::uint64_t webclient::Job::return_iteration_count() {
    return this->total_number_of_iterations;
}

/**
 * return_current_job_state Returns the current state of the job.
 * @return
 */
uint8_t webclient::Job::return_current_job_state() {
    return this->job_current_state;
}

/**
 * increment_iteration_count Increment the number of iterations/loops/cycles this job has successfully completed.
 */
void webclient::Job::increment_iteration_count() {
    this->total_number_of_iterations++;
}

/**
 * set_current_job_state Sets the new state of the job.
 * @param new_state
 */
void webclient::Job::set_current_job_state(uint8_t new_state) {
    this->job_current_state = new_state;
}

