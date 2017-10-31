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
#include "StateFactory.h"
#include "JobFactory.h"

extern int is_webclient_alive();

void* stdin_cb_fn(void *arg)
{
    char threadName[10] = "STDIN_CB";
    pthread_setname_np(pthread_self(), threadName);
    
    while(is_webclient_alive())
    {
        char input_command[100]={0};
        VLOG_EMERG("\nEnter your command:");
        scanf("%s",input_command);
        VLOG_EMERG("\nYou entered %s",input_command);
        
        switch(input_command[0])
        {
            case '1':
            {
                const char *p_queue_index = (const char *)(input_command + 1);
                int queue_index= atoi(p_queue_index);
                
                if(queue_index <0 || queue_index >= webclient::State_Factory::get_total_number_of_states())
                {
                    printf("\ninvalid queue index (%d)\n",queue_index);
                }
                else
                {
                    long count = webclient::Queue_Factory::Instance()->count(queue_index);
                    printf("count=%ld\n",count);
                    if(count > 0)
                    {
                        while(count)
                        {
                            webclient::Job *p_job = NULL;
                            webclient::uint32_t length = 0;
                    
                            //p_job = (webclient::Job *) calloc(1,sizeof(webclient::Job));
                            p_job = NULL;
                    
                            webclient::Queue_Factory::Instance()->peek_at_the_front_of_queue(queue_index,
                                (void **)&p_job,
                                &length,
                                count-1);
                        
                            p_job->print_Job();
                            count--;                            
                        }                        
                    }
                }
            }
            break;
                
            case '2':
                break;
            
            default:
                printf("\ninvalid input_command[0]=%c\n",input_command[0]);
                break;
        }
    }
    
    VLOG_EMERG("%s thread exiting",threadName);
   
   return NULL;
    return NULL;
}