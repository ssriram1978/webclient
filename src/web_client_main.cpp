#include "namespace.h"
#include "Job.h"
#include "JobFactory.h"
#include "QueueFactory.h"
#include "SchedulerFactory.h"
#include "StateFactory.h"

void print_job_details(void *p_job_details)
{
    webclient::Job *p_job = (webclient::Job *) p_job_details;
    
    if(!p_job)
    {
        printf("%s:%s:%d Invalid job details.\n",__FILE__,__FUNCTION__,__LINE__);
        return;
    }
    
    p_job->print_Job();
}

void socket_creator(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
   
}

void socket_connect(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_writer(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_reader(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}
    
void socket_destroyer(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
   print_job_details(p_job_details);
}

int main()
{
 
   webclient::Scheduler_Factory::Instance()->initialize(1,64000,123456,123456);
   webclient::Scheduler_Factory::Instance()->run();
   
   webclient::Job *p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
   webclient::uint32_t length = 0;
   webclient::Queue_Factory::Instance()->dequeue(webclient::State_Factory::get_init_state(),
           (void **)&p_job,&length);

   printf("%s:%s:%d pmsg=%p,length=%d\n",__FILE__,__FUNCTION__,__LINE__,
           p_job,length);
   
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   webclient::Scheduler_Factory::Instance()->Process_this_Job(p_job);
   return 1;
}

