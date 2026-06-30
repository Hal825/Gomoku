#ifndef __MYSQL_UTIL_HPP__
#define __MYSQL_UTIL_HPP__

#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include "logger.hpp"

class mysql_util
{
public:
    // 创建 MySQL 连接
    static MYSQL *mysql_create(
        const std::string &host,
        const std::string &user,
        const std::string &pass,
        const std::string &db,
        uint16_t port = 3306)
    {
        MYSQL *mysql = mysql_init(nullptr);
        if (mysql == nullptr)
        {
            ERR_LOG("mysql_init failed!");
            return nullptr;
        }

        // 设置连接超时（5秒）
        int timeout = 5;
        mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        mysql_options(mysql, MYSQL_OPT_READ_TIMEOUT, &timeout);
        mysql_options(mysql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);

        // 设置自动重连（忽略弃用警告，暂时保留）
        // 注意：MYSQL_OPT_RECONNECT 在 MySQL 8.0 中已弃用，但暂时可用
        bool reconnect = true;
        mysql_options(mysql, MYSQL_OPT_RECONNECT, &reconnect);

        if (mysql_real_connect(mysql, host.c_str(), user.c_str(),
                               pass.c_str(), db.c_str(), port, nullptr, 0) == nullptr)
        {
            ERR_LOG("mysql_real_connect failed: %s", mysql_error(mysql));
            mysql_close(mysql);
            return nullptr;
        }

        // 设置字符集
        if (mysql_set_character_set(mysql, "utf8") != 0)
        {
            ERR_LOG("mysql_set_character_set failed: %s", mysql_error(mysql));
            mysql_close(mysql);
            return nullptr;
        }

        // 测试连接是否有效
        if (mysql_ping(mysql) != 0)
        {
            ERR_LOG("mysql_ping failed: %s", mysql_error(mysql));
            mysql_close(mysql);
            return nullptr;
        }

        INF_LOG("MySQL 连接成功: %s@%s:%d/%s", user.c_str(), host.c_str(), port, db.c_str());
        return mysql;
    }

    // 执行 SQL 语句（无结果集，如 INSERT/UPDATE/DELETE）
    static bool mysql_exec(MYSQL *mysql, const std::string &sql)
    {
        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            ERR_LOG("mysql_query failed: %s", mysql_error(mysql));
            ERR_LOG("SQL: %s", sql.c_str());
            return false;
        }
        return true;
    }

    // 执行查询语句并返回结果集（需要手动释放）
    static MYSQL_RES *mysql_query_store(MYSQL *mysql, const std::string &sql)
    {
        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            ERR_LOG("mysql_query failed: %s", mysql_error(mysql));
            ERR_LOG("SQL: %s", sql.c_str());
            return nullptr;
        }
        MYSQL_RES *res = mysql_store_result(mysql);
        if (res == nullptr)
        {
            ERR_LOG("mysql_store_result failed: %s", mysql_error(mysql));
            return nullptr;
        }
        return res;
    }

    // 释放连接
    static void mysql_destroy(MYSQL *mysql)
    {
        if (mysql != nullptr)
        {
            mysql_close(mysql);
            INF_LOG("MySQL 连接已释放");
        }
    }
};

#endif