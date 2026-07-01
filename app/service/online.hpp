#include <unordered_map>
#include <mutex>
#include "utils/logger.hpp"
#include "utils/mysql_util.hpp"
#include "utils/json_util.hpp"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

// 固定写法：给websocket服务端起别名
typedef websocketpp::server<websocketpp::config::asio> websocket_server;

class online_manager{
private:
    std::mutex _mutex;
    //用于建立游戏大厅用户的用户ID与通信连接的关系
    std::unordered_map<uint64_t, websocket_server::connection_ptr> _hall_user;
    //用于建立游戏房间用户的用户ID与通信连接的关系
    std::unordered_map<uint64_t, websocket_server::connection_ptr> _room_user;
public:
    online_manager(){}
    //websocket连接建立的时候才会加入游戏大厅&游戏房间在线用户管理
    void enter_game_hall(uint64_t uid, websocket_server::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _hall_user.insert(std::make_pair(uid,conn));

    }
    bool enter_game_room(uint64_t uid, websocket_server::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _room_user.insert(std::make_pair(uid,conn));
    }

    //websocket连接断开的时候，才会移除游戏大厅&游戏房间在线用户管理
    bool exit_game_hall(uint64_t uid, websocket_server::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _hall_user.erase(uid);
    }
    bool exit_game_room(uint64_t uid, websocket_server::connection_ptr &conn)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _room_user.erase(uid);
    }

    //判断当前指定用户是否在游戏大厅/游戏房间
    bool is_in_game_hall(uint64_t uid){
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _hall_user.find(uid);
        if(it == _hall_user.end())
        {
            return false;
        }
        return true;
    }
    bool is_in_game_room(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _room_user.find(uid);
        if(it == _room_user.end())
        {
            return false;
        }
        return true;
    }

    //通过用户ID在游戏大厅/游戏房间用户管理中获取对应的通信连接
    websocket_server::connection_ptr get_conn_from_hall(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _hall_user.find(uid);
        if(it == _hall_user.end())
        {
            return websocket_server::connection_ptr();
        }
        return it->second;
    }
    websocket_server::connection_ptr get_conn_from_room(uint64_t uid)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        auto it = _room_user.find(uid);
        if(it == _room_user.end())
        {
            return websocket_server::connection_ptr();
        }
        return it->second;
    }
};