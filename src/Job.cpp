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

void webclient::Job::set_Job(uint16_t local_port,
        uint32_t local_ipv4,
        uint32_t remote_ipv4,
        char remote_address[])
{
   this->local_port=local_port;
   this->local_ipv4=local_ipv4;
   this->remote_ipv4=remote_ipv4;
   int len_of_remote_address = sizeof(remote_address)/sizeof(char);
   if(len_of_remote_address < MAX_REMOTE_DOMAIN_NAME)
   {
       memcpy(this->remote_domain_name,remote_address,len_of_remote_address);
   }
   this->job_current_state=0;
   this->total_number_of_iterations=0;
}    

void webclient::Job::print_Job()
{
    VLOG_NOTICE("%s:%s:%d local_port=%d,local_ipv4=0x%08x,remote_ipv4=0x%08x,current_state=%d,total_no_of_iter=%ld,socket_fd=%d.\n"
            ,__FILE__,__FUNCTION__,__LINE__,
            this->local_port,
            this->local_ipv4,
            this->remote_ipv4,
            this->job_current_state,
            this->total_number_of_iterations,
            this->socket_file_descriptor);
}

webclient::Job::~Job()
{
  //Do nothing
}

webclient::uint64_t webclient::Job::return_iteration_count()
{
   return this->total_number_of_iterations;
}

uint8_t webclient::Job::return_current_job_state()
{
   return this->job_current_state;
}

void webclient::Job::increment_iteration_count()
{
   this->total_number_of_iterations++;
}

void webclient::Job::set_current_job_state(uint8_t new_state)
{
   this->job_current_state=new_state;
}

