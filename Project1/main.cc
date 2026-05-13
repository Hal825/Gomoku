#include "logger.hpp"

int main()
{
    LOG("Hello, %s!\n", "world");
    // LOG("Hello, %s!\n", "world");
    ILOG("Hello, %s!\n", "INF");
    DLOG("Hello, %s!\n", "DEBUG");
    ELOG("Hello, %s!\n", "ERROR");
    return 0;
}