#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static void Usage(const std::string& proc)
{
    std::cout <<"Usage:\n\t";
    std::cout << proc << " server_ip server_port" <<std::endl;

}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    // const std::string server_ip = argv[1];
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    //1.创建socket
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
    }
   // 2. 构建server端socket信息 
    // client 需要有自己的IP和Port信息吗? 需要的！
    // 需要显示的bind自己的ip和端口吗？不要显示bind！！！
    // 1. 为什么不让client显示bind？client bind port 出现冲突！client port只需要具有唯一性即可，具体是几，不重要。
    // 2. 如何设置自己的IP和端口呢？client 一般会采用随机端口的方式！由OS自主选择！
    // udp client 首次发送数据的时候，OS底层会隐式自动帮你进行获取随机端口，然后bind + Port + IP
    struct sockaddr_in server;
    memset(&server,0,sizeof(server));
    // socklen_t len = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());

    while(true)
    {
        std::string message;
        // 1.获取用户输入
        std::cout<<"Please Enter##";
        std::getline(std::cin,message);

        //2.client发送数据给server
        // char inbuffer[1024] = {0};
        
        ssize_t n = sendto(sockfd,message.c_str(),message.size(),0,(struct sockaddr*)&server,sizeof(server));
        if(n>0)
        {
            //recvfrom
            char inbuffer[1024] = {0};
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            ssize_t m = recvfrom(sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr*)&temp,&len);
            if(m>0)
            {
                inbuffer[m] = 0;
                std::cout << inbuffer << std::endl;
            }
            else
            {
                std::cout<<"recvform error";
            }
        }
        else{
                std::cout<<"sendto server error";
        }
    }
    return 0;
}