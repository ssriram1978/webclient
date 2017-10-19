#ifndef State_Factory_H
#define State_Factory_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

extern void socket_creator(void *);
extern void socket_connect(void *);
extern void socket_writer(void *);
extern void socket_reader(void *);
extern void socket_destroyer(void *);

class State_Factory {
  private:
     State_Factory();
       typedef enum __state_name 
       {
           SOCKET_CREATOR=0,
           SOCKET_CONNECT=1,
           SOCKET_WRITER=2,
           SOCKET_READER=3,
           SOCKET_DESTROYER=4,
           SOCKET_STATE_MAX=5
       } state_name;
       
       typedef struct __state_action 
        {
            uint8_t next_state;
            void (*action_function)(void *p_job_details);
        } state_action;
     
     static state_action state_action_var[SOCKET_STATE_MAX];

   ~State_Factory();
   public:
   static uint8_t get_total_number_of_states();
   static uint8_t get_init_state();
   static uint8_t run_job_on_this_current_state(uint8_t state,void *p_job_details);
   static uint8_t get_next_state(uint8_t current_state);

};

#endif

