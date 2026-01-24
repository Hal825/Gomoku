#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <functional>

enum{
    PIPE_ERROR,
    OK = 0,
    FORK_ERROR
};

const int grocessnum  = 5;
using cb_t = std::function<void(int)>;

void DoTask(int fd)//子进程做什么任务,参数是子进程读/写的入口
{
    while(true)
    {
        // ssize_t n = read(fd);
        sleep(1);
    }
}

class ProcessPoll
{
public:
    void Init(cb_t cb)
    {
        CreateProcessChannel(cb);
    }
private:
    class Channel{
    public:
        Channel(int wfd,pid_t pid):_wfd(wfd) , _sub_pid(pid){
            _sub_name = "sub-channel-"+std::to_string(_sub_pid);
        };
        ~Channel(){};
    private:
        int _wfd;//进程的读下标
        pid_t _sub_pid;
        std::string _sub_name; 
    };
private:
    void CreateProcessChannel(cb_t cb)
    {
        for (int i = 0; i < grocessnum; i++)
        {
            // 创建多管道和多进程;
            int pipefd[2];
            int n = pipe(pipefd);
            if (n < 0)
            {
                std::cerr << "pipe create error" << std::endl;
                exit(PIPE_ERROR);
            }
            pid_t id = fork();
            // if,elseif处理子进程，else运行父进程
            if (id < 0)
            {
                std::cerr << "fork error" << std::endl;
                exit(FORK_ERROR);
            }
            else if (n == 0)
            {
                close(pipefd[1]);
                DoTask(pipefd[0]);
                exit(OK);
            }
            else
            {
                close(pipefd[0]);
                channels.emplace_back(pipefd[1], id);
                std::cout << "创建子进程: " << id << " 成功..." << std::endl;
                sleep(1);
            }
        }
    }
private:
    std::vector<Channel> channels;
};

int main()
{
    ProcessPoll pp;
    pp.Init(DoTask);
    return 0;
}