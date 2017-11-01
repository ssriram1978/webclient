/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchedulerFactory.h
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */

#ifndef SCHEDULER_FACTORY_H
#define SCHEDULER_FACTORY_H

#include "namespace.h"
#include "Job.h"

namespace webclient {
class Scheduler_Factory {
  private:
  Scheduler_Factory();

  static Scheduler_Factory* m_pInstance;
  typedef std::vector<long> Enqueue_count;
  Enqueue_count enqueue_job_count;
  typedef std::vector<long> Dequeue_count;
  Dequeue_count dequeue_job_count;
  public:
   ~Scheduler_Factory();   
   static Scheduler_Factory* Instance();
   void initialize(    
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address,
    char remote_address[]);
   void run();
   void stop();
   static void Perform_a_Job(uint8_t state_id);
   static void* Dequeue_Job(void *p_state);
   static void Execute_Job(webclient::Job *p_job);
   static void Move_Job(webclient::Job *p_job);
   static long return_current_dequeue_done_count(void *arg);
   static long return_current_enqueue_done_count(void *arg);
};
}
#endif
