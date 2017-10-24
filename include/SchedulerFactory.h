#ifndef SCHEDULER_FACTORY_H
#define SCHEDULER_FACTORY_H

#include "namespace.h"
#include "Job.h"

namespace webclient {
class Scheduler_Factory {
  private:
  Scheduler_Factory();

  static Scheduler_Factory* m_pInstance;
  
  public:
   ~Scheduler_Factory();   
   static Scheduler_Factory* Instance();
   void initialize(    
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address);
   void run();
   void stop();
   static void Perform_a_Job(uint8_t state_id);
   static void* Dequeue_Job(void *p_state);
   static void Execute_Job(webclient::Job *p_job);
   static void Move_Job(webclient::Job *p_job);
};
}
#endif
