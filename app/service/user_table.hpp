#ifndef __USER_TABLE_HPP__
#define __USER_TABLE_HPP__

#define INSERT_USER "INSERT INTO user VALUES(NULL, '%s', MD5('%s'), 100, 0, 0);"
#define INSERT_LOGIN "SELECT id, score, total_count, win_count FROM user ""WHERE username='%s' AND password=MD5('%s');"
#define SELECT_NAME "SELECT id, score, total_count, win_count FROM user WHERE username='%s';"
#define SELECT_ID "SELECT username, score, total_count, win_count FROM user WHERE id=%lu;"
#define UPDATE_WIN "UPDATE user SET score=score+30, total_count=total_count+1, win_count=win_count+1 WHERE id=%lu;"
#define UPDATE_LOSE "UPDATE user SET score=score-30, total_count=total_count+1 WHERE id=%lu;"

#include <string>
#include <mutex>
#include <cassert>
#include <mysql/mysql.h>
#include "utils/logger.hpp"
#include "utils/mysql_util.hpp"
#include "utils/json_util.hpp"

class user_table
{
private:
    MYSQL *_mysql;     // MySQL 连接句柄
    std::mutex _mutex; // 互斥锁，保证数据库操作线程安全

public:
    // 构造函数：建立连接，失败则终止程序
    user_table(const std::string &host,
               const std::string &username,
               const std::string &password,
               const std::string &dbname,
               uint16_t port = 3306)
    {
        _mysql = mysql_util::mysql_create(host, username, password, dbname, port);
        assert(_mysql != nullptr);
        INF_LOG("user_table 初始化成功");
    }
    // 析构函数：释放连接
    ~user_table()
    {
        mysql_util::mysql_destroy(_mysql);
        _mysql = nullptr;
    }

    // 注册时新增用户
    bool insert(Json::Value &user)
    {
        if (user["username"].isNull() || user["password"].isNull())
        {
            ERR_LOG("注册信息不完整");
            return false;
        }

        Json::Value tmp;
        if (select_by_name(user["username"].asString(), tmp))
        {
            ERR_LOG("用户名已存在: %s", user["username"].asCString());
            return false;
        }

        char sql[4096] = {0};
        snprintf(sql, sizeof(sql), INSERT_USER,
                 user["username"].asCString(),
                 user["password"].asCString());

        std::lock_guard<std::mutex> lock(_mutex);
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (!ret)
        {
            ERR_LOG("插入用户失败: %s", user["username"].asCString());
            return false;
        }
        INF_LOG("注册成功: %s", user["username"].asCString());
        return true;
    }
    // 登陆验证,并返回详细的用户信息
    bool login(Json::Value &user)
    {

        if (user["username"].isNull() || user["password"].isNull())
        {
            DBG_LOG("登录信息不完整");
            return false;
        }

        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),INSERT_LOGIN,
                 user["username"].asCString(),
                 user["password"].asCString());

        //查询用户数据,并且只能返回一条,因为user_name是唯一的.
        std::lock_guard<std::mutex> lock(_mutex);
        MYSQL_RES *res = mysql_util::mysql_query_store(_mysql, sql);
        if (res == nullptr)
        {
            DBG_LOG("查询失败或用户名密码错误");
            return false;
        }

        int rows = mysql_num_rows(res);
        if (rows != 1)
        {
            mysql_free_result(res);
            DBG_LOG("用户不存在或查询结果不唯一");
            return false;
        }

        //取出这行的所有用户数据
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["username"] = user["username"]; // 保留原始用户名
        user["score"] = (Json::UInt64)std::stol(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);

        mysql_free_result(res);
        INF_LOG("登录成功: %s (id=%lu)", user["username"].asCString(), user["id"].asUInt64());
        return true;
    }

    // 通过用户名获取信息
    bool select_by_name(const std::string &name, Json::Value &user)
    {
        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),SELECT_NAME, name.c_str());

        std::lock_guard<std::mutex> lock(_mutex);
        MYSQL_RES *res = mysql_util::mysql_query_store(_mysql, sql);
        if (res == nullptr)
        {
            ERR_LOG("查询用户失败: %s", name.c_str());
            return false;
        }

        int rows = mysql_num_rows(res);

        if (rows != 1)
        {
            mysql_free_result(res);
            ERR_LOG("用户不存在或查询结果不唯一: %s", name.c_str());
            return false;
        }


        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["username"] = name;
        user["score"] = (Json::UInt64)std::stol(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);

        mysql_free_result(res);
        return true;
    }

    // 通过用户id获取信息
    bool select_by_id(uint64_t id, Json::Value &user)
    {
        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),SELECT_ID,id);

        std::lock_guard<std::mutex> lock(_mutex);
        MYSQL_RES *res = mysql_util::mysql_query_store(_mysql, sql);
        if (res == nullptr)
        {
            ERR_LOG("查询用户失败: id=%lu", id);
            return false;
        }

        int rows = mysql_num_rows(res);
        if (rows != 1)
        {
            mysql_free_result(res);
            ERR_LOG("用户不存在或查询结果不唯一: id=%lu", id);
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)id;
        user["username"] = row[0];
        user["score"] = (Json::UInt64)std::stol(row[1]);
        user["total_count"] = std::stoi(row[2]);
        user["win_count"] = std::stoi(row[3]);

        mysql_free_result(res);
        return true;
    }
    // 5. 胜利：积分+30，总场次+1，胜场+1
    bool win(uint64_t id)
    {
        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),UPDATE_WIN,id);

        std::lock_guard<std::mutex> lock(_mutex);
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (!ret)
        {
            ERR_LOG("更新胜利信息失败: id=%lu", id);
            return false;
        }
        INF_LOG("玩家 %lu 胜利，积分+30", id);
        return true;
    }

    // 6. 失败：积分-30，总场次+1（胜场不变）
    bool lose(uint64_t id)
    {
        char sql[4096] = {0};
        snprintf(sql, sizeof(sql),UPDATE_LOSE,id);

        std::lock_guard<std::mutex> lock(_mutex);
        bool ret = mysql_util::mysql_exec(_mysql, sql);
        if (!ret)
        {
            ERR_LOG("更新失败信息失败: id=%lu", id);
            return false;
        }
        INF_LOG("玩家 %lu 失败，积分-30", id);
        return true;
    }
};
#endif
