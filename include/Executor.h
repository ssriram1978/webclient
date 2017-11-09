/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   WebClient.h
 * Author: ssridhar
 *
 * Created on November 1, 2017, 11:46 AM
 */

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

namespace pipeline_framework {


#define STARTING_PORT 1025
#define ENDING_PORT 64000
#define LOCAL_IP 0xfefefefe
#define REMOTE_IP 0xfefe1234
#define MAX_REMOTE_ADDRESS_LEN 200
#define MAX_NUMBER_OF_STATES 100

    class Executor {
    private:
        void print_job_details(void *p_job_details);

        uint16_t local_starting_port;
        uint16_t local_ending_port;
        uint32_t local_ip_address;
        uint32_t remote_ip_address;
        char remote_address[MAX_REMOTE_ADDRESS_LEN];
        uint64_t total_number_of_jobs;

        uint32_t convert_domain_name_to_ip_address(char *p_IPAddr);
        int hostname_to_ip(const char *hostname,
                char *service,
                char *ip,
                uint32_t ip_len);

        typedef struct __state_action {
            uint8_t next_state;
            int (*action_function)(void *p_job_details);
        } state_action;

        static state_action state_action_var[MAX_NUMBER_OF_STATES];

        typedef struct _Job {
            int socket_file_descriptor;
            uint16_t local_port;
            uint32_t local_ipv4;
            uint32_t remote_ipv4;
            char remote_domain_name[MAX_REMOTE_ADDRESS_LEN];
            uint8_t current_state;
            uint64_t iteration_count;
        } Job;

        Job *p_jobs;
        void prepare_jobs();

        typedef enum __state_name {
            SOCKET_CREATOR = 0,
            SOCKET_CONNECT = 1,
            SOCKET_WRITER = 2,
            SOCKET_READER = 3,
            SOCKET_DESTROYER = 4,
            SOCKET_STATE_MAX = 5
        } state_name;

        static int socket_creator(void *p_job_details);
        static int socket_connect(void *p_job_details);
        static int socket_writer(void *p_job_details);
        static int socket_reader(void *p_job_details);
        static int socket_destroyer(void *p_job_details);

    public:
        Executor();
        void initialize(int argc, char **argv);
        uint64_t get_total_number_of_jobs();
        uint8_t get_total_number_of_states();
        uint8_t get_init_state();
        uint8_t get_next_state(uint8_t current_state);
        uint8_t get_max_state();
        uint8_t get_current_state(uint64_t job_identifier);
        void increment_iteration_count(uint64_t job_identifier);
        void set_current_job_state(uint64_t job_identifier,
                uint8_t current_state);

        void get_all_jobs(std::vector<uint64_t> &job_identifiers);
        int process_job(uint64_t job_identifier);
        void print_job(uint64_t job_identifier);
        std::string convert_state_to_name(uint8_t state_val);
    };
}


#endif /* WEBCLIENT_H */

