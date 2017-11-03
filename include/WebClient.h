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

namespace webclient {

    class WebClientFactory {
    private:
        WebClientFactory();
        void print_job_details(void *p_job_details);
        static WebClientFactory *p_web_client_factory;
    public:
        static int socket_creator(void *p_job_details);
        static int socket_connect(void *p_job_details);
        static int socket_writer(void *p_job_details);
        static int socket_reader(void *p_job_details);
        static int socket_destroyer(void *p_job_details);
        static WebClientFactory *Instance();
    };
}


#endif /* WEBCLIENT_H */

