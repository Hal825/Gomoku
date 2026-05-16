#pragma once

#include <iostream>
#include <string>
#include "UserManager.hpp"
#include "Mutex.hpp"

class Route
{
public:
    Route():_uma(std::make_unique<UserManager>())
    {}
    void ChceckUser(const InetAddr& addr)
    {
        LockGuard lockguard(_lock);
        _uma->AddUser(addr);
    }
    void OffLine(const InetAddr& addr)
    {
        LockGuard lockguard(_lock);
        _uma->DelUser(addr);
    }
    void BroadCast(int sockfd,std::string message)
    {
        LockGuard lockguard(_lock);
        auto& users = _uma->GetUsers();
        for(auto& user:users)
        {
            sendto(sockfd,message.c_str(),sizeof(message),0,(struct sockaddr*)user.GetAddress(),user.Len());
        }
    }
    ~Route()
    {}
private:
    std::unique_ptr<UserManager> _uma;
    Mutex _lock;
};
