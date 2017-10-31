/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StateFactory.cpp
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */
#include "StateFactory.h"

webclient::State_Factory::state_action webclient::State_Factory::state_action_var[] = 
{
   {
       webclient::State_Factory::SOCKET_CONNECT,
       socket_creator
   },
   {
       webclient::State_Factory::SOCKET_WRITER,
       socket_connect
   },
   { 
       webclient::State_Factory::SOCKET_READER,
       socket_writer
   },
   {
       webclient::State_Factory::SOCKET_DESTROYER,
        socket_reader
   },
   {
       webclient::State_Factory::SOCKET_CREATOR,
       socket_destroyer
   },
};


std::string webclient::State_Factory::convert_state_to_name(uint8_t state_val)
{
    std::string state_name;
    
    switch(state_val)
    {
        case webclient::State_Factory::SOCKET_CONNECT:
        state_name.assign("SOCKET_CONNECT");
        break;
        
        case webclient::State_Factory::SOCKET_WRITER:
        state_name.assign("SOCKET_WRITER");
        break;
        
        case webclient::State_Factory::SOCKET_READER:
        state_name.assign("SOCKET_READER");
        break;

        case webclient::State_Factory::SOCKET_DESTROYER:
        state_name.assign("SOCKET_DESTROY");
        break;

        case webclient::State_Factory::SOCKET_CREATOR:
        state_name.assign("SOCKET_CREATOR");
        break;
        
        default:
        state_name.assign("INVALID");
        break;
    }
    
    return state_name;
}


webclient::State_Factory::State_Factory()
{
   //Nothing
}


webclient::State_Factory::~State_Factory()
{
    //Nothing
}


uint8_t webclient::State_Factory::get_total_number_of_states()
{
   return SOCKET_STATE_MAX;
}

uint8_t webclient::State_Factory::get_init_state()
{
   return SOCKET_CREATOR;
}


uint8_t webclient::State_Factory::run_job_on_this_current_state(uint8_t state,void *p_job_details)
{
   if(state <0 || state >= SOCKET_STATE_MAX)
   {
      VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return -1;
   }

   webclient::State_Factory::state_action_var[state].action_function(p_job_details);

   return 0;
}

uint8_t webclient::State_Factory::get_next_state(uint8_t current_state)
{
   
   if(current_state <0 || current_state >= SOCKET_STATE_MAX)
   {
      VLOG_ERROR("%s:%d  Input parameters are invalid.\n",__FILE__,__LINE__);
      return -1;
   }

   return webclient::State_Factory::state_action_var[current_state].next_state;
}

