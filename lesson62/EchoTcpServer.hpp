#pragma once
//#pragma once 是 C/C++ 中的预处理指令，
//核心作用是：保证当前头文件（.h/.hpp）在整个编译过程中只被编译一次，避免因头文件重复包含导致的 “重定义错误”。
#include <iostream>
#include <cstring>
#include <unistd.h>
//网络三件套
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

using namespace NS_LOG_MODULE;

enum 
{
    SUCCESS = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR
};

const static int gbacklog = 16;
static const uint16_t gport = 8888;

class TcpServer
{
public:
    TcpServer(uint16_t port = gport):_port(port)
    {}
    void InitServer()
    {
        //1.创建socket
        _listensockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_listensockfd<0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG) << "create socket success";
        //2.填充本地网络信息
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        // local.sin_port = _port;   bug发现点##错误：没有转换端口号,操操操,serverbind不到8080会bind随机值
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        //3.bind
        int n = bind(_listensockfd,(struct sockaddr*)&local,sizeof(local));
        if(n<0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "bind success";

        //4.listen 返回的是服务员(多->多)，_sockfd是接待员（一个->多个）
        n = listen(_listensockfd,gbacklog);
        if(n<0)
        {
            LOG(LogLevel::FATAL) << "LISTEN error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "listen success";

    }
//     void InitServer()
// {
//     // 添加详细的调试输出
//     std::cout << "\n========== DEBUG INFO ==========" << std::endl;
//     std::cout << "1. _port value in InitServer: " << _port << std::endl;
    
//     _listensockfd = socket(AF_INET,SOCK_STREAM,0);
//     if(_listensockfd<0)
//     {
//         std::cout << "2. socket failed: " << strerror(errno) << std::endl;
//         LOG(LogLevel::FATAL) << "create socket error";
//         exit(SOCKET_ERR);
//     }
//     std::cout << "2. socket success, fd: " << _listensockfd << std::endl;
    
//     struct sockaddr_in local;
//     memset(&local,0,sizeof(local));
//     local.sin_family = AF_INET;
//     local.sin_port = htons(_port);
//     local.sin_addr.s_addr = INADDR_ANY;
    
//     std::cout << "3. Attempting to bind to port: " << _port 
//               << " (network order: " << ntohs(local.sin_port) << ")" << std::endl;
    
//     int n = bind(_listensockfd,(struct sockaddr*)&local,sizeof(local));
//     if(n<0)
//     {
//         std::cout << "4. bind FAILED: " << strerror(errno) << std::endl;
//         LOG(LogLevel::FATAL) << "bind error";
//         exit(BIND_ERR);
//     }
//     std::cout << "4. bind returned success" << std::endl;
    
//     // 检查实际绑定的端口
//     struct sockaddr_in bound_addr;
//     socklen_t bound_len = sizeof(bound_addr);
//     getsockname(_listensockfd, (struct sockaddr*)&bound_addr, &bound_len);
//     std::cout << "5. ACTUALLY bound to port: " << ntohs(bound_addr.sin_port) << std::endl;
    
//     n = listen(_listensockfd,gbacklog);
//     if(n<0)
//     {
//         std::cout << "6. listen failed: " << strerror(errno) << std::endl;
//         LOG(LogLevel::FATAL) << "LISTEN error";
//         exit(LISTEN_ERR);
//     }
//     std::cout << "6. listen success" << std::endl;
//     std::cout << "================================\n" << std::endl;
// }

    void serverIO(int sockfd,InetAddr& address)
    {
        LOG(LogLevel::DEBUG) <<"client info is : "<<address.ToString();
        while(true)
        {
            char inbuffer[1024] = {0};
            //读
            ssize_t n = read(sockfd,inbuffer,sizeof(inbuffer)-1);
            if(n>0)
            {
                inbuffer[n] = 0;
                LOG(LogLevel::INFO) <<address.ToString()<<" say#" << inbuffer;

                std::string echo_string = "server echo# ";
                echo_string+=inbuffer;

                write(sockfd,echo_string.c_str(),echo_string.size());
            }
            else if(n==0)
            {
                LOG(LogLevel::INFO) <<"client quit, address : "<<address.ToString();
                break;
            }
            else{
                LOG(LogLevel::ERROR) <<"client read error, address : "<<address.ToString();
                break;
            }
        }
    }

    void Start()
    {
        while (true)
        {
            //5.创建连接
            struct sockaddr_in clientaddr; 
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd,(struct sockaddr*)&clientaddr,&len);
            if(sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error!";
                continue;
            }
            LOG(LogLevel::DEBUG) << "accept success! sockfd: " << sockfd;

            //6.处理sockfd
            InetAddr clientaddress(clientaddr);//bug发现点,没有传入clientaddr导致client的connect失败
            serverIO(sockfd,clientaddress);
            close(sockfd);
        }
        
    }
    ~TcpServer(){}
private:
    int _listensockfd;
    uint16_t _port;
};