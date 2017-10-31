/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Log.cpp
 * Author: ssridhar
 * 
 * Created on October 11, 2017, 1:06 PM
 */

#include <Log.h>


using namespace std;

static char  gProcessName[PROCESS_NAME_LEN] = "WEBCLIENT";
static char  gPid[PROCESS_ID_LEN]           = "0";

static map<string, LogLevel> loglevels;

#if 0
static char* threadId() {
    static __thread char thread_id_buf[25];
    static __thread bool firsttime = false;

    if (false == firsttime) {
        snprintf(thread_id_buf, sizeof (thread_id_buf), "[%ld] ", int64_t(::syscall(SYS_gettid)));
        firsttime = true;
    }

    return thread_id_buf;
}
#endif

static char* threadName() {
    static __thread char thread_name_buf[35];
    static __thread bool firsttime = false;

    if (false == firsttime) {
        char thread_name[25];
        pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));
        snprintf(thread_name_buf, sizeof (thread_name_buf), "[%s]", thread_name);
        firsttime = true;
    }

    return thread_name_buf;
}

char* timestamp(char* ts_buf, size_t size) {
  char tmp[6];

  ::timeb raw;
  ::tm time_info;

  ::ftime(&raw);
  ::localtime_r(&raw.time, &time_info);
  //size_t size_ts = ::strftime(ts_buf, size, "[%a %b %d %T.",&time_info);
  size_t size_tmp = std::snprintf(tmp, sizeof (tmp), "%03d]", raw.millitm);
  strncat(ts_buf, tmp, size_tmp);
  return ts_buf;
}

void initializeLogParameters(const char *pname) 
{
    string processname(pname);
    auto pos = processname.rfind('/');
    if (pos != string::npos) {
        ++pos;
    } else {
        pos = 0;
    }

    snprintf(gProcessName, PROCESS_NAME_LEN, "[%s]", processname.substr(pos).c_str());
    snprintf(gPid, PROCESS_ID_LEN, "[%d]", uint16_t(::getpid()));

    //Initialize log levels map.
    loglevels[string("EMERGENCY")] = LOGLEVEL_EMERGENCY;
    loglevels[string("INIT")] = LOGLEVEL_INIT;
    loglevels[string("ALERT")] = LOGLEVEL_ALERT;
    loglevels[string("CRITICAL")] = LOGLEVEL_CRITICAL;
    loglevels[string("ERROR")] = LOGLEVEL_ERROR;
    loglevels[string("WARN")] = LOGLEVEL_WARN;
    loglevels[string("NOTICE")] = LOGLEVEL_NOTICE;
    loglevels[string("INFO")] = LOGLEVEL_INFO;
    loglevels[string("DEBUG")] = LOGLEVEL_DEBUG;
    loglevels[string("TRACE")] = LOGLEVEL_TRACE;
    loglevels[string("VERBOSE")] = LOGLEVEL_VERBOSE;
}

void setLogLevel(const char* level)
{
    auto it = loglevels.find(level);
    if (it != loglevels.end()) {
        gCommonLogLevel = loglevels[level];
        printf("Log level set to: [%s : %d]", level, gCommonLogLevel);
    } else {
        VLOG_ERROR("Failed to set log level to %s", level);
    }
}

void logText(const char* level, const char* message)
{
    char ts_buf[26 * 3] = {0};
    std::cout << gProcessName << gPid << threadName() << timestamp(ts_buf, sizeof(ts_buf)) << level << message << std::endl;
}

void logErrorText(const char* level, const char* message)
{
    char ts_buf[26 * 3] = {0};
    std::cerr << gProcessName << gPid << threadName() << timestamp(ts_buf, sizeof(ts_buf)) << level << message << std::endl;
}