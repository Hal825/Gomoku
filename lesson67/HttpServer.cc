#include "TcpServer.hpp"
#include "Http.hpp"
#include <memory>

static void Usage(const std::string &proc)
{
    std::cout<<"Usage:\n\t" << proc << " server_port"<<std::endl;
}


void Login(HttpRequest &req, HttpResponse &resp)
{

}
void Register(HttpRequest &req, HttpResponse &resp)
{

}
void Search(HttpRequest &req, HttpResponse &resp)
{

}

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(1);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);

    //2.定义协议对象
    std::unique_ptr<HttpProtocol> protocol = std::make_unique<HttpProtocol>();
    protocol->RegisterService("/Login",Login);
    protocol->RegisterService("/Register",Register);
    protocol->RegisterService("/Search",Search);

    //3.定义网络对象
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer)
        {
            return protocol->HandlerHttpRequest(inbuffer);
        },port
    );
    
    tsvr->Loop();
    return 0;
}
