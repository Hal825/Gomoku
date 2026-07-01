#ifndef __JSON_UTIL_HPP__
#define __JSON_UTIL_HPP__

#include <jsoncpp/json/json.h>
#include <string>
#include <sstream>
#include <memory>
#include "utils/logger.hpp"

class json_util
{
public:
    // 序列化：Json::Value -> std::string
    static bool serialize(const Json::Value &root, std::string &str)
    {
        Json::StreamWriterBuilder swb;
        std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
        std::stringstream ss;
        int ret = sw->write(root, &ss);
        if (ret != 0)
        {
            ERR_LOG("JSON 序列化失败");
            return false;
        }
        str = ss.str();
        return true;
    }
    // 反序列化：std::string -> Json::Value
    static bool unserialize(const std::string &str, Json::Value &root)
    {
        Json::CharReaderBuilder crb;
        std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
        bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), &root, nullptr);
        if (!ret)
        {
            ERR_LOG("JSON 反序列化失败: %s", str.c_str());
            return false;
        }
        return true;
    }
};

#endif
