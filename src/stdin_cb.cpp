/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdin_cb.h"

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
                break;
            
            case '2':
                break;
            
            default:
                break;
        }
    }
    
    VLOG_EMERG("%s thread exiting",threadName);
   
   return NULL;
    return NULL;
}