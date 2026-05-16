#pragma once

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Logger.hpp"
#include "InetAddr.hpp"

namespace NS_SOCKET_MODULE
{
    enum
    {
        OK = 0,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };
    // const static int gbacklog = 16;
    static const int gbacklog = 16;
    using namespace NS_LOG_MODULE;

    class Socket
    {
    public:
        ~Socket() {}

    protected:
        virtual void CreateSocketOrDie() = 0;
        virtual void BindSocketOrDie(uint16_t port) = 0;
        virtual void ListenSocketOrDie() = 0;
    public:
        virtual std::shared_ptr<Socket> Accept(InetAddr& clientaddr) = 0;
        virtual int Sockfd() = 0;
        virtual int Recv(std::string* out) = 0;
        virtual int Send(const std::string&in) = 0;
        virtual void Close() = 0;
        virtual bool Connect(InetAddr &serveraddr) = 0;
    public:
        void BuildTcpSocketMethod(uint16_t port)
        {
            CreateSocketOrDie();
            BindSocketOrDie(port);
            ListenSocketOrDie();
        }
        void BuildTcpClientSocketMethod()
        {
            CreateSocketOrDie();
        }
    };

    class TcpSocket : public Socket
    {
    public:
        TcpSocket():_sockfd(0)
        {}
        TcpSocket(int sockfd)
            : _sockfd(sockfd)
        {}
        void CreateSocketOrDie() override
        {
            // int sockfd = socket(AF_INET, SOCK_STREAM, 0); 注意不要习惯性的在这里创建sockfd，要用_sockfd
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << " create socket error ";
                exit(SOCKET_ERR);
            }
        }
        void BindSocketOrDie(uint16_t port) override
        {
            InetAddr addr(port);
            if (bind(_sockfd, addr.NetAddress(), addr.Len()) != 0)
            {
                LOG(LogLevel::FATAL) << " bind socket error ";
                exit(BIND_ERR);
            }
        }
        void ListenSocketOrDie() override
        {
            if (listen(_sockfd, gbacklog) != 0)
            {
                LOG(LogLevel::FATAL) << " listen socket error ";
                exit(LISTEN_ERR);
            }
        }
        std::shared_ptr<Socket> Accept(InetAddr& clientaddr) override
        {
            InetAddr addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd,CONV(&addr),&len);
            if(sockfd<0)
            {
                LOG(LogLevel::WARNING) << "accept error ";
                return nullptr;
            }
            clientaddr = addr;
            return std::make_shared<TcpSocket>(sockfd);//要将接收到accept的sockfd传过去,不然系统会使用默认构造的0
        }
        int Sockfd() override
        {
            return _sockfd;
        }
        int Recv(std::string* out) override
        {
            char inbuffer[1024];
            ssize_t n = recv(_sockfd,inbuffer,sizeof(inbuffer)-1,0);
            if(n>0)
            {
                inbuffer[n] = 0;
                *out+=inbuffer;//追加写入
            }
            return n;
        }
        int Send(const std::string&in) override
        {
            return send(_sockfd,in.c_str(),in.size(),0);
        }
        void Close() override
        {
            if(_sockfd>=0)
            {
                close(_sockfd);
                _sockfd = -1;
            }
        }
        bool Connect(InetAddr &serveraddr) override
        {
            int n = connect(_sockfd,serveraddr.NetAddress(),serveraddr.Len());
            if(n<0)
            {
                return false;
            }
            return true;
        }
        ~TcpSocket() {}

    private:
        int _sockfd;
    };
}
