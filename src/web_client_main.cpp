#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"


void socket_creator(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

void socket_connect(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}
    
void socket_writer(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}
    
void socket_reader(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}
    
void socket_destroyer(void *p_job_details)
{
   printf("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
}

int main()
{
   Queue_Factory::Instance()->set_total_number_of_queues(5);
   
   printf("queue::Instance()->is_empty(1) returned \
   %d\n",Queue_Factory::Instance()->is_empty(1));

   Queue_Factory::Instance()->enqueue(1,(void *)"sriram",strlen("sriram"));

   
   printf("queue::Instance()->is_empty(1) returned \
   %d\n",Queue_Factory::Instance()->is_empty(1));

   char *pmsg = (char *) calloc(1,100);
   uint32_t length = 0;
   Queue_Factory::Instance()->dequeue(1,(void **)&pmsg,&length);

   printf("pmsg=%s,length=%d\n",pmsg,length);
   
   printf("queue::Instance()->is_empty() returned %d\n",Queue_Factory::Instance()->is_empty(1));

   printf("State_Factory::Instance()->get_total_number_of_states() returned %d\n",State_Factory::get_total_number_of_states());

   for(int index=0; index < State_Factory::get_total_number_of_states(); index++)
   {
       State_Factory::run_job_on_this_current_state(State_Factory::get_next_state(index),NULL);
   }

   Job_Factory::Instance()->create_Jobs(1,64000,123456,123456);
   Job job_obj;
   job_obj.set_Job(1,123456,123456);
   printf("Job_Factory::Instance()->run_Job()\n");
   Job_Factory::Instance()->run_Job(&job_obj);
   printf("Job_Factory::Instance()->move_Job()\n");
   Job_Factory::Instance()->move_Job(&job_obj);
   return 1;
}

