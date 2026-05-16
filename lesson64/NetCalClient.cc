#include <iostream>
#include <memory>
#include "Socket.hpp"
#include "Protocol.hpp"
#include "InetAddr.hpp"

using namespace NS_SOCKET_MODULE;

static void Usage(const std::string &proc)
{
    std::cout<<"Usage:\n\t" << proc << " server_ip, server_port"<<std::endl;
}

static void HandlerResponse(Response &resp)
{
    std::cout<<"Calculate Result: "<< resp.Get_result() << " [" << resp.Get_code()<<"]" << std::endl;
}

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    std::unique_ptr<Socket> socket = std::make_unique<TcpSocket>();
    socket->BuildTcpClientSocketMethod();
    InetAddr serveraddress(server_port,server_ip);
    bool n = socket->Connect(serveraddress);
    if(!n)
    {
        std::cerr << "connect error: " << serveraddress.ToString() << std::endl;
        exit(2);
    }
    Protocol protocol(HandlerResponse);
    std::string inbuffer;
    while(true)
    {
        // 0.获取数据
        int x,y;
        char oper;
        std::cout << " Please Enter: ";
        std::cin>>x;
        std::cout << " Please Enter: ";
        std::cin>>y;
        std::cout << " Please Enter: ";
        std::cin>>oper;

        //1.定义请求对象
        Request req(x,y,oper);
        //2.序列化
        std::string req_json_string;
        req.Serialize(&req_json_string);

        //3.分包
        std::string send_json_string = protocol.Packet(req_json_string);

        //4.发送
        socket->Send(send_json_string);

        //5.接收
        socket->Recv(&inbuffer);

        //6.
        protocol.ParseResponse(inbuffer);
    }
    return 0;
}
