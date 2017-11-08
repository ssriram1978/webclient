/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   web_client_main.cpp
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#include "pipeline_framework.h"
#include "JobFactory.h"
#include "QueueFactory.h"
#include "SchedulerFactory.h"
#include "ThreadFactory.h"
#include "stdin_cb.h"
#include  <signal.h>

unsigned char time_to_exit = 0;

/**
 * is_webclient_alive Informs whether the current process is alive or not.
 * @return
 */
int pipeline_framework::is_webclient_alive() {
    return !time_to_exit;
}

/**
 * INThandler catch SIGINT and shut down the process.
 * @param dummy
 */
void INThandler(int dummy) {
    LOG_ERROR("INThandler: Exiting...\n");
    time_to_exit = 1;
}

int main(int argc, char **argv) {
    //    signal(SIGINT, INThandler);
    initializeLogParameters("WEBCLIENT");
    char threadName[10] = "MAIN";
    pthread_setname_np(pthread_self(), threadName);

    pthread_t pthread_var;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(
            &pthread_var,
            &attr,
            &stdin_cb_fn,
            NULL);

    pipeline_framework::Scheduler_Factory::Instance()->initialize(argc, argv);



#if 0
    pipeline_framework::Job *p_job = (pipeline_framework::Job *) calloc(1, sizeof (pipeline_framework::Job));
    pipeline_framework::uint32_t length = 0;
    pipeline_framework::Queue_Factory::Instance()->dequeue(pipeline_framework::State_Factory::get_init_state(),
            (void **) &p_job, &length);

    printf("%s:%s:%d pmsg=%p,length=%d\n", __FILE__, __FUNCTION__, __LINE__,
            p_job, length);

    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
    pipeline_framework::Scheduler_Factory::Instance()->Process_this_Job(p_job);
#endif
    sleep(1);
    pipeline_framework::Scheduler_Factory::Instance()->run();

    return 1;
}

