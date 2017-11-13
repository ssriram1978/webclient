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

#include "pipeline_framework.h"

namespace pipeline_framework {

    class Job_Scheduler {
    private:
        Job_Scheduler();

        static Job_Scheduler* m_pInstance;
        typedef std::vector<long> Enqueue_count;
        Enqueue_count enqueue_job_count;
        typedef std::vector<long> Dequeue_count;
        Dequeue_count dequeue_job_count;
    public:
        ~Job_Scheduler();
        static Job_Scheduler* Instance();
        void initialize(int argc, char **argv);
        void run();
        void stop();
        static void Perform_a_Job(uint8_t state_id);
        static void* Dequeue_Job(void *p_state);
        static int Execute_Job(uint8_t state_id, uint64_t job_id);
        static void Move_Job(uint8_t state_id, uint64_t job_id);
        static long return_current_dequeue_done_count(void *arg);
        static long return_current_enqueue_done_count(void *arg);
        static void Enqueue_All_Jobs_to_specified_queue(void *p_queue_id);
        static void move_current_job_to_init_state(void *p_job_id);
        static void move_Job(void *p_job_id);

    };
}
#endif
