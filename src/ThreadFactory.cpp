#include "ThreadFactory.h"

webclient::Thread_Factory *webclient::Thread_Factory::Instance()
{
    if (!p_thread_factory)
    {
        p_thread_factory=new Thread_Factory();
    }
    return p_thread_factory;
}

void webclient::Thread_Factory::Initialize_Thread(uint8_t thread_identifier, uint8_t total_number_of_threads)
{
    if ((thread_identifier < webclient::State_Factory::get_init_state()) ||
        (thread_identifier > webclient::State_Factory::get_total_number_of_states()))
    {
        printf("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
        return;
    }
    
    for(count=0; count < total_number_of_threads; count++)
    {
        //Spawn that many number of threads each protected with a mutex lock 
        //before enqueue operation and before dequeue operation.
    }
}
