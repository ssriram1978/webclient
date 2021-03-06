/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   StateFactory.h
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#ifndef STATE_FACTORY_H
#define STATE_FACTORY_H

#include "namespace.h"
#include "WebClient.h"
extern void socket_creator(void *);
extern void socket_connect(void *);
extern void socket_writer(void *);
extern void socket_reader(void *);
extern void socket_destroyer(void *);

namespace webclient {

    class State_Factory {
    private:
        State_Factory();

        typedef enum __state_name {
            SOCKET_CREATOR = 0,
            SOCKET_CONNECT = 1,
            SOCKET_WRITER = 2,
            SOCKET_READER = 3,
            SOCKET_DESTROYER = 4,
            SOCKET_STATE_MAX = 5
        } state_name;

        typedef struct __state_action {
            uint8_t next_state;
            int (*action_function)(void *p_job_details);
        } state_action;

        static state_action state_action_var[SOCKET_STATE_MAX];

        ~State_Factory();
    public:
        static int8_t get_total_number_of_states();
        static uint8_t get_init_state();
        static int run_job_on_this_current_state(uint8_t state, void *p_job_details);
        static uint8_t get_next_state(uint8_t current_state);
        static std::string convert_state_to_name(uint8_t state_name);
    };
}
#endif
