#pragma once

#include <iostream>
#include <vector>
#include "InetAddr.hpp"
#include "Logger.hpp"

using namespace NS_LOG_MODULE;

class UserManager
{
public:
    void AddUser(const InetAddr& addr)
    {
        if(!SearchUser(addr))
        _users.push_back(addr);
        return ;
    }
    void DelUser(const InetAddr& addr)
    {
        for(auto user = _users.begin();user!= _users.end();user++)
        {
            if(*user == addr)
            {
                _users.erase(user);
            }
        }
    }
    bool SearchUser(const InetAddr& addr)
    {
        for(auto user:_users)
        {
            if(user==addr)
            return true;
        }
        return false;
    }
    bool ModUser(const InetAddr& addr)
    {
        DelUser(addr);
        AddUser(addr);
        return true;
    }
    std::vector<InetAddr>& GetUsers()
    {
        return _users;
    }
private:
    std::vector<InetAddr> _users;
};