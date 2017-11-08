/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   stdin_cb.cpp
 * Author: ssridhar
 *
 * Created on October 31, 2017, 1:06 PM
 */
#include "stdin_cb.h"
#include "QueueFactory.h"
#include "JobFactory.h"
#include "one_second_timer.h"

/**
 * stdin_cb_fn This is the entry point for the back door entry into the current process.
 * This is used for debugging purpose only.
 * @param arg
 * @return
 */
void* stdin_cb_fn(void *arg) {
    char threadName[10] = "STDIN_CB";
    pthread_setname_np(pthread_self(), threadName);

    while (pipeline_framework::is_webclient_alive()) {
        char input_command[100] = {0};
        LOG_EMERG("\nEnter your command:");
        scanf("%s", input_command);
        LOG_EMERG("\nYou entered %s", input_command);

        switch (input_command[0]) {
            case '1':
            {
                const char *p_queue_index = (const char *) (input_command + 1);
                int queue_index = atoi(p_queue_index);

                if (queue_index < 0 || queue_index >= pipeline_framework::Job_Factory::get_total_number_of_states()) {
                    printf("\ninvalid queue index (%d)\n", queue_index);
                } else {
                    long count = pipeline_framework::Queue_Factory::Instance()->count(queue_index);
                    printf("count=%ld\n", count);
                    if (count > 0) {
                        while (count) {
                            void *p_job = NULL;
                            pipeline_framework::uint32_t length = 0;

                            //p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
                            p_job = NULL;

                            pipeline_framework::Queue_Factory::Instance()->peek_at_the_front_of_queue(queue_index,
                                    (void **) &p_job,
                                    &length,
                                    count - 1);

                            //p_job->print_Job();
                            count--;
                        }
                    }
                }
            }
                break;

            case '2':
            {
                //turn on printing the counts.
                pipeline_framework::one_second_timer_factory::Instance()->print_stats = 1;
            }
                break;

            case '3':
            {
                //turn off printing the counts.
                pipeline_framework::one_second_timer_factory::Instance()->print_stats = 0;
            }
                break;


            default:
                printf("\ninvalid input_command[0]=%c\n", input_command[0]);
                break;
        }
    }

    LOG_EMERG("%s thread exiting", threadName);

    return NULL;
}