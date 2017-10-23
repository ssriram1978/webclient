#ifndef JOB_FACTORY_H
#define JOB_FACTORY_H

#include "namespace.h"
#include "Job.h"

namespace webclient {
class Job_Factory {
  private:
  Job_Factory();
  static uint16_t total_number_of_jobs;
  static Job_Factory* m_pInstance;
  
  static webclient::Job *p_jobs;
  public:

  ~Job_Factory();
  void create_Jobs(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address);
  void run_Job(webclient::Job *p_job_obj);
  void move_Job(webclient::Job *p_job_obj);
  static void Enqueue_All_Jobs_to_specified_queue(uint8_t queue_id);
   static Job_Factory* Instance();
};
}
#endif