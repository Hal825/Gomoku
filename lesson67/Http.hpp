#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include "Logger.hpp"

const std::string linesep = "\r\n";
const std::string spacesep = " ";
const std::string headersep = ": ";
const std::string g_http_version = "HTTP/1.0";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwroot";
const std::string page_404 = "./wwwroot/404.html";
const std::string suffixsep = ".";

using namespace NS_LOG_MODULE;

class Util
{
public:
    static std::string ReadLine(std::string &str)
    {
        auto pos = str.find(linesep);
        if (pos == std::string::npos)
            return std::string();
        std::string line = str.substr(0, pos);
        str.erase(0, line.size() + linesep.size()); // 只有空行也是移除
        if (line.empty())
            return linesep;
        return line;
    }

    static std::string ReadFile(const std::string &filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            return std::string();
        }
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size <= 0)
            return std::string();
        std::string buffer;
        buffer.resize(static_cast<size_t>(size));

        if (!file.read(&buffer[0], size))
        {
            throw std::runtime_error("Failed to read file: " + filename);
        }

        return buffer;
    }
};

class HttpRequest
{
private:
    bool ParseReqLine(std::string &httpstr)
    {
        std::string req_line = Util::ReadLine(httpstr);
        if (req_line.empty() || req_line == linesep)
            return false;
        std::stringstream ss(req_line);
        ss >> _method >> _uri >> _version;

        // 修复：如果URI不以/开头，添加/
        if (!_uri.empty() && _uri[0] != '/')
        {
            _uri = "/" + _uri;
        }

        if (_uri == "/")
            _uri += g_first_page;

        _uri = g_wwwroot + _uri;

        auto pos = _uri.rfind(suffixsep);
        if (pos == std::string::npos)
            _suffix = ".html";
        else
            _suffix = _uri.substr(pos);
        return true;
    }
    bool ParseHeaderkv(std::string &httpstr)
    {
        // // std::string header_line = Util::ReadLine(httpstr);
        // while (true)
        // {
        //     std::string header_line = Util::ReadLine(httpstr);
        //     if (header_line.empty())
        //         return false;
        //     if (header_line == linesep)
        //         break;
        //     if (header_line != linesep)
        //     {
        //         auto pos = header_line.find(headersep);
        //         if (pos == std::string::npos)
        //             return false;
        //         std::string key = header_line.substr(0, pos);
        //         std::string value = header_line.substr(pos + headersep.size());
        //         _header[key] = value;
        //         // _header.insert(std::make_pair(key,value));
        //         // _header.insert({key,value});
        //     }
        // }
        while (true)
        {
            std::string header_line = Util::ReadLine(httpstr);

            // 如果读取失败（没有更多数据）
            if (header_line.empty())
            {
                // 没有数据了，头部结束
                break;
            }

            // 如果是空行，头部结束
            if (header_line == linesep)
            {
                break;
            }

            // 解析header
            auto pos = header_line.find(headersep);
            if (pos == std::string::npos)
            {
                LOG(LogLevel::ERROR) << "Invalid header format: " << header_line;
                return false;
            }

            std::string key = header_line.substr(0, pos);
            std::string value = header_line.substr(pos + headersep.size());
            _header[key] = value;
            LOG(LogLevel::DEBUG) << "Header: " << key << " = " << value;
        }
        return true;
    }

public:
    bool Deserialize(std::string &httpstr)
    {
        // // std::cout << httpstr;

        // 1.解析请求行
        // bool n = ParseReqLine(httpstr);
        // (void)n;

        // // LOG(LogLevel::DEBUG) << "_method# " << _method;
        // // LOG(LogLevel::DEBUG) << "_uri# " << _uri;
        // // LOG(LogLevel::DEBUG) << "_version# " << _version;

        // // std::cout << httpstr;

        // // 2.解析报文
        // n = ParseHeaderkv(httpstr);
        // if (n)
        // {
        //     for (const auto &header : _header)
        //         std::cout << header.first << headersep << header.second << "\r\n";
        // }
        // // std::cout << "start|" << httpstr << "|end" << "\r\n";
        // // return true;
        // // 1.解析请求行
        if (!ParseReqLine(httpstr))
        {
            LOG(LogLevel::ERROR) << "Failed to parse request line";
            return false;
        }

        // 2.解析报文头
        if (!ParseHeaderkv(httpstr))
        {
            LOG(LogLevel::ERROR) << "Failed to parse headers";
            return false;
        }

        // 3. 剩余的内容就是body（如果有的话）
        if (!httpstr.empty())
        {
            _text = httpstr;
            LOG(LogLevel::DEBUG) << "Body size: " << _text.size();
        }
        // 添加调试日志
        LOG(LogLevel::DEBUG) << "Parse success - Method: " << _method
                             << ", URI: " << _uri
                             << ", Suffix: " << _suffix;
        LOG(LogLevel::DEBUG) << "Headers parsed successfully, count: " << _header.size();
        return true;
    }

    std::string Uri()
    {
        return _uri;
    }
    std::string RequestContent()
    {
        return Util::ReadFile(_uri);
    }
    std::string Suffix()
    {
        return _suffix;
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
    std::string _suffix;
};



class HttpResponse
{
private:
    std::string CodeToDesc(int code)
    {
        std::string code_desc;
        switch (code)
        {
        // 1xx: 信息响应
        case 100:
            code_desc = "Continue";
            break;
        case 101:
            code_desc = "Switching Protocols";
            break;

        // 2xx: 成功
        case 200:
            code_desc = "OK";
            break;
        case 201:
            code_desc = "Created";
            break;
        case 204:
            code_desc = "No Content";
            break;

        // 3xx: 重定向
        case 301:
            code_desc = "Moved Permanently";
            break;
        case 302:
            code_desc = "Found";
            break;
        case 304:
            code_desc = "Not Modified";
            break;

        // 4xx: 客户端错误
        case 400:
            code_desc = "Bad Request";
            break;
        case 401:
            code_desc = "Unauthorized";
            break;
        case 403:
            code_desc = "Forbidden";
            break;
        case 404:
            code_desc = "Not Found";
            break;
        case 405:
            code_desc = "Method Not Allowed";
            break;
        case 429:
            code_desc = "Too Many Requests";
            break;

        // 5xx: 服务器错误
        case 500:
            code_desc = "Internal Server Error";
            break;
        case 502:
            code_desc = "Bad Gateway";
            break;
        case 503:
            code_desc = "Service Unavailable";
            break;
        case 504:
            code_desc = "Gateway Timeout";
            break;

        // 默认情况：未知状态码
        default:
            code_desc = "Unknown Status Code";
            break;
        }
        return code_desc;
    }

public:
    HttpResponse(std::string version = g_http_version)
        : _version(version),
          _code(0),
          _blank_line(linesep)
    {
    }
    void SetCode(int code)
    {
        _code = code;
        _code_desc = CodeToDesc(_code);
    }
    void SetBody(const std::string &content)
    {
        _text = content;
    }
    int BodySize()
    {
        return _text.size();
    }
    // bool Build(HttpRequest &http_req)
    // {
    //     std::string target_file = http_req.Uri();
    //     _text = Util::ReadFile(target_file);
    //     if (_text.empty())
    //     {
    //         _code = 404;
    //         return false;
    //     }
    //     else
    //     {
    //         _code = 200;
    //         _code_desc = "OK";
    //         return true;
    //     }
    // }
    void AddHeader(const std::string &key, const std::string &value)
    {
        _header[key] = value;
    }
    std::string Serialize()
    {
        // 构建状态行
        std::string resptr = _version;
        resptr += spacesep;
        resptr += std::to_string(_code);
        resptr += spacesep;
        resptr += _code_desc;
        resptr += linesep;

        // 构建报头
        for (auto &header : _header)
        {
            std::string line = header.first + headersep + header.second + linesep;
            resptr += line;
        }

        // 添加换行符
        resptr += linesep;

        // 添加正文
        resptr += _text;
        return resptr;
    }
    ~HttpResponse() {}

private:
    std::string _version;
    int _code;
    std::string _code_desc;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
};


using service_t = std::function<void (HttpRequest &req,HttpResponse &resp)>;

class HttpProtocol
{
private:
    void InitMimeTypes()
    {

        // 文本类型
        mimeTypes[".txt"] = "text/plain";
        mimeTypes[".html"] = "text/html";
        mimeTypes[".htm"] = "text/html";
        mimeTypes[".css"] = "text/css";
        mimeTypes[".js"] = "application/javascript";
        mimeTypes[".json"] = "application/json";
        mimeTypes[".xml"] = "application/xml";

        // 图片类型
        mimeTypes[".jpg"] = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".png"] = "image/png";
        mimeTypes[".gif"] = "image/gif";
        mimeTypes[".bmp"] = "image/bmp";
        mimeTypes[".svg"] = "image/svg+xml";
        mimeTypes[".webp"] = "image/webp";
        mimeTypes[".ico"] = "image/x-icon";

        // 文档类型
        mimeTypes[".pdf"] = "application/pdf";
        mimeTypes[".doc"] = "application/msword";
        mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        mimeTypes[".xls"] = "application/vnd.ms-excel";
        mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
        mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

        // 音视频类型
        mimeTypes[".mp3"] = "audio/mpeg";
        mimeTypes[".mp4"] = "video/mp4";
        mimeTypes[".avi"] = "video/x-msvideo";
        mimeTypes[".mov"] = "video/quicktime";
        mimeTypes[".wav"] = "audio/wav";
        mimeTypes[".flac"] = "audio/flac";
        mimeTypes[".ogg"] = "audio/ogg";

        // 压缩文件
        mimeTypes[".zip"] = "application/zip";
        mimeTypes[".rar"] = "application/x-rar-compressed";
        mimeTypes[".7z"] = "application/x-7z-compressed";
        mimeTypes[".tar"] = "application/x-tar";
        mimeTypes[".gz"] = "application/gzip";

        // 可执行文件
        mimeTypes[".exe"] = "application/x-msdownload";
        mimeTypes[".sh"] = "application/x-sh";
    }

public:
    HttpProtocol()
    {
        InitMimeTypes();
    }

    void RegisterService(const std::string &uri, service_t service)
    {
        _http_services[uri] = service;
    }

    std::string SuffixToMimeType(const std::string &suffix)
    {
        auto iter = mimeTypes.find(suffix);
        if (iter != mimeTypes.end())
            return iter->second;
        else
            return "text/html";
    }
    std::string HandlerHttpRequest(std::string &req)
    {
        // 1.保证报文完整性----IGN

        // 2.序列化
        // std::cout<<"Handler Pid: "<<getpid()<<std::endl;
        HttpRequest http_req;
        http_req.Deserialize(req);

        // std::cout<<"Suffix:  "<<http_req.Suffix()<<std::endl;

        // 3.根据不同的请求方法和uri给用户返回不同的报文
        HttpResponse http_resp;

        std::string content = http_req.RequestContent();

        if (content.empty())
        {
            // 404
            //  http_resp.SetCode(404);
            //  http_resp.SetBody(Util::ReadFile(page_404));
            //  http_resp.AddHeader("Content-Length",std::to_string(http_resp.BodySize()));

            http_resp.SetCode(302);
            http_resp.AddHeader("Location", "/404.html");
        }
        else
        {
            http_resp.SetCode(200);
            http_resp.AddHeader("Content-Length", std::to_string(content.size()));
            http_resp.AddHeader("Content-Type", SuffixToMimeType(http_req.Suffix()));
            http_resp.AddHeader("Connection", "close");
            http_resp.SetBody(content);
        }

        // http_resp.Build(http_req);

        return http_resp.Serialize();

        // std::cout<<"############################"<<std::endl;
        // std::cout<<req<<std::endl;

        // std::string status_line = "HTTP/1.1 200 OK\r\n";
        // status_line += "\r\n";

        // std::string html = "<html><body><h1>hello world!<h1><body><html>";

        // return status_line + html;
    }
    ~HttpProtocol() {}

private:
    std::unordered_map<std::string, std::string> mimeTypes;
    std::unordered_map<std::string, service_t> _http_services;
};
