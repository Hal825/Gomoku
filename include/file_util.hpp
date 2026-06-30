#ifndef __FILE_UTIL__
#define __FILE_UTIL__

#include <iostream>
#include <string>
#include <fstream>
#include "logger.hpp"

class file_util{
public:
    static bool read(const std::string &filename,std::string &body)
    {
        //打开文件
        std::ifstream ifs(filename,std::ios::binary);
        if(ifs.is_open() == false)
        {
            DBG_LOG("%s file open failed!!",filename.c_str());
            return false;
        }
        //获取文件大小
        size_t fsize = 0;
        ifs.seekg(0,std::ios::end);
        fsize = ifs.tellg();
        ifs.seekg(0,std::ios::beg);
        body.resize(fsize);
        //读取文件
        ifs.read(&body[0],fsize);
        if(ifs.good() == false)
        {
            ERR_LOG("read %s file content failed!",filename.c_str());
            ifs.close();
            return false;
        }
        //关闭文件
        ifs.close();
        return true;
    }
};

#endif