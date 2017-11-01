/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Job.h
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */

#ifndef JOB_H
#define JOB_H

#include "namespace.h"

#define MAX_REMOTE_DOMAIN_NAME 100
namespace webclient {
class Job {
  private:    
     uint8_t job_current_state;
     uint64_t total_number_of_iterations;
     //friend Job_Factory::move_Job();
     
  public:
    int socket_file_descriptor;
     uint16_t local_port;
     uint32_t local_ipv4;
     uint32_t remote_ipv4;
     char remote_domain_name[MAX_REMOTE_DOMAIN_NAME];
     
    void set_Job(uint16_t local_port,
    uint32_t local_ipv4,
    uint32_t remote_ipv4,
    char remote_address[]);
    void increment_iteration_count();
    void set_current_job_state(uint8_t new_state);
    void print_Job();
    ~Job();

    webclient::uint64_t return_iteration_count();

    uint8_t return_current_job_state();
};
}
#endif