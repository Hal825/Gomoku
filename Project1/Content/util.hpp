#ifndef _M_UTIL_H_
#define _M_UTIL_H_
#include "logger.hpp"
#include <string>
#include <mysql/mysql.h>

class mysql_util{
    public:
        static MYSQL *mysql_create(const std::string &host,
            const std::string &user,
            const std::string &passwd,
            const std::string &db,
            unsigned int port = 3306){
        }
};
#endif