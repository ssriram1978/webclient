#ifndef Job_Factory_H
#define Job_Factory_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Job.h"

using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

class Job_Factory {
  private:
  Job_Factory();
  static uint16_t total_number_of_jobs;
  static Job_Factory* m_pInstance;
  
  static Job *p_jobs;
  public:

  ~Job_Factory();
  void create_Jobs(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address);
  void run_Job(Job *p_job_obj);
  void move_Job(Job *p_job_obj);

   static Job_Factory* Instance();
};

#endif

