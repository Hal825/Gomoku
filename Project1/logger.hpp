#ifndef __MY_LOGGER_H__
#define __MY_LOGGER_H__

#include <stdio.h>
#include <time.h>



#define INF 0
#define DEBUG 1
#define ERROR 2
#define DEFAULT_LOG_LEVEL DEBUG
#define LOG(format,...) do{\
    if(DEFAULT_LOG_LEVEL > DEBUG) break;\
    __time_t t = time(NULL);\
    struct tm* tm_info = localtime(&t);\
    char time_str[32];\
    strftime(time_str, sizeof(time_str)-1, "%Y-%m-%d %H:%M:%S", tm_info);\
    fprintf(stdout,"[%s %s:%d]" format "\n",time_str,__FILE__,__LINE__,##__VA_ARGS__);\
}while(0) 

#define ILOG(format,...) LOG("[INF]" format,##__VA_ARGS__)
#define DLOG(format,...) LOG("[DEBUG]" format,##__VA_ARGS__)
#define ELOG(format,...) LOG("[ERROR]" format,##__VA_ARGS__)

#endif  