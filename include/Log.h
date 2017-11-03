/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Log.h
 * Author: ssridhar
 *
 * Created on October 26, 2017, 4:32 PM
 */

#ifndef LOG_H
#define LOG_H

//#include <stdio.h>

//#ifdef __cplusplus
//extern "C" {
//#endif

#include "namespace.h"

#define STR(x) #x
#define XSTR(x) STR(x)

typedef enum {
    LOGLEVEL_EMERGENCY = 0,
    LOGLEVEL_INIT,
    LOGLEVEL_ALERT,
    LOGLEVEL_CRITICAL,
    LOGLEVEL_ERROR,
    LOGLEVEL_WARN,
    LOGLEVEL_NOTICE,
    LOGLEVEL_INFO,
    LOGLEVEL_DEBUG,
    LOGLEVEL_TRACE,
    LOGLEVEL_VERBOSE
} LogLevel;

static LogLevel gCommonLogLevel = LOGLEVEL_ERROR;

#define PROCESS_NAME_LEN     25
#define PROCESS_ID_LEN       25

void logText(const char* level, const char* message);
void logErrorText(const char* level, const char* message);

#define __LOG_CREATE_MESSAGE(...)                                        \
  char __message[BUFSIZ+1];                    \
  int __lglen = snprintf(__message, BUFSIZ, "[" __FILE__ ":" XSTR(__LINE__) "] "  \
      __VA_ARGS__);                                                        \
  if (__lglen >= BUFSIZ)                                                  \
  {                                                                       \
      __message[BUFSIZ-1] = '\0';                                         \
      __lglen = BUFSIZ-1;                                                 \
  }

#define LOG_EMERG(...)                          \
  do {                                           \
    if (LOGLEVEL_EMERGENCY > gCommonLogLevel)    \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logErrorText("[EMERG]", __message);          \
  } while(0)

#define LOG_INIT(...)                           \
  do {                                           \
    if (LOGLEVEL_INIT > gCommonLogLevel)         \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[INIT]", __message);                \
  } while(0)

#define LOG_ALERT(...)                          \
  do {                                           \
    if (LOGLEVEL_ALERT > gCommonLogLevel)        \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logErrorText("[ALERT]", __message);          \
  } while(0)

#define LOG_CRITICAL(...)                       \
  do {                                           \
    if (LOGLEVEL_CRITICAL > gCommonLogLevel)     \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logErrorText("[CRITICAL]", __message);       \
  } while(0)

#define LOG_ERROR(...)                          \
  do {                                           \
    if (LOGLEVEL_ERROR > gCommonLogLevel)        \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logErrorText("[ERROR]", __message);          \
  } while(0)

#define LOG_WARN(...)                           \
  do {                                           \
    if (LOGLEVEL_WARN > gCommonLogLevel)         \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logErrorText("[WARN]", __message);           \
  } while(0)

#define LOG_NOTICE(...)                         \
  do {                                           \
    if (LOGLEVEL_NOTICE > gCommonLogLevel)       \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[NOTICE]", __message);              \
  } while(0)

#define LOG_INFO(...)                           \
  do {                                           \
    if (LOGLEVEL_INFO > gCommonLogLevel)         \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[INFO]", __message);                \
  } while(0)

#define LOG_DEBUG(...)                          \
  do {                                           \
    if (LOGLEVEL_DEBUG > gCommonLogLevel)        \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[DEBUG]", __message);               \
  } while(0)

#define LOG_TRACE(...)                          \
  do {                                           \
    if (LOGLEVEL_TRACE > gCommonLogLevel)        \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[TRACE]", __message);               \
  } while(0)

#define LOG_VERBOSE(...)                        \
  do {                                           \
    if (LOGLEVEL_VERBOSE > gCommonLogLevel)      \
      break;                                     \
    __LOG_CREATE_MESSAGE(__VA_ARGS__);          \
    logText("[VERBOSE]", __message);             \
  } while(0)

void initializeLogParameters(const char *processname);
void setLogLevel(const char*);

//#ifdef __cplusplus
//}
//#endif

#endif /* LOG_H */

