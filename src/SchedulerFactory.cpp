#include "SchedulerFactory.h"
#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"

webclient::Scheduler_Factory* webclient::Scheduler_Factory::m_pInstance = NULL;


webclient::Scheduler_Factory::Scheduler_Factory()
{
  //Nothing
}


webclient::Scheduler_Factory::~Scheduler_Factory()
{
  //Nothing
}

webclient::Scheduler_Factory* webclient::Scheduler_Factory::Instance()
{       
    if(!m_pInstance)
    {
       m_pInstance = new webclient::Scheduler_Factory();
    }
    
    return m_pInstance;
}

void webclient::Scheduler_Factory::initialize(
    uint16_t starting_port,
    uint16_t ending_port,
    uint32_t local_ipv4_address,
    uint32_t remote_ipv4_address)
{

   //State Factory is already initialized. (All static functions)
   printf("%s:%s:%d State_Factory::Instance()->get_total_number_of_states() returned %d\n",
           __FILE__,__FUNCTION__,__LINE__,
           webclient::State_Factory::get_total_number_of_states());

   //Initialize Queue Factory
   webclient::Queue_Factory::Instance()->set_total_number_of_queues(webclient::State_Factory::get_total_number_of_states());

   printf("%s:%s:%d queue::Instance()->is_empty(1) returned \
   %d\n",__FILE__,__FUNCTION__,__LINE__,
           webclient::Queue_Factory::Instance()->is_empty(1));

      //Initialize Job Factory
   webclient::Job_Factory::Instance()->create_Jobs(starting_port,ending_port,local_ipv4_address,remote_ipv4_address);

#if 0  
   webclient::Queue_Factory::Instance()->enqueue(1,(void *)"sriram",strlen("sriram"));

   
   printf("%s:%s:%d queue::Instance()->is_empty(1) returned \
   %d\n",__FILE__,__FUNCTION__,__LINE__,
        webclient::Queue_Factory::Instance()->is_empty(1));

   char *pmsg = (char *) calloc(1,100);
   uint32_t length = 0;
   webclient::Queue_Factory::Instance()->dequeue(1,(void **)&pmsg,&length);

   printf("%s:%s:%d pmsg=%s,length=%d\n",__FILE__,__FUNCTION__,__LINE__,
           pmsg,length);
   
   printf("%s:%s:%d queue::Instance()->is_empty() returned %d\n",
           __FILE__,__FUNCTION__,__LINE__,
           webclient::Queue_Factory::Instance()->is_empty(1));
   
   webclient::Job_Factory::Instance()->create_Jobs(1,64000,123456,123456);
   webclient::Job job_obj;
   job_obj.set_Job(1,123456,123456);
   printf("Job_Factory::Instance()->run_Job()\n");
   webclient::Job_Factory::Instance()->run_Job(&job_obj);
   printf("Job_Factory::Instance()->move_Job()\n");
   webclient::Job_Factory::Instance()->move_Job(&job_obj);
   
   for(int index=0; index < webclient::State_Factory::get_total_number_of_states(); index++)
   {
       webclient::State_Factory::run_job_on_this_current_state(webclient::State_Factory::get_next_state(index),NULL);
   }
#endif   
   
   //Initialize Thread Factory   
}


void webclient::Scheduler_Factory::run()
{
   //Enqueue all the Jobs to the queue listed on the first state.
   webclient::Job_Factory::Instance()->Enqueue_All_Jobs_to_specified_queue(
   webclient::State_Factory::get_init_state());
}

void webclient::Scheduler_Factory::Process_this_Job(webclient::Job *p_job)
{
   printf("Scheduler_Factory Job_Factory::Instance()->run_Job()\n");
   webclient::Job_Factory::Instance()->run_Job(p_job);
   printf("Scheduler_Factory Job_Factory::Instance()->move_Job()\n");
   webclient::Job_Factory::Instance()->move_Job(p_job); 
}

void webclient::Scheduler_Factory::stop()
{
    //Delete all the Jobs from all the queues.
}

