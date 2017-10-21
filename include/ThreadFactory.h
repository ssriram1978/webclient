#ifndef THREAD_FACTORY_H
#define THREAD_FACTORY_H

namespace webclient {
    
    class Thread_Factory()
    {
        
    private:
#define MAX_THREADS_PER_QUEUE 10
        Thread_Factory();
        static Thread_Factory *p_thread_factory;
    public:
        Thread_Factory *Instance();
        void Initialize_Thread(uint8_t thread_identifier, uint8_t total_number_of_threads);
        
    };
    
}

#endif