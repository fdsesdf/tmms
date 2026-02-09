#pragma once
#include <string>
#include <vector>
namespace tmms
{   
    namespace base
    {
        using std::string;
        class StringUtils
        {
        public:
            static bool StartsWith(const string &s, const string &sub);//判断字符串s是否以sub开头
            static bool EndsWith(const string &s, const string &sub);//判断字符串s是否以sub结尾
            static std::string FilePath(const std::string &path);//获取文件路径
            static std::string FileNameExt(const std::string &path);//获取文件扩展名
            static std::string FileName(const std::string &path);//获取文件名
            static std::string FileExtension(const std::string &path);//获取文件扩展名
            static std::vector<std::string> SplitString(const std::string &s, const string &delimiter);//分割字符串
            static std::vector<std::string> SplitStringwithFSM(const std::string &s, const char delimiter);//使用有限状态机分割字符串
        };
    }
}   


