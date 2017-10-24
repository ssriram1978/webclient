#ifndef THREAD_FACTORY_H
#define THREAD_FACTORY_H

#include <map>
#include <pthread.h>
#include <vector>

namespace webclient {   
    class Thread_Factory {        
    private:
        typedef std::vector<pthread_t*> Pthread_array;
        
        typedef struct __pthread_variables
        {
            uint8_t total_number_of_pthreads;            
            Pthread_array thread_array_var;
        }Pthread_variables;
        
        typedef std::map<uint8_t,Pthread_variables*> Thread_map;
        Thread_map thread_var;
                
        static void* thread_main_job(void *arg);
        static Thread_Factory *p_thread_factory;
        
        Thread_Factory();
    
    public:
        static Thread_Factory *Instance();
        void Initialize_Thread_Factory();
        void Add_more_threads(uint8_t thread_identifier);
        void Delete_threads(uint8_t thread_identifier);
        virtual ~Thread_Factory();
    };
}

#endif