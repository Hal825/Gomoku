#ifndef __USER_TABLE_HPP__
#define __USER_TABLE_HPP__

#include <string>
#include <mutex>
#include <cassert>
#include <mysql/mysql.h>
#include "logger.hpp"
#include "mysql_util.hpp"
#include "json_util.hpp"


class user_table{
private:
    MYSQL* _mysql;          // MySQL 连接句柄
    std::mutex _mutex;      // 互斥锁，保证数据库操作线程安全

public:
    // 构造函数：建立连接，失败则终止程序
    user_table(const std::string& host,
               const std::string& username,
               const std::string& password,
               const std::string& dbname,
               uint16_t port = 3306
            ){
        _mysql = mysql_util::mysql_create(host, username, password, dbname, port);
        assert(_mysql != nullptr);
        INF_LOG("user_table 初始化成功");

    }
     // 析构函数：释放连接
    ~user_table() {
        mysql_util::mysql_destroy(_mysql);
        _mysql = nullptr;
    }

    bool insert(Json::Value& user){
        if(user["username"].isNull() || user["password"].isNull())
        {
             ERR_LOG("注册信息不完整");
            return false;
        }

        Json::Value tmp;
        if(select_by_name(user["username"].asString(),tmp))
        {
            ERR_LOG("用户名已存在: %s", user["username"].asCString());
                return false;
        }

        char sql[4096] = {0};
        snprintf(sql,sizeof(sql),
                "INSERT INTO user VALUES(NULL, '%s', MD5('%s'), 1000, 0, 0);",
                user["username"].asCString(),
                user["password"].asCString());

        std::lock_guard<std::mutex> lock(_mutex);
        bool ret = mysql_util::mysql_exec(_mysql,sql);
        if (!ret) {
            ERR_LOG("插入用户失败: %s", user["username"].asCString());
            return false;
        }
        INF_LOG("注册成功: %s", user["username"].asCString());
        return true;
    }


    bool login(Json::Value& user) {

        if (user["username"].isNull() || user["password"].isNull()) {
            ERR_LOG("登录信息不完整");
            return false;
        }

        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),
                 "SELECT id, score, total_count, win_count FROM user "
                 "WHERE username='%s' AND password=MD5('%s');",
                 user["username"].asCString(),
                 user["password"].asCString());

        std::lock_guard<std::mutex> lock(_mutex);
        MYSQL_RES* res = mysql_util::mysql_query_store(_mysql, sql);
        if (res == nullptr) {
            ERR_LOG("查询失败或用户名密码错误");
            return false;
        }

        int rows = mysql_num_rows(res);
        if (rows != 1) {
            mysql_free_result(res);
            ERR_LOG("用户名或密码错误");
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"]          = (Json::UInt64)std::stol(row[0]);
        user["username"]    = user["username"];  // 保留原始用户名
        user["score"]       = (Json::UInt64)std::stol(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"]   = std::stoi(row[3]);

        mysql_free_result(res);
        INF_LOG("登录成功: %s (id=%lu)", user["username"].asCString(), user["id"].asUInt64());
        return true;
    }

    bool select_by_name(const std::string& name, Json::Value& user)
    {
        
    }
}