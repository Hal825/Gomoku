#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <string>
#include <time.h>
#include <pthread.h>

#define INF 0
#define DBG 1
#define ERR 2

// 控制日志输出级别：设为 DBG 表示打印 INF、DBG、ERR；设为 ERR 则只打印 ERR
#define LOG_LEVEL INF

#define LOG(level, format, ...)                                                       \
    do                                                                                \
    {                                                                                 \
        if (level < LOG_LEVEL)                                                        \
            break;                                                                    \
        time_t t = time(NULL);                                                        \
        struct tm *lt = localtime(&t);                                                \
        char time_buf[32] = {0};                                                      \
        strftime(time_buf, 31, "%H:%M:%S", lt);                                       \
        fprintf(stdout, "[%p %s %s:%d] " format "\n",                                 \
                (void *)pthread_self(), time_buf, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define INF_LOG(format, ...) LOG(INF, format, ##__VA_ARGS__)
#define DBG_LOG(format, ...) LOG(DBG, format, ##__VA_ARGS__)
#define ERR_LOG(format, ...) LOG(ERR, format, ##__VA_ARGS__)

#endif