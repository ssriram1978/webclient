/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   WebClient.cpp
 * Author: ssridhar
 *
 * Created on November 1, 2017, 11:46 AM
 */

#include "pipeline_framework.h"
#include "Executor.h"
#include "Log.h"

extern LogLevel gCommonLogLevel;

/**
 * WebClientFactory Constructor
 */
pipeline_framework::Executor::Executor() {
    local_starting_port = STARTING_PORT;
    local_ending_port = ENDING_PORT;
    local_ip_address = LOCAL_IP;
    remote_ip_address = REMOTE_IP;
    memset(remote_address, 0, MAX_REMOTE_ADDRESS_LEN);
    total_number_of_jobs = 0;
}

/**
 * print_job_details Prints the job details.
 * @param p_job_details
 */
void pipeline_framework::Executor::print_job_details(void *p_job_details) {
    pipeline_framework::Executor::Job *p_job = (pipeline_framework::Executor::Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("%s:%s:%d Invalid job details.\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    //p_job->print_Job();
}

void pipeline_framework::Executor::initialize(int argc, char **argv) {
    if (!argv) {
        LOG_ERROR("Invalid input parameters.\n");
        return;
    }

    char c = '\0';

    while ((c = getopt(argc, argv, "d:s:e:l:r:t:")) != -1) {
        switch (c) {
            case 'd':
                switch (atoi(optarg)) {
                    case 0:
                        setLogLevel("DEBUG");
                        break;

                    case 1:
                        setLogLevel("INFO");
                        break;

                    case 2:
                        setLogLevel("NOTICE");
                        break;

                    case 3:
                        setLogLevel("ERROR");
                        break;

                    default:
                        setLogLevel("NOTICE");
                        break;
                }
                break;

            case 's':
                local_starting_port = (unsigned short) atoi(optarg);
                break;

            case 'e':
                local_ending_port = (unsigned short) atoi(optarg);
                break;

            case 'l':
                local_ip_address = convert_domain_name_to_ip_address(optarg);
                break;

            case 'r':
            {
                char *p_IPAddr = (char *) optarg;
                if (p_IPAddr && strlen(p_IPAddr) < MAX_REMOTE_ADDRESS_LEN) {
                    memset(remote_address, 0, MAX_REMOTE_ADDRESS_LEN);
                    memcpy(remote_address, p_IPAddr, strlen(p_IPAddr));
                    remote_ip_address = convert_domain_name_to_ip_address(optarg);
                }
            }
                break;

            case 'h':
            default:
                LOG_ERROR("options: [-d <dbglvl 0-DEBUG,1-INFO,2-NOTICE,3-ERROR>] "
                        "           [-s <starting port>] "
                        "           [-e <ending port>] "
                        "           [-l <local ipaddr>] "
                        "           [-r <remote ip or domain name>]\n");
                exit(EXIT_FAILURE);
                break;
        }
    }

    printf("local_starting_port=%d,"
            "local_ending_port=%d,"
            "local_ip_address=%08x,"
            "remote_ip_address=%08x,"
            "remote_address=%s",
            local_starting_port,
            local_ending_port,
            local_ip_address,
            remote_ip_address,
            remote_address);

    prepare_jobs();
}

void pipeline_framework::Executor::prepare_jobs() {
    uint16_t total_number_of_ports = (local_ending_port - local_starting_port) + 1;

    p_jobs = (Job *) calloc(total_number_of_ports, sizeof (Job));

    LOG_DEBUG("total_number_of_ports=%d\n", total_number_of_ports);

    for (uint16_t count = 0;
            count < total_number_of_ports;
            count++) {
        (p_jobs + count)->local_port = local_starting_port + count;
        (p_jobs + count)->local_ipv4 = local_ip_address;
        (p_jobs + count)->remote_ipv4 = remote_ip_address;
        memcpy((p_jobs + count)->remote_domain_name,
                remote_address,
                strlen(remote_address));
        total_number_of_jobs++;
    }
}

/**
 * hostname_to_ip Helper function to convert a hostname to IP address.
 * @param hostname
 * @param service
 * @param ip
 * @param ip_len
 * @return ipv4 address.
 */
int pipeline_framework::Executor::hostname_to_ip(const char *hostname,
        char *service,
        char *ip,
        uint32_t ip_len) {
    //int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    struct sockaddr_in6 *h6;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(hostname, service, &hints, &servinfo)) != 0) {
        LOG_ERROR("getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            h = (struct sockaddr_in *) p->ai_addr;
            inet_ntop(p->ai_family,
                    (const void *) &(h->sin_addr.s_addr),
                    ip,
                    (socklen_t) ip_len);

            LOG_ERROR("Hostname %s Resolved to %s\n", hostname, ip);
            break;
        } else if (p->ai_family == AF_INET6) {
            h6 = (struct sockaddr_in6 *) p->ai_addr;
            inet_ntop(p->ai_family,
                    (const void *) &(h6->sin6_addr.s6_addr[0]),
                    ip,
                    (socklen_t) ip_len);

            LOG_ERROR("Hostname %s Resolved to %s\n", hostname, ip);
            break;
        } else {
            LOG_ERROR("Invalid address family\n");
        }
    }
    freeaddrinfo(servinfo); // all done with this structure, free 'er up
    return 0;
}

#define DOMAIN_NAME_ADDRESS_MAX 256

/**
 * convert_domain_name_to_ip_address This converts a domain name to IP address.
 * @param p_IPAddr
 * @return
 */
uint32_t pipeline_framework::Executor::convert_domain_name_to_ip_address(char *p_IPAddr) {
    char nIPAddr[256];
    int hname_lkup_rc = 0;
    uint32_t ipaddress = -1;

    if (strlen(p_IPAddr) < DOMAIN_NAME_ADDRESS_MAX) {
        hname_lkup_rc = hostname_to_ip(p_IPAddr,
                NULL,
                nIPAddr,
                sizeof (nIPAddr));
        if (hname_lkup_rc != 0) {
            LOG_ERROR("IP name resolution failure... exiting\n");
            exit(2);
        }

        int s = inet_pton(AF_INET, nIPAddr, &ipaddress);
        if (s <= 0) {
            if (s == 0)
                LOG_ERROR("%s:%d ipv4 addr not in presentation format\n", __FILE__, __LINE__);
            else
                perror("inet_pton v4\n");
        }
    }
    return ipaddress;
}

uint64_t pipeline_framework::Executor::get_total_number_of_jobs() {
    return total_number_of_jobs;
}

uint8_t pipeline_framework::Executor::get_current_state(uint64_t job_identifier) {
    if ((job_identifier <= 0) || (job_identifier > total_number_of_jobs)) {
        LOG_ERROR("Invalid job id(%ld).\n", (job_identifier - 1));
        return FAILURE;
    }
    Job *p_current_job = (Job *) (p_jobs + (job_identifier - 1));

    if (!p_current_job) {
        LOG_ERROR("Job cannot be found with (%ld).\n", (job_identifier - 1));
        return FAILURE;
    }

    return p_current_job->current_state;
}

void pipeline_framework::Executor::increment_iteration_count(uint64_t job_identifier) {
    if ((job_identifier <= 0) || (job_identifier > total_number_of_jobs)) {
        LOG_ERROR("Invalid job id(%ld).\n", (job_identifier - 1));
        return;
    }
    Job *p_current_job = (Job *) (p_jobs + (job_identifier - 1));

    if (!p_current_job) {
        LOG_ERROR("Job cannot be found with (%ld).\n", (job_identifier - 1));
        return;
    }

    p_current_job->iteration_count++;
}

void pipeline_framework::Executor::set_current_job_state(uint64_t job_identifier,
        uint8_t current_state) {
    if ((job_identifier <= 0) || (job_identifier > total_number_of_jobs)) {
        LOG_ERROR("Invalid job id(%ld).\n", (job_identifier - 1));
        return;
    }
    Job *p_current_job = (Job *) (p_jobs + (job_identifier - 1));

    if (!p_current_job) {
        LOG_ERROR("Job cannot be found with (%ld).\n", (job_identifier - 1));
        return;
    }

    p_current_job->current_state = current_state;
}

uint8_t pipeline_framework::Executor::get_total_number_of_states() {
    return SOCKET_STATE_MAX;
}

uint8_t pipeline_framework::Executor::get_init_state() {
    return SOCKET_CREATOR;
}

uint8_t pipeline_framework::Executor::get_next_state(uint8_t current_state) {
    return pipeline_framework::Executor::state_action_var[current_state].next_state;
}

uint8_t pipeline_framework::Executor::get_max_state() {
    return SOCKET_STATE_MAX;
}



/**
 * state_action_var User registers their jobs here in this structure
 * via a call back fun and the corresponding next state to move the job once the job
 * is executed on the current state.
 */
pipeline_framework::Executor::state_action pipeline_framework::Executor::state_action_var[] = {
    {
        pipeline_framework::Executor::SOCKET_CONNECT,
        pipeline_framework::Executor::socket_creator
    },
    {
        pipeline_framework::Executor::SOCKET_WRITER,
        pipeline_framework::Executor::socket_connect
    },
    {
        pipeline_framework::Executor::SOCKET_READER,
        pipeline_framework::Executor::socket_writer
    },
    {
        pipeline_framework::Executor::SOCKET_DESTROYER,
        pipeline_framework::Executor::socket_reader
    },
    {
        pipeline_framework::Executor::SOCKET_CREATOR,
        pipeline_framework::Executor::socket_destroyer
    },
};

/**
 * convert_state_to_name A helper function to convert a state to a printable name.
 * @param state_val
 * @return string
 */
std::string pipeline_framework::Executor::convert_state_to_name(uint8_t state_val) {
    std::string state_name;

    switch (state_val) {
        case pipeline_framework::Executor::SOCKET_CONNECT:
            state_name.assign("SOCKET_CONNECT");
            break;

        case pipeline_framework::Executor::SOCKET_WRITER:
            state_name.assign("SOCKET_WRITER");
            break;

        case pipeline_framework::Executor::SOCKET_READER:
            state_name.assign("SOCKET_READER");
            break;

        case pipeline_framework::Executor::SOCKET_DESTROYER:
            state_name.assign("SOCKET_DESTROY");
            break;

        case pipeline_framework::Executor::SOCKET_CREATOR:
            state_name.assign("SOCKET_CREATOR");
            break;

        default:
            state_name.assign("INVALID");
            break;
    }

    return state_name;
}

void pipeline_framework::Executor::get_all_jobs(std::vector<uint64_t> &job_identifiers) {
    for (uint64_t count = 1; count <= total_number_of_jobs; count++) {
        //LOG_DEBUG("Pushing job id(%ld).\n", count);
        job_identifiers.push_back(count);
    }
}

int pipeline_framework::Executor::process_job(uint64_t job_identifier) {
    if ((job_identifier <= 0) || (job_identifier > total_number_of_jobs)) {
        LOG_ERROR("Invalid job id(%ld).\n", (job_identifier - 1));
        return FAILURE;
    }
    Job *p_current_job = (Job *) (p_jobs + (job_identifier - 1));

    if (!p_current_job) {
        LOG_ERROR("Job cannot be found with (%ld).\n", (job_identifier - 1));
        return FAILURE;
    }

    print_job(job_identifier);

    return pipeline_framework::Executor::state_action_var[p_current_job->current_state].action_function((void *) p_current_job);
}

void pipeline_framework::Executor::print_job(uint64_t job_identifier) {
    if ((job_identifier <= 0) || (job_identifier > total_number_of_jobs)) {
        LOG_ERROR("Invalid job id(%ld).\n", (job_identifier - 1));
        return;
    }
    Job *p_current_job = (Job *) (p_jobs + (job_identifier - 1));

    if (!p_current_job) {
        LOG_ERROR("Job cannot be found with (%ld).\n", (job_identifier - 1));
        return;
    }
    LOG_DEBUG("job_id=%ld,"
            "current_state=%d"
            "local_ipv4=0x%08x"
            "local_port=%d"
            "remote_domain_name=%s"
            "remote_ipv4=0x%08x"
            "socket_file_descriptor=%d"
            "iteration_count=%ld"
            "\n",
            (job_identifier - 1),
            p_current_job->current_state,
            p_current_job->local_ipv4,
            p_current_job->local_port,
            p_current_job->remote_domain_name,
            p_current_job->remote_ipv4,
            p_current_job->socket_file_descriptor,
            p_current_job->iteration_count);

}

#define PROTO_ENT_VAL 6

/**
 * socket_creator A call back function to create and bind a TCP socket.
 * @param p_job_details
 * @return
 */
int pipeline_framework::Executor::socket_creator(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;
    int optval; /* flag value for setsockopt */
    struct sockaddr_in clientaddr; /* client addr */

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    //webclient::WebClient::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor > 0) {
        LOG_NOTICE("Closing socket with fd=%d.\n", p_job->socket_file_descriptor);
        close(p_job->socket_file_descriptor);
        p_job->socket_file_descriptor = 0;
    }

    p_job->socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (p_job->socket_file_descriptor < 0) {
        LOG_DEBUG("Error opening socket.\n");
        return FAILURE;
    } else {
        LOG_DEBUG("Successfully opened socket with fd=%d.\n", p_job->socket_file_descriptor);
    }
    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    optval = 1;
    setsockopt(p_job->socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
            (const void *) &optval, sizeof (int));

    /*
     * build the server's Internet address
     */
    bzero((char *) &clientaddr, sizeof (clientaddr));

    /* this is an Internet address */
    clientaddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    //clientaddr.sin_addr.s_addr = htonl(p_job->local_ipv4);
    memcpy(&clientaddr.sin_addr.s_addr, &p_job->local_ipv4, sizeof (in_addr_t));

    /* this is the port we will listen on */
    clientaddr.sin_port = htons((unsigned short) p_job->local_port);

    /*
     * bind: associate the parent socket with a port
     */
    if (bind(p_job->socket_file_descriptor, (struct sockaddr *) &clientaddr,
            sizeof (struct sockaddr_in)) < 0) {

        LOG_ERROR("ERROR on binding.socket=%d,local_port=%d\n",
                p_job->socket_file_descriptor,
                p_job->local_port);

        perror("bind failed. Error");
        return FAILURE;
    } else {
        LOG_DEBUG("Successfully binded ip=%08x:%08x,port=%d for fd=%d.\n",
                p_job->local_ipv4,
                clientaddr.sin_addr.s_addr,
                p_job->local_port,
                p_job->socket_file_descriptor);
    }
    return SUCCESS;
}

/**
 * socket_connect Performs a TCP connect to the dest IP.
 * @param p_job_details
 * @return
 */
int pipeline_framework::Executor::socket_connect(void *p_job_details) {
    //VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
    Job *p_job = (Job *) p_job_details;
    struct sockaddr_in serveraddr;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    //webclient::WebClient::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    //serveraddr.sin_addr.s_addr=htonl(p_job->remote_ipv4);
    memcpy(&serveraddr.sin_addr.s_addr, &p_job->remote_ipv4, sizeof (in_addr_t));

    uint16_t server_port = 80;
    serveraddr.sin_port = htons(server_port);

    /* connect: create a connection with the server */
    int return_code = connect(p_job->socket_file_descriptor,
            (struct sockaddr *) &serveraddr,
            sizeof (struct sockaddr_in));

    if (return_code < 0) {
        LOG_ERROR("fd=%d,ERROR connecting server ip=%08x:%08x.rc=%d\n",
                p_job->socket_file_descriptor,
                p_job->remote_ipv4,
                serveraddr.sin_addr.s_addr,
                return_code);
        perror("connect failed. Error");
        return FAILURE;
    } else {
        LOG_DEBUG("Successfully connected to remote ip=%d for fd=%d.\n",
                p_job->remote_ipv4,
                p_job->socket_file_descriptor);
    }
    return SUCCESS;
}

#define MAX_REQUEST_LEN 200

/**
 * socket_writer This call back fn writes a HTTP GET request to the TCP socket.
 * @param p_job_details
 * @return
 */
int pipeline_framework::Executor::socket_writer(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;
    ssize_t nbytes_total, nbytes_last;
    char request[MAX_REQUEST_LEN];
    int request_len = 0;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    //webclient::WebClient::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }

    /* Send HTTP request. */
    nbytes_total = 0;
    memset(request, 0, MAX_REQUEST_LEN);

    if (strlen(p_job->remote_domain_name) < MAX_REQUEST_LEN) {
#if 0
        request_len = snprintf(static_cast<char *> (request),
                static_cast<size_t> (MAX_REQUEST_LEN),
                static_cast<const char *> (request_template),
                static_cast<char *> (p_job->remote_domain_name));
#endif
        request_len = sprintf(request,
                "GET / HTTP/1.1\r\nHost: %s\r\n\r\n",
                p_job->remote_domain_name);
    }

    if (request_len >= MAX_REQUEST_LEN) {
        LOG_ERROR("request length large: %d\n", request_len);
        return FAILURE;
    }

    while (nbytes_total < request_len) {
        nbytes_last = write(p_job->socket_file_descriptor,
                request + nbytes_total,
                request_len - nbytes_total);
        if (nbytes_last == -1) {
            LOG_ERROR("write failed.\n");
            return FAILURE;
        } else {
            LOG_DEBUG("Successfully wrote %s to remote ip=%s for fd=%d.\n",
                    request,
                    p_job->remote_domain_name,
                    p_job->socket_file_descriptor);
        }
        nbytes_total += nbytes_last;
    }
    return SUCCESS;
}

/**
 * socket_reader This call back fn reads and extracts 200 OK for the HTTP WEB GET request.
 */
int pipeline_framework::Executor::socket_reader(void *p_job_details) {
    ssize_t nbytes_total = 0;
    char buffer[BUFSIZ];
    Job *p_job = (Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    //webclient::WebClient::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }
    //VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);

    while ((nbytes_total = read(p_job->socket_file_descriptor,
            buffer,
            BUFSIZ)) > 0) {
        //fprintf(stderr, "debug: after a read\n");
        //write(STDOUT_FILENO, buffer, nbytes_total);
        //LOG_DEBUG("Read Length=%ld %s \n", nbytes_total, buffer);
    }
    //fprintf(stderr, "debug: after last read\n");

    if (nbytes_total == -1) {
        LOG_ERROR("read failed.\n");
        return FAILURE;
        //exit(EXIT_FAILURE);
    } else {
        LOG_DEBUG("Successfully read from remote ip=%s for fd=%d.\n",
                p_job->remote_domain_name,
                p_job->socket_file_descriptor);
    }
    return SUCCESS;
}

/**
 * socket_destroyer This call back fn destroys the TCP socket.
 * @param p_job_details
 * @return
 */
int pipeline_framework::Executor::socket_destroyer(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    //webclient::WebClient::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }

    close(p_job->socket_file_descriptor);

    LOG_DEBUG("Successfully closed socket towards remote ip=%s for fd=%d.\n",
            p_job->remote_domain_name,
            p_job->socket_file_descriptor);

    p_job->socket_file_descriptor = 0;


    return SUCCESS;
}