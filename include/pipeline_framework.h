/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   namespace.h
 * Author: ssridhar
 *
 * Created on October 11, 2017, 1:06 PM
 */
#ifndef NAMESPACE_H
#define NAMESPACE_H

#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h> /* getprotobyname */
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Log.h"
#include <errno.h> //For errno - the error number

#include <map>
#include <vector>

#include <sys/timeb.h>
#include <string>
#include <syscall.h>

namespace pipeline_framework {

    using namespace std;

    typedef unsigned char uint8_t;
    typedef unsigned int uint32_t;
    typedef unsigned long uint64_t;

#define SUCCESS 1
#define FAILURE 2

#ifndef WEB_CLIENT
#define WEB_CLIENT
#endif

    extern int is_webclient_alive();

}

#endif

