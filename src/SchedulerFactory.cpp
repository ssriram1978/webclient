/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SchedulerFactory.cpp
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#include "SchedulerFactory.h"
#include "QueueFactory.h"
#include "StateFactory.h"
#include "JobFactory.h"
#include "MutexFactory.h"
#include "one_second_timer.h"
#include "ThreadFactory.h"

webclient::Scheduler_Factory* webclient::Scheduler_Factory::m_pInstance = NULL;

/**
 * Scheduler_Factory Constructor.
 */
webclient::Scheduler_Factory::Scheduler_Factory() {
    //Nothing
}

/**
 * ~Scheduler_Factory Destructor.
 */
webclient::Scheduler_Factory::~Scheduler_Factory() {
    //Nothing
}

/**
 * Instance Returns the scheduler factory instance.
 * @return
 */
webclient::Scheduler_Factory* webclient::Scheduler_Factory::Instance() {
    if (!m_pInstance) {
        m_pInstance = new webclient::Scheduler_Factory();
    }

    return m_pInstance;
}

#define SCHEDULER_JOB "SCHEDULER_JOB-"

/**
 * initialize This is the front facing interface to the end user.
 * The user provides Job related parameters to Scheduler Factory.
 * The scheduler factory instructs the Job factory to create jobs based upon the input.
 * The scheduler factory instructs Thread and Mutex factories to instantiate themselves.
 *
 * @param starting_port Starting port on the localhost where to bind the socket.
 * @param ending_port Ending port on the localhost where to bind the socket.
 * @param local_ipv4_address Local ipv4 address on the localhost where to bind the socket.
 * @param remote_ipv4_address remote Ipv4 address where to connect.
 * @param remote_address Remote domain name of the host.
 */
void webclient::Scheduler_Factory::initialize(
        uint16_t starting_port,
        uint16_t ending_port,
        uint32_t local_ipv4_address,
        uint32_t remote_ipv4_address,
        char remote_address[]) {

    //State Factory is already initialized. (All static functions)
    LOG_DEBUG("%s:%s:%d State_Factory::Instance()->get_total_number_of_states() returned %d\n",
            __FILE__, __FUNCTION__, __LINE__,
            webclient::State_Factory::get_total_number_of_states());

    //Initialize Queue Factory
    webclient::Queue_Factory::Instance()->set_total_number_of_queues(
            webclient::State_Factory::get_total_number_of_states());

    //VLOG_DEBUG("%s:%s:%d queue::Instance()->is_empty(1) returned
    //%d\n",__FILE__,__FUNCTION__,__LINE__,
    //       webclient::Queue_Factory::Instance()->is_empty(1));

    //Initialize Job Factory
    webclient::Job_Factory::Instance()->create_Jobs(starting_port,
            ending_port,
            local_ipv4_address,
            remote_ipv4_address,
            remote_address);

    //Register for one second timer event callback.
    for (int index = 0;
            index < webclient::State_Factory::get_total_number_of_states();
            index++) {
        long count = 0;
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));
        enqueue_job_count.push_back(count);
        one_second_timer_factory::Instance()->register_for_one_second_timer(
                str_count,
                webclient::Scheduler_Factory::Instance()->return_current_dequeue_done_count);

        std::string str_count2(SCHEDULER_JOB);
        str_count2.append("enqueue_count-");
        str_count2.append(webclient::State_Factory::convert_state_to_name(index));
        dequeue_job_count.push_back(count);

        one_second_timer_factory::Instance()->register_for_one_second_timer(
                str_count2,
                webclient::Scheduler_Factory::Instance()->return_current_enqueue_done_count);
    }

    //Initialize thread factory.
    webclient::Thread_Factory::Instance()->Initialize_Thread_Factory();
}

/**
 * return_current_dequeue_done_count A statistic call back fn to the one second timer.
 * It provides an insight into the processing dequeue speed of the current thread.
 * @param arg Thread identifier.
 * @return long returns the running count of the processing dequeue speed of the current thread.
 */
long webclient::Scheduler_Factory::return_current_dequeue_done_count(void *arg) {
    std::string *p_thread_name = (std::string *) arg;
    long return_value = -1;

    if (!arg) {
        LOG_ERROR("%s:%d Invalid input parameters.\n", __FILE__, __LINE__);
        return return_value;
    }

    uint8_t match_found = 0;
    int index = 0;

    for (index = 0;
            index < webclient::State_Factory::get_total_number_of_states();
            index++) {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));

        if (p_thread_name->compare(str_count) == 0) {
            match_found = 1;
            break;
        }
    }

    if (match_found) {
        return_value = webclient::Scheduler_Factory::Instance()->dequeue_job_count[index];
        webclient::Scheduler_Factory::Instance()->dequeue_job_count[index] = 0;
    }

    return return_value;
}

/**
 * return_current_enqueue_done_count A statistic call back fn to the one second timer.
 * It provides an insight into the processing enqueue speed of the current thread.
 * @param arg Thread identifier.
 * @return long returns the running count of the processing enqueue speed of the current thread.
 */
long webclient::Scheduler_Factory::return_current_enqueue_done_count(void *arg) {
    std::string *p_thread_name = (std::string *) arg;
    long return_value = -1;

    if (!arg) {
        LOG_ERROR("%s:%d Invalid input parameters.\n", __FILE__, __LINE__);
        return return_value;
    }

    uint8_t match_found = 0;
    int index = 0;

    for (index = 0;
            index < webclient::State_Factory::get_total_number_of_states();
            index++) {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("enqueue_count-");
        str_count.append(webclient::State_Factory::convert_state_to_name(index));

        if (p_thread_name->compare(str_count) == 0) {
            match_found = 1;
            break;
        }
    }

    if (match_found) {
#if 0
        webclient::Thread_Factory::Pthread_variables *p_thread_var =
                webclient::Thread_Factory::Instance()->thread_var[index];

        if (p_thread_var) {
            return_value = p_thread_var->current_job_count[index];
            p_thread_var->current_job_count[index] = 0;
        }
#endif
        return_value = webclient::Scheduler_Factory::Instance()->enqueue_job_count[index];
        webclient::Scheduler_Factory::Instance()->enqueue_job_count[index] = 0;
    }

    return return_value;
}

#define TOTAL_NUMBER_OF_MESSAGES_PER_THREAD 1000

/**
 * run The end user kick starts this loop by invoking run.
 */
void webclient::Scheduler_Factory::run() {
    uint8_t state = webclient::State_Factory::get_init_state();
    uint64_t *ptr_to_state = (uint64_t *) & state;

    LOG_DEBUG("\n%s:%s:%d Going to enqueue all jobs to the first state (%d)\n",
            __FILE__, __FUNCTION__, __LINE__,
            state);

    //For the first time, enqueue all the jobs to the init or the first state queue.
    webclient::Mutex_Factory::Instance()->condition_signal(
            webclient::State_Factory::get_init_state(),
            webclient::Job_Factory::Instance()->Enqueue_All_Jobs_to_specified_queue,
            (void *) *ptr_to_state);

    //Continuously keep monitoring all the threads.
    while (is_webclient_alive()) {
        //sleep(1);
        //wake up all sleeping threads if their queue is not empty.
        for (int index = webclient::State_Factory::get_init_state();
                index < webclient::State_Factory::get_total_number_of_states();
                index++) {
            if (!webclient::Queue_Factory::Instance()->is_empty(index)) {
                webclient::Mutex_Factory::Instance()->condition_signal(
                        index,
                        NULL,
                        NULL);
            }

            //Spawn new thread based upon the current queue count.
            int current_queue_count = webclient::Queue_Factory::Instance()->count(index);

            if (current_queue_count > TOTAL_NUMBER_OF_MESSAGES_PER_THREAD) {
                int number_of_threads = current_queue_count / TOTAL_NUMBER_OF_MESSAGES_PER_THREAD;
                int current_number_of_threads_per_queue =
                        webclient::Thread_Factory::Instance()->return_total_number_of_threads_per_queue(index);

                if (current_number_of_threads_per_queue < number_of_threads) {
                    LOG_ERROR("For index=%d,current_queue_count=%d,number_of_threads=%d,current_number_of_threads_per_queue=%d.\n",
                            index,
                            current_queue_count,
                            number_of_threads,
                            current_number_of_threads_per_queue);

                    for (int count = current_number_of_threads_per_queue;
                            count < number_of_threads;
                            count++) {
                        webclient::Thread_Factory::Instance()->Add_more_threads(index);
                    }
                }
            }
        }
    }
}

/**
 * Perform_a_Job This function gets invoked by a thread.
 * Try to dequeue a message from the queue assigned to the current thread.
 * After dequeuing the message, execute or run the job.
 * After the job is done, move the job to the next queue so that
 * the next thread picks up the job from where it was left off by this current thread.
 * @param state_id
 */
void webclient::Scheduler_Factory::Perform_a_Job(uint8_t state_id) {
    uint64_t *ptr_to_state = (uint64_t *) & state_id;

    LOG_DEBUG("\n%s:%s:%d Before invoking condition_wait(). state=(%d)\n",
            __FILE__, __FUNCTION__, __LINE__,
            state_id);
    webclient::Job *p_job = (webclient::Job *) webclient::Mutex_Factory::Instance()->condition_wait(
            state_id,
            webclient::Scheduler_Factory::Dequeue_Job,
            (void *) *ptr_to_state);
    if (p_job) {
        LOG_DEBUG("\n%s:%s:%d Found a job. state=(%d)\n",
                __FILE__, __FUNCTION__, __LINE__,
                state_id);
        webclient::Scheduler_Factory::Instance()->dequeue_job_count[state_id] += 1;
        int return_code = Scheduler_Factory::Execute_Job(p_job);
        if (return_code == SUCCESS) {
            Scheduler_Factory::Move_Job(p_job);
        } else {
            //Move all jobs to init state.
            //Scheduler_Factory::run();
        }

    } else {
        LOG_DEBUG("\n%s:%s:%d p_job is NULL. state=(%d)\n",
                __FILE__, __FUNCTION__, __LINE__,
                state_id);
    }
}

/**
 * Dequeue_Job This is a helper function to dequeue a job from the specified queue identifier.
 * @param p_state
 * @return
 */
void* webclient::Scheduler_Factory::Dequeue_Job(void *p_state) {
    webclient::Job *p_job = NULL;
    uint8_t state_id = 0;
    uint64_t arg = (uint64_t) p_state;
    state_id = (uint8_t) arg;

    if (state_id < webclient::State_Factory::get_init_state() ||
            state_id > webclient::State_Factory::get_total_number_of_states()) {
        LOG_ERROR("\n%s:%d Invalid input parameter\n",
                __FUNCTION__, __LINE__);
        return p_job;
    }

    LOG_DEBUG("%s:%s:%d state=%d,is_empty=%d\n", __FILE__, __FUNCTION__, __LINE__,
            state_id, webclient::Queue_Factory::Instance()->is_empty(state_id));

    if (!webclient::Queue_Factory::Instance()->is_empty(state_id)) {
        //p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
        p_job = NULL;
        webclient::uint32_t length = 0;

        webclient::Queue_Factory::Instance()->dequeue(state_id,
                (void **) &p_job,
                &length);

        LOG_DEBUG("%s:%s:%d p_job=%p,length=%d\n", __FILE__, __FUNCTION__, __LINE__,
                p_job, length);
    } else {
        LOG_DEBUG("%s:%s:%d state=%d. Queue is empty.\n", __FILE__, __FUNCTION__, __LINE__,
                state_id);
    }

    return (void *) p_job;
}

/**
 * Execute_Job This is a helper function which is used to run the current job.
 * @param p_job
 * @return
 */
int webclient::Scheduler_Factory::Execute_Job(webclient::Job *p_job) {
    if (!p_job) {
        LOG_ERROR("%s:%s:%d p_job is NULL\n", __FILE__, __FUNCTION__, __LINE__);
        return FAILURE;
    }

    LOG_DEBUG("Scheduler_Factory Job_Factory::Instance()->run_Job()\n");
    return webclient::Job_Factory::Instance()->run_Job(p_job);
}

/**
 * Move_Job Moves the job from the current queue to the next queue.
 * Note that this has to wake up by signaling the sleeping thread of the next queue.
 * @param p_job
 */
void webclient::Scheduler_Factory::Move_Job(webclient::Job *p_job) {
    if (!p_job) {
        LOG_ERROR("%s:%s:%d p_job is NULL\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    LOG_DEBUG("Scheduler_Factory Job_Factory::Instance()->move_Job()\n");

    uint8_t next_state = webclient::State_Factory::get_next_state(p_job->return_current_job_state());

    webclient::Mutex_Factory::Instance()->condition_signal(
            next_state,
            webclient::Job_Factory::Instance()->move_Job,
            (void *) p_job);
    webclient::Scheduler_Factory::Instance()->enqueue_job_count[p_job->return_current_job_state()] += 1;
}

/**
 * stop Stop all job processing.
 */
void webclient::Scheduler_Factory::stop() {
    //Delete all the Jobs from all the queues.
}
