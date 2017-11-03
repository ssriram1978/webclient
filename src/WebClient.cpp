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

#include "WebClient.h"
#include "Log.h"
#include "Job.h"

webclient::WebClientFactory* webclient::WebClientFactory::p_web_client_factory = NULL;

/**
 * WebClientFactory Constructor
 */
webclient::WebClientFactory::WebClientFactory() {

}

/**
 * Instance Returns Web Client Factory instance.
 * @return pointer to web client factory.
 */
webclient::WebClientFactory *webclient::WebClientFactory::Instance() {
    if (!webclient::WebClientFactory::p_web_client_factory) {
        webclient::WebClientFactory::p_web_client_factory = new webclient::WebClientFactory();
    }
    return webclient::WebClientFactory::p_web_client_factory;
}

/**
 * print_job_details Prints the job details.
 * @param p_job_details
 */
void webclient::WebClientFactory::print_job_details(void *p_job_details) {
    webclient::Job *p_job = (webclient::Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("%s:%s:%d Invalid job details.\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    p_job->print_Job();
}

#define PROTO_ENT_VAL 6

/**
 * socket_creator A call back function to create and bind a TCP socket.
 * @param p_job_details
 * @return
 */
int webclient::WebClientFactory::socket_creator(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;
    int optval; /* flag value for setsockopt */
    struct sockaddr_in clientaddr; /* client addr */

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    webclient::WebClientFactory::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor > 0) {
        LOG_NOTICE("Closing socket with fd=%d.\n", p_job->socket_file_descriptor);
        p_job->socket_file_descriptor = 0;
    }

    p_job->socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("Error opening socket.\n");
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
        LOG_ERROR("ERROR on binding.%d\n", p_job->socket_file_descriptor);
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
int webclient::WebClientFactory::socket_connect(void *p_job_details) {
    //VLOG_NOTICE("%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
    Job *p_job = (Job *) p_job_details;
    struct sockaddr_in serveraddr;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    webclient::WebClientFactory::Instance()->print_job_details(p_job_details);

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
int webclient::WebClientFactory::socket_writer(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;
    ssize_t nbytes_total, nbytes_last;
    char request[MAX_REQUEST_LEN];
    char request_template[] = "GET / HTTP/1.1\r\nHost: %s\r\n\r\n";
    int request_len = 0;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    webclient::WebClientFactory::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }

    /* Send HTTP request. */
    nbytes_total = 0;
    memset(request, 0, MAX_REQUEST_LEN);

    if (strlen(p_job->remote_domain_name) < MAX_REQUEST_LEN) {
        request_len = snprintf(static_cast<char *> (request),
                static_cast<size_t> (MAX_REQUEST_LEN),
                static_cast<const char *> (request_template),
                static_cast<char *> (p_job->remote_domain_name));
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
int webclient::WebClientFactory::socket_reader(void *p_job_details) {
    ssize_t nbytes_total = 0;
    char buffer[BUFSIZ];
    Job *p_job = (Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    webclient::WebClientFactory::Instance()->print_job_details(p_job_details);

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
int webclient::WebClientFactory::socket_destroyer(void *p_job_details) {
    Job *p_job = (Job *) p_job_details;

    if (!p_job) {
        LOG_ERROR("Invalid input parameters.\n");
        return FAILURE;
    }

    webclient::WebClientFactory::Instance()->print_job_details(p_job_details);

    if (p_job->socket_file_descriptor < 0) {
        LOG_ERROR("socket_file_descriptor is less than 0.\n");
        return FAILURE;
    }

    close(p_job->socket_file_descriptor);
    return SUCCESS;
}