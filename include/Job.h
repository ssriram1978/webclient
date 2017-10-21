#ifndef JOB_H
#define JOB_H

#include "namespace.h"

namespace webclient {
class Job {
  private:
     uint16_t local_port;
     uint32_t local_ipv4;
     uint32_t remote_ipv4;
     uint8_t job_current_state;
     uint64_t total_number_of_iterations;
     
     //friend Job_Factory::move_Job();
     
  public:

    void set_Job(uint16_t local_port,uint32_t local_ipv4,uint32_t remote_ipv4);
    void increment_iteration_count();
    void set_current_job_state(uint8_t new_state);
    void print_Job();
    ~Job();

    webclient::uint64_t return_iteration_count();

    uint8_t return_current_job_state();
};
}
#endif