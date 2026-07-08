#include <jsoncpp/json/forwards.h>
#include <vector>
#include "online.hpp"
#include <cstdint>
#include "user_table.hpp"

#define BOARD_ROW 15
#define BOARD_COL 15

typedef enum
{
    GAME_START,
    FAME_OVER
} room_statu;
class room
{
private:
    uint64_t _room_id;
    room_statu _statu;
    int _player_count;
    uint64_t _white_id;
    uint64_t _back_id;
    user_table *_tb_user;
    online_manager *_online_user;
    std::vector<std::vector<int>> _board;

public:
    room(uint64_t room_id, user_table *tb_user, online_manager *online_user)
        : _room_id(room_id), _statu(GAME_START), _player_count(0),
          _tb_user(tb_user), _online_user(online_user), _board(BOARD_ROW, std::vector<int>(BOARD_COL, 0))
    {
        DBG_LOG("%lu 房间创建成功！", _room_id);
    }

    ~room()
    {
        DBG_LOG("%lu 房间销毁成功！", _room_id);
    }

    uint64_t get_id()
    {
        return _room_id;
    }

    room_statu get_statu()
    {
        return _statu;
    }

    int get_player_count()
    {
        return _player_count;
    }

    void add_white_id(uint64_t uid)
    {
        _white_id = uid;
        _player_count++;
    }

    uint64_t get_white_id()
    {
        return _white_id;
    }

    void add_back_id(uint64_t uid)
    {
        _back_id = uid;
        _player_count++;
    }

    uint64_t get_back_id()
    {
        return _back_id;
    }
    
    // 处理下棋
    void handle_chess(Json::Value &req)
    {
    }
    // 处理聊天
    void handle_chat(Json::Value &req)
    {
    }
    // 处理玩家退出
    void handle_exit(uint64_t uid)
    {
    }
    // 处理广播
    void broadcast(Json::Value &rsp)
    {
    }
    void handle_request(Json::Value &rsp)
    {
    }
};