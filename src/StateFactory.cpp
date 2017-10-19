#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#include "StateFactory.h"

State_Factory::state_action State_Factory::state_action_var[] = 
{
   {
       State_Factory::SOCKET_CONNECT,
       socket_creator
   },
   {
       State_Factory::SOCKET_WRITER,
       socket_connect
   },
   { 
       State_Factory::SOCKET_READER,
       socket_writer
   },
   {
       State_Factory::SOCKET_DESTROYER,
        socket_reader
   },
   {
       State_Factory::SOCKET_CREATOR,
       socket_destroyer
   },
};


State_Factory::State_Factory()
{
   //Nothing
}


State_Factory::~State_Factory()
{
    //Nothing
}


uint8_t State_Factory::get_total_number_of_states()
{
   return SOCKET_STATE_MAX;
}

uint8_t State_Factory::get_init_state()
{
   return SOCKET_CREATOR;
}


uint8_t State_Factory::run_job_on_this_current_state(uint8_t state,void *p_job_details)
{
   if(state <0 || state >= SOCKET_STATE_MAX)
   {
      printf("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return -1;
   }

   State_Factory::state_action_var[state].action_function(p_job_details);

   return 0;
}

uint8_t State_Factory::get_next_state(uint8_t current_state)
{
   
   if(current_state <0 || current_state >= SOCKET_STATE_MAX)
   {
      printf("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return -1;
   }

   return State_Factory::state_action_var[current_state].next_state;
}

