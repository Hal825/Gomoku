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
    TcpServer(Handler_t handler,uint16_t port = gport)
        : _port(port),
          _listensocket(std::make_unique<TcpSocket>()),
          _handler(handler)
    {
        _listensocket->BuildTcpSocketMethod(_port);
        LOG(LogLevel::INFO) << "create listen socket success: "<<_listensocket->Sockfd();
    }
    void loop()
    {
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
        std::string inbuffer,outbuffer;
        while (true)
        {
            outbuffer.clear();
            int n = sockfd->Recv(&inbuffer);
            if(n<=0)
            {
                LOG(LogLevel::WARNING) << "recv: client quit! " << clientaddr.ToString();
                break;
            }
            LOG(LogLevel::DEBUG) << "inbuffer: \n"<<inbuffer;
            if(_handler)
            {
                outbuffer+=_handler(inbuffer);
            }
            if(outbuffer.empty())
            {
                continue;
            }

            LOG(LogLevel::DEBUG) << "outbuffer: \n"<<outbuffer;
            n = sockfd->Send(outbuffer);

            if(n<0)
            {
                LOG(LogLevel::WARNING) << "send: client quit! ";
                break;
            }
        }
    }

private:
    Handler_t _handler;
    uint16_t _port;
    std::unique_ptr<Socket> _listensocket;
};