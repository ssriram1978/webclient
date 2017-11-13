/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   JobFactory.h
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */

#ifndef JOB_FACTORY_H
#define JOB_FACTORY_H

#include "pipeline_framework.h"
#include "Executor.h"

namespace pipeline_framework {

    class Job_Factory {
    private:
        Job_Factory();
        static Job_Factory* m_pInstance;
        static Executor *p_Executor;
    public:
        ~Job_Factory();
        void create_Jobs(int argc, char **argv);
        int run_Job(uint64_t job_id);
        static Job_Factory* Instance();
        static uint64_t get_total_number_of_Jobs();
        static uint8_t get_total_number_of_states();
        static std::string convert_state_to_name(uint8_t state_val);
        static uint8_t get_init_state();
        static uint8_t get_next_state(uint8_t current_state);
        static uint8_t get_max_state();
        void get_all_jobs(std::vector<uint64_t> &job_identifiers);
        void set_current_job_state(void *p_job_id, uint8_t state);
        uint8_t get_current_job_state(void *p_job_id);
        void increment_iteration_count(void *p_job_id);
        uint64_t get_iteration_count(void *p_job_id);
    };
}
#endif