#include "ThreadPool.hpp"//执行者
#include "Route.hpp"
#include "UdpServer.hpp"//消息
#include <memory>


using namespace NS_THREAD_POOL_MODULE;

using task_t = std::function<void()>;

void Usage(const std::string& process)
{
    std::cerr << "Usage:\n\t";
    std::cerr <<process <<"local_ip local_port"<<std::endl;
}

int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);
    // std::string server_ip = argv[1];

    // 1.线程池模块
    auto thread_pool = ThreadPool<task_t>::Instance();

    //2.路由模块
    Route r;

    //3.网络模块
    UdpServer usvr(server_port);
    usvr.Init();
    usvr.RegisterService(
        [&r](const InetAddr& addr)
        {
            r.ChceckUser(addr);
        },
        [&r,thread_pool](int sockfd,std::string msg)
        {
            auto t = std::bind(&Route::BroadCast,&r,sockfd,msg);
            thread_pool->Enqueue(t);

        //     thread_pool->Enqueue([&r,&sockfd,&msg]()     第二种写法
        //     {
        //         r.BroadCast(sockfd,msg);
        //     }
        // );
        }
        // [&r](int sockfd,std::string msg)
        // {
        //     r.BroadCast(sockfd,msg);
        // }
    );
    usvr.Start();


    // std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(server_port);
    // usvr->Init();
    // usvr->Start();



    return 0;
}