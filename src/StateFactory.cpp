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
#include "WebClient.h"
/**
 * state_action_var User registers their jobs here in this structure
 * via a call back fun and the corresponding next state to move the job once the job
 * is executed on the current state.
 */
webclient::State_Factory::state_action webclient::State_Factory::state_action_var[] = {
    {
        webclient::State_Factory::SOCKET_CONNECT,
        webclient::WebClientFactory::socket_creator
    },
    {
        webclient::State_Factory::SOCKET_WRITER,
        webclient::WebClientFactory::socket_connect
    },
    {
        webclient::State_Factory::SOCKET_READER,
        webclient::WebClientFactory::socket_writer
    },
    {
        webclient::State_Factory::SOCKET_DESTROYER,
        webclient::WebClientFactory::socket_reader
    },
    {
        webclient::State_Factory::SOCKET_CREATOR,
        webclient::WebClientFactory::socket_destroyer
    },
};

/**
 * convert_state_to_name A helper function to convert a state to a printable name.
 * @param state_val
 * @return string
 */
std::string webclient::State_Factory::convert_state_to_name(uint8_t state_val) {
    std::string state_name;

    switch (state_val) {
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

/**
 * State_Factory Constructor of the state factory.
 */
webclient::State_Factory::State_Factory() {
    //Nothing
}

/**
 * ~State_Factory Destructor of the state factory.
 */
webclient::State_Factory::~State_Factory() {
    //Nothing
}

/**
 * get_total_number_of_states Helper function that returns the total number of states.
 * @return unsigned char.
 */
int8_t webclient::State_Factory::get_total_number_of_states() {
    return SOCKET_STATE_MAX;
}

/**
 * get_init_state Returns the init/first state.
 * @return unsigned char.
 */
uint8_t webclient::State_Factory::get_init_state() {
    return SOCKET_CREATOR;
}

/**
 * run_job_on_this_current_state Invokes the registered call back function to run the job.
 * @param state
 * @param p_job_details
 * @return integer.
 */
int webclient::State_Factory::run_job_on_this_current_state(uint8_t state, void *p_job_details) {
    if (state < 0 || state >= SOCKET_STATE_MAX) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FILE__, __LINE__);
        return -1;
    }

    return webclient::State_Factory::state_action_var[state].action_function(p_job_details);
    ;
}

/**
 * get_next_state Returns the next state of the current job.
 * @param current_state
 * @return
 */
uint8_t webclient::State_Factory::get_next_state(uint8_t current_state) {

    if (current_state < 0 || current_state >= SOCKET_STATE_MAX) {
        LOG_ERROR("%s:%d  Input parameters are invalid.\n", __FILE__, __LINE__);
        return -1;
    }

    return webclient::State_Factory::state_action_var[current_state].next_state;
}