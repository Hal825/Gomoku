#include "../include/logger.hpp"
#include "../include/user_table.hpp"

int main() {
    // 创建 user_table 实例（注意替换密码）
    user_table users("127.0.0.1", "root", "@Lzs0825", "online_gobang", 3306);

    // 测试注册
    Json::Value new_user;
    new_user["username"] = "testuser";
    new_user["password"] = "123456";
    if (users.insert(new_user)) {
        INF_LOG("注册测试成功");
    } else {
        ERR_LOG("注册测试失败（可能用户名已存在）");
    }

    // 测试登录
    Json::Value login_user;
    login_user["username"] = "xiaobai";
    login_user["password"] = "123";
    if (users.login(login_user)) {
        INF_LOG("登录测试成功: id=%lu, score=%lu, total=%d, win=%d",
                login_user["id"].asUInt64(),
                login_user["score"].asUInt64(),
                login_user["total_count"].asInt(),
                login_user["win_count"].asInt());
    } else {
        ERR_LOG("登录测试失败");
    }

    // 测试查询用户信息
    Json::Value user_info;
    if (users.select_by_name("xiaobai", user_info)) {
        INF_LOG("查询用户成功: %s, 积分=%lu", 
                user_info["username"].asCString(),
                user_info["score"].asUInt64());
    }

    return 0;
}