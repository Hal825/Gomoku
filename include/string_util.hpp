#ifndef __STRING_UTIL__
#define __STRING_UTIL__

#include <string>
#include <vector>

class string_util
{
public:
    static int split(const std::string &src, const std::string &sep, std::vector<std::string> &res)
    {
        size_t pos, idx = 0;
        while (idx < src.size())
        {
            pos = src.find(sep, idx);
            if (pos == std::string::npos)
            {
                res.push_back(src.substr(idx));
                return;
            }
            if(pos==idx){
                idx+=sep.size();
                continue;
            }
            res.push_back(src.substr(idx, pos - idx));
            idx = pos + sep.size();
        }
        return res.size();
    }
};

#endif