#include "../include/logger.hpp"
#include "../include/mysql_util.hpp"
#include "../include/json_util.hpp"

int main() {
    // 测试 JSON 序列化
    Json::Value test;
    test["name"] = "zhangsan";
    test["age"] = 18;
    test["score"].append(88.5);
    test["score"].append(99.0);

    std::string json_str;
    if (json_util::serialize(test, json_str)) {
        INF_LOG("序列化成功: %s", json_str.c_str());
    }

    // 测试 JSON 反序列化
    Json::Value parsed;
    if (json_util::unserialize(json_str, parsed)) {
        INF_LOG("反序列化成功: name=%s, age=%d", 
                parsed["name"].asCString(), parsed["age"].asInt());
    }

    return 0;
}