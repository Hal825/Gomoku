#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>
#include "Logger.hpp"
#include <functional>

using namespace NS_LOG_MODULE;

// Request报文
class Request
{
public:
    Request()
        : _data_x(0),
          _data_y(0),
          _oper(0) {}
    Request(int x, int y, int op)
        : _data_x(x),
          _data_y(y),
          _oper(op)
    {
    }
    bool Serialize(std::string *out)
    {
        // 防御性检查：输出指针不能为空
        if (out == nullptr)
        {
            std::cerr << "错误：输出字符串指针为空！" << std::endl;
            return false;
        }

        try
        {
            Json::Value root;
            root["left"] = _data_x;
            root["right"] = _data_y;
            root["oper"] = _oper;

            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

            std::stringstream ss;
            // 序列化操作（可能抛出异常的核心步骤）
            sw->write(root, &ss);
            *out = ss.str();

            return true;
        }
        // 捕获所有标准异常，避免程序崩溃
        catch (const std::exception &e)
        {
            std::cerr << "序列化异常：" << e.what() << std::endl;
            return false;
        }
        // if (out == nullptr)
        // {
        //     return false;
        // }
        // Json::Value root;
        // root["left"] = _data_x;
        // root["right"] = _data_y;
        // root["mid"] = _oper;

        // Json::StreamWriterBuilder swb;
        // std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

        // std::stringstream ss;
        // sw->write(root, &ss);
        // *out = ss.str();
        // return true;
    }
    bool Deserialize(std::string &in)
    {
        // if(in==nullptr) return false;引用永远不为空
        if (in.empty())
        {
            std::cerr << "反序列化失败：输入字符串为空" << std::endl;
            return false;
        }
        try
        {
            Json::Value root;
            Json::Reader reader;
            bool parsesuccess = reader.parse(in, root);
            if (!parsesuccess)
            {
                std::cerr << "反序列化失败:JSON 格式错误" << std::endl;
                return false; // 失败返回，成功读数据
            }
            _data_x = root["left"].asInt();
            _data_y = root["right"].asInt();
            _oper = root["oper"].asInt();
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "反序列化异常：" << e.what() << std::endl;
            return false;
        }
    }
    int Get_data_x()
    {
        return _data_x;
    }
    int Get_data_y()
    {
        return _data_y;
    }
    char Get_oper()
    {
        return _oper;
    }
    ~Request() {}

private:
    int _data_x;
    int _data_y;
    char _oper;
};

// Response报文
class Response
{
public:
    Response()
        : _result(0),
          _code(0)
    {
    }
    Response(int result, int code)
        : _result(result),
          _code(code)
    {
    }
    bool Serialize(std::string *out)
    {
        // / 空指针检查（防御性编程
        if (out == nullptr)
        {
            return false;
        }

        Json::Value root;
        root["result"] = _result;
        root["code"] = _code;
        try
        {
            // 简化写法：替代手动创建 StreamWriter
            Json::StreamWriterBuilder swb;
            // swb["indentation"] = ""; // 无缩进，生成紧凑的 JSON 字符串（可选）
            *out = Json::writeString(swb, root);
            return true;
        }
        catch (const std::exception &e)
        {
            // 捕获序列化异常，避免程序崩溃
            (void)e; // 可根据需要打印日志
            return false;
        }
    }
    bool Deserialize(std::string &in)
    {
        // if(in==nullptr) return false;引用永远不为空
        if (in.empty())
        {
            std::cerr << "反序列化失败：输入字符串为空" << std::endl;
            return false;
        }
        try
        {
            Json::Value root;
            Json::Reader reader;
            bool parsesuccess = reader.parse(in, root);
            if (!parsesuccess)
            {
                std::cerr << "反序列化失败:JSON 格式错误" << std::endl;
                return false; // 失败返回，成功读数据
            }
            _result = root["result"].asInt();
            _code = root["code"].asInt();
            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "反序列化异常：" << e.what() << std::endl;
            return false;
        }
    }
    void Modify_result(int r)
    {
        _result = r;
    }
    void Modify_code(int c)
    {
        _code = c;
    }
    int Get_result()
    {
        return _result;
    }
    int Get_code()
    {
        return _code;
    }
    ~Response() {}

private:
    int _result; // 结果
    int _code;   // 状态码
};

const static std::string gsep = "\r\n";
using HandlerRequest_t = std::function<Response(Request &)>;
using HandlerResponse_t = std::function<void (Response &)>;

class Protocol
{
public:
    Protocol(HandlerRequest_t handler_request)
        : _version1("1.0"),
          _handler_request(handler_request)
    {}
    Protocol(HandlerResponse_t handler_response)
        : _version1("1.0"),
          _handler_response(handler_response)
    {}
    Protocol(){}
    std::string Packet(std::string &json_string)
    {
        return std::to_string(json_string.size()) + gsep + json_string + gsep;
    }
    //>0:success;
    //=0:no error,but nullpr;
    //<0:error;
    int Unpack(std::string &packet, std::string *json_string)
    {
        if (packet.empty())
            return 0;
        if (json_string == nullptr)
            return -1;

        // 分析报文
        int pos = packet.find(gsep);
        if (pos == std::string::npos)
        {
            return 0;
        }
        // 提取json串长度&判断是否存在>=1的完整报文
        std::string lenstr = packet.substr(0, pos);
        int len = std::stoi(lenstr);
        int total = lenstr.size() + 2 * gsep.size() + len;
        if (packet.size() < total) // 如果已经序列化的字符串不满足>=1个字串完整
        {
            return 0;
        }
        // 提取报文
        *json_string = packet.substr(pos + gsep.size(), len);
        // 删除报文&返回正确结果
        packet.erase(0, total);
        return 1;
    }

    // 如果有多个报文，我们需要循环处理

    std::string ParseRequst(std::string &inbuffer)
    {
        std::string result;
        while (true)
        {
            // 1.解包
            std::string json_string;
            int n = Unpack(inbuffer, &json_string);
            if (n < 0)
            {
                LOG(LogLevel::DEBUG) << "no way!";

                return std::string();
            }
            if (n == 0)
            {
                LOG(LogLevel::INFO) << inbuffer << " parse done! ";
                return result;
            }
            LOG(LogLevel::DEBUG)<<"json_string:\n"<<json_string;
            LOG(LogLevel::DEBUG)<<"Unpack done,inbuffer:\n"<<inbuffer;
            // 2.反序列化
            Request req;
            if (!req.Deserialize(json_string))
            {
                return std::string();
            }
            // 3.业务计算，交给response
            Response resp;
            if (_handler_request)
                resp = _handler_request(req);
            // 4.序列化
            std::string resp_json_string;
            resp.Serialize(&resp_json_string);
            // 5.添加报头
            result += Packet(resp_json_string);
        }
        return result;
    }
    std::string ParseResponse(std::string &inbuffer)
    {
        while (true)
        {
            // 1.解包
            std::string json_string;
            int n = Unpack(inbuffer, &json_string);
            if (n < 0)
            {
                LOG(LogLevel::DEBUG) << "no way!";

                return std::string();
            }
            if (n == 0)
            {
                LOG(LogLevel::INFO) << inbuffer << " parse done! ";
                return std::string();
            }
            // 2.反序列化
            Response resp;
            if (!resp.Deserialize(json_string))
            {
                return std::string();
            }
            // 3.回调处理
            if (_handler_response)
                _handler_response(resp);
            // // 4.序列化
            // std::string resp_json_string;
            // resp.Serialize(&resp_json_string);
            // // 5.添加报头
            // result += Packet(resp_json_string);
            // return result;
        }
    }
    ~Protocol() {}

private:
    HandlerRequest_t _handler_request;
    HandlerResponse_t _handler_response;
    std::string _version1;
};
