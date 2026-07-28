#ifndef UTILS_H
#define UTILS_H


#include <string>

#include <vector>



namespace Utils
{


    // 判断文件是否存在
    bool FileExists(
        const std::string& path
    );



    // 获取文件名
    std::string GetFileName(
        const std::string& path
    );



    // 获取文件路径
    std::string GetDirectory(
        const std::string& path
    );



    // 字符串分割
    std::vector<std::string> Split(
        const std::string& str,
        char delimiter
    );



    // 替换字符串
    std::string Replace(
        std::string str,
        const std::string& from,
        const std::string& to
    );



    // 获取当前时间字符串
    std::string GetTime();



}


#endif