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
#include "JobFactory.h"
#include "MutexFactory.h"
#include "one_second_timer.h"
#include "ThreadFactory.h"

pipeline_framework::Scheduler_Factory* pipeline_framework::Scheduler_Factory::m_pInstance = NULL;

/**
 * Scheduler_Factory Constructor.
 */
pipeline_framework::Scheduler_Factory::Scheduler_Factory() {
    //Nothing
}

/**
 * ~Scheduler_Factory Destructor.
 */
pipeline_framework::Scheduler_Factory::~Scheduler_Factory() {
    //Nothing
}

/**
 * Instance Returns the scheduler factory instance.
 * @return
 */
pipeline_framework::Scheduler_Factory* pipeline_framework::Scheduler_Factory::Instance() {
    if (!m_pInstance) {
        m_pInstance = new pipeline_framework::Scheduler_Factory();
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
void pipeline_framework::Scheduler_Factory::initialize(int argc, char **argv) {

    //State Factory is already initialized. (All static functions)
    LOG_DEBUG("%s:%s:%d pipeline_framework::Job_Factory::get_total_number_of_states() returned %d\n",
            __FILE__, __FUNCTION__, __LINE__,
            pipeline_framework::Job_Factory::get_total_number_of_states());

    //Initialize Queue Factory
    pipeline_framework::Queue_Factory::Instance()->set_total_number_of_queues(
            pipeline_framework::Job_Factory::get_total_number_of_states());

    //VLOG_DEBUG("%s:%s:%d queue::Instance()->is_empty(1) returned
    //%d\n",__FILE__,__FUNCTION__,__LINE__,
    //       webclient::Queue_Factory::Instance()->is_empty(1));

    //Initialize Job Factory
    pipeline_framework::Job_Factory::Instance()->create_Jobs(argc, argv);

    //Register for one second timer event callback.
#if 0
    uint8_t current_state = pipeline_framework::Job_Factory::get_init_state();
    uint8_t final_state = pipeline_framework::Job_Factory::get_init_state();

    while (current_state <= final_state) {
        long count = 0;
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(pipeline_framework::Job_Factory::convert_state_to_name(current_state));
        enqueue_job_count.push_back(count);
        one_second_timer_factory::Instance()->register_for_one_second_timer(
                str_count,
                pipeline_framework::Scheduler_Factory::Instance()->return_current_dequeue_done_count);

        std::string str_count2(SCHEDULER_JOB);
        str_count2.append("enqueue_count-");
        str_count2.append(pipeline_framework::Job_Factory::convert_state_to_name(current_state));
        dequeue_job_count.push_back(count);

        one_second_timer_factory::Instance()->register_for_one_second_timer(
                str_count2,
                pipeline_framework::Scheduler_Factory::Instance()->return_current_enqueue_done_count);

        current_state = pipeline_framework::Job_Factory::get_next_state(current_state);
    }
#endif
    //Initialize thread factory.
    pipeline_framework::Thread_Factory::Instance()->Initialize_Thread_Factory();
}

/**
 * return_current_dequeue_done_count A statistic call back fn to the one second timer.
 * It provides an insight into the processing dequeue speed of the current thread.
 * @param arg Thread identifier.
 * @return long returns the running count of the processing dequeue speed of the current thread.
 */
long pipeline_framework::Scheduler_Factory::return_current_dequeue_done_count(void *arg) {
    std::string *p_thread_name = (std::string *) arg;
    long return_value = -1;

    if (!arg) {
        LOG_ERROR("%s:%d Invalid input parameters.\n", __FILE__, __LINE__);
        return return_value;
    }

    uint8_t match_found = 0;
    int index = 0;

    for (index = 0;
            index < pipeline_framework::Job_Factory::get_total_number_of_states();
            index++) {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("dequeue_count-");
        str_count.append(pipeline_framework::Job_Factory::convert_state_to_name(index));

        if (p_thread_name->compare(str_count) == 0) {
            match_found = 1;
            break;
        }
    }

    if (match_found) {
        return_value = pipeline_framework::Scheduler_Factory::Instance()->dequeue_job_count[index];
        pipeline_framework::Scheduler_Factory::Instance()->dequeue_job_count[index] = 0;
    }

    return return_value;
}

/**
 * return_current_enqueue_done_count A statistic call back fn to the one second timer.
 * It provides an insight into the processing enqueue speed of the current thread.
 * @param arg Thread identifier.
 * @return long returns the running count of the processing enqueue speed of the current thread.
 */
long pipeline_framework::Scheduler_Factory::return_current_enqueue_done_count(void *arg) {
    std::string *p_thread_name = (std::string *) arg;
    long return_value = -1;

    if (!arg) {
        LOG_ERROR("%s:%d Invalid input parameters.\n", __FILE__, __LINE__);
        return return_value;
    }

    uint8_t match_found = 0;
    int index = 0;

    for (index = 0;
            index < pipeline_framework::Job_Factory::get_total_number_of_states();
            index++) {
        std::string str_count(SCHEDULER_JOB);
        str_count.append("enqueue_count-");
        str_count.append(pipeline_framework::Job_Factory::convert_state_to_name(index));

        if (p_thread_name->compare(str_count) == 0) {
            match_found = 1;
            break;
        }
    }

    if (match_found) {
#if 0
        pipeline_framework::Thread_Factory::Pthread_variables *p_thread_var =
                pipeline_framework::Thread_Factory::Instance()->thread_var[index];

        if (p_thread_var) {
            return_value = p_thread_var->current_job_count[index];
            p_thread_var->current_job_count[index] = 0;
        }
#endif
        return_value = pipeline_framework::Scheduler_Factory::Instance()->enqueue_job_count[index];
        pipeline_framework::Scheduler_Factory::Instance()->enqueue_job_count[index] = 0;
    }

    return return_value;
}

#define TOTAL_NUMBER_OF_MESSAGES_PER_THREAD 1000

/**
 * run The end user kick starts this loop by invoking run.
 */
void pipeline_framework::Scheduler_Factory::run() {

    uint8_t state = pipeline_framework::Job_Factory::get_init_state();
    uint64_t *ptr_to_state = (uint64_t *) & state;

    LOG_DEBUG("\n%s:%s:%d Going to enqueue all jobs to the first state (%d)\n",
            __FILE__, __FUNCTION__, __LINE__,
            state);

    //For the first time, enqueue all the jobs to the init or the first state queue.
    pipeline_framework::Mutex_Factory::Instance()->condition_signal(
            pipeline_framework::Job_Factory::get_init_state(),
            pipeline_framework::Job_Factory::Instance()->Enqueue_All_Jobs_to_specified_queue,
            (void *) *ptr_to_state);


    //Continuously keep monitoring all the threads.
    while (is_webclient_alive()) {
        //sleep(1);
        //wake up all sleeping threads if their queue is not empty.
        for (int index = pipeline_framework::Job_Factory::get_init_state();
                index < pipeline_framework::Job_Factory::get_total_number_of_states();
                index++) {
            if (!pipeline_framework::Queue_Factory::Instance()->is_empty(index) &&
                    pipeline_framework::Queue_Factory::Instance()->is_this_queue_currently_processed_by_a_thread(
                    index) == FALSE) {
                pipeline_framework::Mutex_Factory::Instance()->condition_signal(
                        index,
                        NULL,
                        NULL);
            }

            //Spawn new thread based upon the current queue count.
            int current_queue_count = pipeline_framework::Queue_Factory::Instance()->count(index);

            if (current_queue_count > TOTAL_NUMBER_OF_MESSAGES_PER_THREAD) {
                int number_of_threads = current_queue_count / TOTAL_NUMBER_OF_MESSAGES_PER_THREAD;
                int current_number_of_threads_per_queue =
                        pipeline_framework::Thread_Factory::Instance()->return_total_number_of_threads_per_queue(index);

                if (current_number_of_threads_per_queue < number_of_threads) {
                    LOG_ERROR("For index=%d,current_queue_count=%d,number_of_threads=%d,current_number_of_threads_per_queue=%d.\n",
                            index,
                            current_queue_count,
                            number_of_threads,
                            current_number_of_threads_per_queue);

                    for (int count = current_number_of_threads_per_queue;
                            count < number_of_threads;
                            count++) {
                        pipeline_framework::Thread_Factory::Instance()->Add_more_threads(index);
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
void pipeline_framework::Scheduler_Factory::Perform_a_Job(uint8_t state_id) {
    uint64_t *ptr_to_state = (uint64_t *) & state_id;

    LOG_DEBUG("\n%s:%s:%d Before invoking condition_wait(). state=(%d)\n",
            __FILE__, __FUNCTION__, __LINE__,
            state_id);
    pipeline_framework::Queue_Factory::Instance()->set_is_thread_processing_in_progress(
            state_id,
            TRUE);

    uint64_t job_id = (uint64_t) pipeline_framework::Mutex_Factory::Instance()->condition_wait(
            state_id,
            pipeline_framework::Scheduler_Factory::Dequeue_Job,
            (void *) *ptr_to_state);
    if (job_id >= 0) {
        LOG_DEBUG("\n%s:%s:%d Found a job. state=(%d)\n",
                __FILE__, __FUNCTION__, __LINE__,
                state_id);
        //pipeline_framework::Scheduler_Factory::Instance()->dequeue_job_count[state_id] += 1;
        int return_code = Scheduler_Factory::Execute_Job(state_id, (uint64_t) job_id);

        if (return_code == SUCCESS) {
            Scheduler_Factory::Move_Job(state_id, (uint64_t) job_id);
        } else {
            //Move all jobs to init state.
            pipeline_framework::Mutex_Factory::Instance()->condition_signal(
                    pipeline_framework::Job_Factory::get_init_state(),
                    pipeline_framework::Job_Factory::Instance()->move_current_job_to_init_state,
                    (void *) job_id);
        }

    } else {
        LOG_DEBUG("\n%s:%s:%d p_job is NULL. state=(%d)\n",
                __FILE__, __FUNCTION__, __LINE__,
                state_id);
    }
    pipeline_framework::Queue_Factory::Instance()->set_is_thread_processing_in_progress(
            state_id,
            FALSE);

}

/**
 * Dequeue_Job This is a helper function to dequeue a job from the specified queue identifier.
 * @param p_state
 * @return
 */
void* pipeline_framework::Scheduler_Factory::Dequeue_Job(void *p_state) {

    void *p_job = NULL;
    uint8_t state_id = 0;
    uint64_t arg = (uint64_t) p_state;
    state_id = (uint8_t) arg;


    if (state_id < pipeline_framework::Job_Factory::get_init_state() ||
            state_id > pipeline_framework::Job_Factory::get_total_number_of_states()) {
        LOG_ERROR("\n%s:%d Invalid input parameter\n",
                __FUNCTION__, __LINE__);
        return p_job;
    }
    LOG_DEBUG("state=%d,is_empty=%d total_number_of_jobs_in_queue=%ld\n",
            state_id,
            pipeline_framework::Queue_Factory::Instance()->is_empty(state_id),
            pipeline_framework::Queue_Factory::Instance()->count(state_id));


    if (!pipeline_framework::Queue_Factory::Instance()->is_empty(state_id)) {
        //p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
        p_job = NULL;
        pipeline_framework::uint32_t length = 0;

        pipeline_framework::Queue_Factory::Instance()->dequeue(state_id,
                (void **) &p_job,
                &length);

        LOG_DEBUG("p_job=%ld,length=%d\n", (uint64_t) p_job, length);
    } else {
        LOG_DEBUG("%s:%s:%d state=%d. Queue is empty.\n",
                __FILE__, __FUNCTION__, __LINE__,
                state_id);
    }


    return (void *) p_job;
}

/**
 * Execute_Job This is a helper function which is used to run the current job.
 * @param p_job
 * @return
 */
int pipeline_framework::Scheduler_Factory::Execute_Job(
        uint8_t state_id,
        uint64_t job_id) {
    if (job_id < 0) {
        LOG_ERROR("%s:%s:%d p_job is NULL\n", __FILE__, __FUNCTION__, __LINE__);
        return FAILURE;
    }

    LOG_DEBUG("Going to run the job (%ld) on this state (%d)\n",
            job_id, state_id);
    return pipeline_framework::Job_Factory::Instance()->run_Job(job_id);
}

/**
 * Move_Job Moves the job from the current queue to the next queue.
 * Note that this has to wake up by signaling the sleeping thread of the next queue.
 * @param p_job
 */
void pipeline_framework::Scheduler_Factory::Move_Job(
        uint8_t state_id,
        uint64_t job_id) {
    if (job_id < 0) {
        LOG_ERROR("%s:%s:%d p_job is NULL\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    uint8_t next_state = pipeline_framework::Job_Factory::Instance()->get_next_state(state_id);

    LOG_DEBUG("Conditional signal to move this job(%ld) to this queue(%d).\n",
            job_id,
            next_state);

    pipeline_framework::Mutex_Factory::Instance()->condition_signal(
            next_state,
            pipeline_framework::Job_Factory::Instance()->move_Job,
            (void *) job_id);
    //pipeline_framework::Scheduler_Factory::Instance()->enqueue_job_count[state_id] += 1;
}

/**
 * stop Stop all job processing.
 */
void pipeline_framework::Scheduler_Factory::stop() {
    //Delete all the Jobs from all the queues.
}
