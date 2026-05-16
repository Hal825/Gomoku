#ifndef _ECHOSERVER_HPP
#define _ECHOSERVER_HPP

#include <iostream>
#include <string.h>
#include <cstdlib>
// 网络通信必备的三个头文件
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//回调函数
#include <functional>

#include "Logger.hpp"
#include "InetAddr.hpp"


using namespace NS_LOG_MODULE;
// using callback_t = std::function<std::string(std::string)>;

using handler_addr_t = std::function<void(const InetAddr&)>;
using handler_msg_t = std::function<void(int sokcfd, std::string msg)>;


const static int default_fd = -1;
const static int default_port = 8888;

enum
{
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR
};

class UdpServer
{
public:
    // UdpServer(const std::string& ip,uint16_t port = default_port)
    // UdpServer(callback_t cb,uint16_t port = default_port)
    UdpServer(uint16_t port = default_port)
    :_sockfd(default_fd),
    //  _cb(cb),
    //  _ip(ip),
     _port(port)
     {}
    void Init(){
        //第一步：创建socket，打开网卡

        _sockfd = socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd<0){
            LOG(LogLevel::FATAL)<<" create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO)<<" create socket success";

        // 第二步：填充网络信息
        InetAddr local(_port);
        // struct sockaddr_in local;
        // bzero(&local,sizeof(local));
        
        // local.sin_family = AF_INET;
        // // local.sin_addr.s_addr = inet_addr(_ip.c_str());
        // local.sin_addr.s_addr = INADDR_ANY;
        // local.sin_port = htons(_port);

        // 第三步：bind socket信息
        int n = bind(_sockfd,(struct sockaddr *)local.GetAddress(),local.Len());
        if(n<0)
        {
            LOG(LogLevel::FATAL)<<" bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO)<<" bind socket success" <<", port:" << _port;
        
    }
    void RegisterService(handler_addr_t handler_addr,handler_msg_t handler_msg)
    {
        _handler_addr = handler_addr;
        _handler_msg = handler_msg;
    }
    void Start()
    {
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
         //数据报 读recv(返回实际读到多少个字节，-1失败)
            ssize_t n = recvfrom (_sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr*)&peer,&len);

            if(n>0)
            {
                inbuffer[n] = 0;
                // 1.检测新用户
                InetAddr clientaddr(peer);
                std::string tips = clientaddr.ToString();
                std::string message = tips+inbuffer;//[ip:port]-nickname# message
                LOG(LogLevel::DEBUG) << message;
                _handler_addr(clientaddr);


                // 2.转发消息
                _handler_msg(_sockfd,message);

                // InetAddr addr(peer);
                // // uint16_t client_port = ntohs(peer.sin_port);
                // // std::string client_ip = inet_ntoa(peer.sin_addr); // 4字节IP-> ntoh -> 字符串
                // // std::string client_address = "[" + client_ip + ":" + std::to_string(client_port) + "]# ";

                // inbuffer[n] = 0;
                // std::string result = _cb(inbuffer);
                // // LOG(LogLevel::DEBUG)<<client_address<< inbuffer;
                // // std::string echo_string = "server echo#";
                // // echo_string +=inbuffer;
                // sendto(_sockfd,result.c_str(),result.size(),0,(struct sockaddr*)&peer,len);

            }
            else{
                LOG(LogLevel::ERROR)<<"recvform error";

            }
        }
    }
    ~UdpServer()
    {
        close(_sockfd);
    }
private:
    int _sockfd;
    // std::string _ip;
    uint16_t _port;
    // callback_t _cb;
    handler_addr_t _handler_addr;
    handler_msg_t _handler_msg;
};




#endif 