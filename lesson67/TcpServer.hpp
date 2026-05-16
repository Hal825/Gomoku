#pragma once

#include "Logger.hpp"
#include "InetAddr.hpp"
#include "Socket.hpp"
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <functional>

using namespace NS_SOCKET_MODULE;
using Handler_t = std::function<std::string(std::string &)>;

static int gport = 8080;

class TcpServer
{
public:
    TcpServer(Handler_t handler, uint16_t port = gport)
        : _port(port),
          _listensocket(std::make_unique<TcpSocket>()),
          _handler(handler)
    {
        _listensocket->BuildTcpSocketMethod(_port);
        LOG(LogLevel::INFO) << "create listen socket success: " << _listensocket->Sockfd();
    }
    void Loop()
    {
        // 默认情况下，父进程如果不处理 SIGCHLD 信号，子进程退出后会变成 “僵尸进程”（Zombie Process），占用系统进程表资源。
        //  而通过 signal(SIGCHLD, SIG_IGN) 告诉内核：“子进程退出不用通知我，你直接回收它的资源就行”。
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            InetAddr clientaddr;
            auto sockfd = _listensocket->Accept(clientaddr);
            if (!sockfd)
                continue;
            LOG(LogLevel::DEBUG) << "get ad new link, socket address: " << clientaddr.ToString() << " sockfd: " << sockfd->Sockfd();

            if (fork() == 0)
            {
                // child
                service(sockfd, clientaddr);
                sockfd->Close();
                exit(0);
            }
        }
    }
    ~TcpServer()
    {
    }

private:
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        // while (true)
        // {

            std::string inbuffer, outbuffer;
            outbuffer.clear();
            int n = sockfd->Recv(&inbuffer);
            if (n <= 0)
            {
                LOG(LogLevel::WARNING) << "recv: client quit! " << clientaddr.ToString();
                return;
            }
            LOG(LogLevel::DEBUG) << "inbuffer: \n"<< inbuffer;
            if (_handler)
            {
                outbuffer = _handler(inbuffer);
            }
            // if (outbuffer.empty())
            // {
            //     return;
            // }

            // LOG(LogLevel::DEBUG) << "outbuffer: \n"<< outbuffer;
            n = sockfd->Send(outbuffer);

            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "send: client quit! ";
                // return ;
            }
        // }
    }

private:
    Handler_t _handler;
    uint16_t _port;
    std::unique_ptr<Socket> _listensocket;
};
