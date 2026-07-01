/**
 * @file test_user_table.cpp
 * @brief user_table 功能测试文件
 *
 * 测试覆盖：
 *   1. insert()       — 注册、重复注册、信息不完整
 *   2. login()        — 正确登录、错误密码、信息不完整
 *   3. select_by_name — 查询存在/不存在的用户
 *   4. select_by_id   — 查询存在/不存在的用户
 *   5. win() / lose() — 胜利/失败积分更新
 *   6. 综合流程         — 注册→登录→多场对局
 *
 * 编译方式（在项目根目录）：
 *   make test
 * 或手动：
 *   g++ -std=c++17 -g -O2 -Iinclude -I/usr/include/mysql \
 *       src/test_user_table.cpp -o test_user_table \
 *       -lmysqlclient -lzstd -lssl -lcrypto -lresolv -lm -ljsoncpp -lpthread
 */

#include "../include/logger.hpp"
#include "../include/user_table.hpp"
#include <cstring>
#include <cstdlib>
#include <string>
#include <sstream>

/* ============================================================
 *  简易测试框架
 * ============================================================ */

static int g_passed = 0;
static int g_failed = 0;

#define TEST_CASE(name)                                                        \
    do {                                                                       \
        fprintf(stdout, "\n\033[1;36m===== [TEST] %s =====\033[0m\n", name);  \
    } while (0)

#define EXPECT(cond, format, ...)                                              \
    do {                                                                       \
        if (cond) {                                                            \
            g_passed++;                                                        \
            fprintf(stdout, "  \033[1;32m[PASS]\033[0m " format "\n",         \
                    ##__VA_ARGS__);                                            \
        } else {                                                               \
            g_failed++;                                                        \
            fprintf(stdout, "  \033[1;31m[FAIL]\033[0m " format "\n",         \
                    ##__VA_ARGS__);                                            \
        }                                                                      \
    } while (0)

#define PRINT_SUMMARY()                                                        \
    do {                                                                       \
        fprintf(stdout,                                                        \
                "\n\033[1;%sm========== 测试结束: 通过 %d / 失败 %d ==========\033[0m\n", \
                (g_failed == 0) ? "32" : "31", g_passed, g_failed);           \
    } while (0)

/* ============================================================
 *  辅助：基于时间戳生成唯一测试用户名，确保每次运行相互独立
 * ============================================================ */

static std::string make_test_name(const char *prefix)
{
    std::ostringstream oss;
    oss << prefix << "_" << time(nullptr);
    return oss.str();
}

/* ============================================================
 *  测试入口
 * ============================================================ */

int main()
{
    fprintf(stdout, "\033[1;33muser_table 单元测试\033[0m\n");
    fprintf(stdout, "连接数据库...\n");

    // 创建实例（连接信息与线上保持一致）
    user_table users("127.0.0.1", "root", "@Lzs0825", "online_gobang", 3306);

    // 生成唯一的测试用户名
    const std::string NAME_A = make_test_name("__test_user");
    const std::string NAME_B = make_test_name("__test_flow");

    // ========================
    //  1. insert() 测试
    // ========================
    TEST_CASE("insert - 正常注册");
    {
        Json::Value user;
        user["username"] = NAME_A;
        user["password"] = "test123";
        bool ok = users.insert(user);
        EXPECT(ok, "注册成功: %s", user["username"].asCString());
    }

    TEST_CASE("insert - 空用户名");
    {
        Json::Value user;
        user["password"] = "123";
        bool ok = users.insert(user);
        EXPECT(!ok, "空用户名被拒绝");
    }

    TEST_CASE("insert - 空密码");
    {
        Json::Value user;
        user["username"] = NAME_A + "_empty";
        bool ok = users.insert(user);
        EXPECT(!ok, "空密码被拒绝");
    }

    TEST_CASE("insert - 重复注册（用户名已存在）");
    {
        Json::Value user;
        user["username"] = NAME_A;
        user["password"] = "another";
        bool ok = users.insert(user);
        EXPECT(!ok, "重复用户名被拒绝");
    }

    // ========================
    //  2. login() 测试
    // ========================
    TEST_CASE("login - 正常登录");
    {
        Json::Value user;
        user["username"] = NAME_A;
        user["password"] = "test123";
        bool ok = users.login(user);
        EXPECT(ok, "登录成功");
        if (ok)
        {
            EXPECT(user["id"].asUInt64() > 0, "id 有效: %lu", user["id"].asUInt64());
            EXPECT(user["score"].asUInt64() == 100, "初始积分=100 (实际=%lu)",
                   user["score"].asUInt64());
            EXPECT(user["total_count"].asInt() == 0, "初始总场次=0 (实际=%d)",
                   user["total_count"].asInt());
            EXPECT(user["win_count"].asInt() == 0, "初始胜场=0 (实际=%d)",
                   user["win_count"].asInt());
        }
    }

    TEST_CASE("login - 错误密码");
    {
        Json::Value user;
        user["username"] = NAME_A;
        user["password"] = "wrong_password";
        bool ok = users.login(user);
        EXPECT(!ok, "错误密码被拒绝");
    }

    TEST_CASE("login - 不存在的用户");
    {
        Json::Value user;
        user["username"] = NAME_A + "_nonexistent_zzz";
        user["password"] = "123";
        bool ok = users.login(user);
        EXPECT(!ok, "不存在用户被拒绝");
    }

    TEST_CASE("login - 空用户名");
    {
        Json::Value user;
        user["password"] = "123";
        bool ok = users.login(user);
        EXPECT(!ok, "空用户名被拒绝");
    }

    // ========================
    //  3. select_by_name() 测试
    // ========================
    TEST_CASE("select_by_name - 查询存在的用户");
    {
        Json::Value info;
        bool ok = users.select_by_name(NAME_A, info);
        EXPECT(ok, "查询成功");
        if (ok)
        {
            EXPECT(info["username"].asString() == NAME_A,
                   "用户名匹配: %s", info["username"].asCString());
            EXPECT(info["id"].asUInt64() > 0, "id 有效");
            EXPECT(!info["score"].isNull(), "积分字段存在");
        }
    }

    TEST_CASE("select_by_name - 查询不存在的用户");
    {
        Json::Value info;
        bool ok = users.select_by_name(NAME_A + "_nonexistent_zzz", info);
        EXPECT(!ok, "不存在的用户返回 false");
    }

    // ========================
    //  4. select_by_id() 测试
    // ========================
    TEST_CASE("select_by_id - 查询存在的用户");
    {
        Json::Value by_name;
        users.select_by_name(NAME_A, by_name);
        uint64_t uid = by_name["id"].asUInt64();

        Json::Value info;
        bool ok = users.select_by_id(uid, info);
        EXPECT(ok, "查询成功");
        if (ok)
        {
            EXPECT(info["username"].asString() == NAME_A,
                   "用户名匹配: %s", info["username"].asCString());
            EXPECT(info["id"].asUInt64() == uid, "id 匹配");
        }
    }

    TEST_CASE("select_by_id - 查询不存在的 id");
    {
        Json::Value info;
        bool ok = users.select_by_id(0, info);
        EXPECT(!ok, "id=0 不存在，返回 false");
    }

    // ========================
    //  5. win() 测试
    // ========================
    TEST_CASE("win - 胜利更新积分");
    {
        Json::Value before;
        users.select_by_name(NAME_A, before);
        uint64_t uid = before["id"].asUInt64();
        uint64_t old_score = before["score"].asUInt64();
        int old_total = before["total_count"].asInt();
        int old_win   = before["win_count"].asInt();

        bool ok = users.win(uid);
        EXPECT(ok, "win() 执行成功");

        Json::Value after;
        users.select_by_name(NAME_A, after);
        EXPECT(after["score"].asUInt64() == old_score + 30,
               "积分 +30: %lu -> %lu", old_score, after["score"].asUInt64());
        EXPECT(after["total_count"].asInt() == old_total + 1,
               "总场次 +1: %d -> %d", old_total, after["total_count"].asInt());
        EXPECT(after["win_count"].asInt() == old_win + 1,
               "胜场 +1: %d -> %d", old_win, after["win_count"].asInt());
    }

    TEST_CASE("win - 不存在的 id（SQL 执行成功但影响 0 行）");
    {
        bool ok = users.win(99999);
        EXPECT(ok, "SQL 执行成功返回 true（实际影响 0 行）");
    }

    // ========================
    //  6. lose() 测试
    // ========================
    TEST_CASE("lose - 失败更新积分");
    {
        Json::Value before;
        users.select_by_name(NAME_A, before);
        uint64_t uid = before["id"].asUInt64();
        uint64_t old_score = before["score"].asUInt64();
        int old_total = before["total_count"].asInt();
        int old_win   = before["win_count"].asInt();

        bool ok = users.lose(uid);
        EXPECT(ok, "lose() 执行成功");

        Json::Value after;
        users.select_by_name(NAME_A, after);
        EXPECT(after["score"].asUInt64() == old_score - 30,
               "积分 -30: %lu -> %lu", old_score, after["score"].asUInt64());
        EXPECT(after["total_count"].asInt() == old_total + 1,
               "总场次 +1: %d -> %d", old_total, after["total_count"].asInt());
        EXPECT(after["win_count"].asInt() == old_win,
               "胜场不变: %d -> %d", old_win, after["win_count"].asInt());
    }

    TEST_CASE("lose - 不存在的 id（SQL 执行成功但影响 0 行）");
    {
        bool ok = users.lose(99999);
        EXPECT(ok, "SQL 执行成功返回 true（实际影响 0 行）");
    }

    // ========================
    //  7. 综合流程测试
    // ========================
    TEST_CASE("综合流程 - 注册→登录→多场对局");
    {
        Json::Value user;
        user["username"] = NAME_B;
        user["password"] = "flow123";
        bool ok = users.insert(user);
        EXPECT(ok, "注册成功");

        if (ok)
        {
            Json::Value login_info;
            login_info["username"] = NAME_B;
            login_info["password"] = "flow123";
            ok = users.login(login_info);
            EXPECT(ok, "登录成功");

            uint64_t uid = login_info["id"].asUInt64();

            // 模拟 3 胜 2 负
            users.win(uid);
            users.win(uid);
            users.lose(uid);
            users.win(uid);
            users.lose(uid);

            Json::Value final_info;
            users.select_by_name(NAME_B, final_info);
            // 初始 100 + 3*30 - 2*30 = 130
            EXPECT(final_info["score"].asUInt64() == 130,
                   "3胜2负后积分=130 (实际=%lu)", final_info["score"].asUInt64());
            EXPECT(final_info["total_count"].asInt() == 5,
                   "总场次=5 (实际=%d)", final_info["total_count"].asInt());
            EXPECT(final_info["win_count"].asInt() == 3,
                   "胜场=3 (实际=%d)", final_info["win_count"].asInt());
        }
    }

    // ========================
    //  汇总
    // ========================
    PRINT_SUMMARY();
    return g_failed == 0 ? 0 : 1;
}
