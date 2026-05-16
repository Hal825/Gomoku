#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "InetAddr.hpp"
#include <errno.h>

void Usage(const std::string &name)
{
    std::cerr << "Usage:\n\t";
    std::cerr << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    // 1.创建tcpsocket套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "create socket error" << std::endl;
        exit(2);
    }
    // 2.要不要bind(要),bind要不要显示bind(不要，交由OS完成：随机分配port)
    InetAddr serveraddress(server_port, server_ip);
    int n = connect(sockfd, (struct sockaddr *)serveraddress.GetAddress(), serveraddress.Len());
    // if (n < 0)
    // {
    //     std::cerr << "connect to" << serveraddress.ToString() << "failed!";
    //     exit(3);
    // }
    if (n < 0)
    {
        std::cerr << "connect to " << serveraddress.ToString()
                  << " failed! errno: " << errno
                  << " (" << strerror(errno) << ")" << std::endl;
        exit(3);
    }
    std::cerr << "connect to" << serveraddress.ToString() << "success!";

    // 3.通信
    while (true)
    {
        std::string line;
        std::cout << "Please Enter# ";
        std::getline(std::cin, line);

        write(sockfd, line.c_str(), line.size());

        char inbuffer[1024] = {0};
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
        if (n > 0)
        {
            inbuffer[n] = 0;
            std::cout << inbuffer << std::endl;
        }
        else if (n == 0)
        {
            std::cout << "read enf of file!" << std::endl;
            break;
        }
        else
        {
            std::cerr << "read error!";
            break;
        }
    }
    return 0;
}