#include "StringUtils.h"

using namespace tmms::base;
bool StringUtils::StartsWith(const string &s, const string &sub)
{
    if(sub.empty()) return true;
    if(s.empty()) return false;
    auto len=s.size();
    auto slen =sub.size();
    if(len<slen) return false;
    return s.compare(0, slen, sub) == 0;
}
bool StringUtils::EndsWith(const string &s, const string &sub)
{
    if(sub.empty()) return true;
    if(s.empty()) return false;
    auto len=s.size();
    auto slen =sub.size();
    if(len<slen) return false;
    return s.compare(len - slen, slen, sub) == 0;
}


std::string StringUtils::FilePath(const std::string &path)
{
    auto pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return "";
    return path.substr(0, pos);
}   
std::string StringUtils::FileNameExt(const std::string &path)
{
    auto pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        if(pos + 1 < path.size())
            return path.substr(pos + 1);
    } 
    return path;   
}
std::string StringUtils::FileName(const std::string &path)
{
    auto filename = FileNameExt(path);
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        if (pos != 0)
        {
            return filename.substr(0, pos);
        }
        return filename;
    }
        return filename;
}
std::string StringUtils::FileExtension(const std::string &path)
{
    auto filename = FileNameExt(path);
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        if (pos != 0&&pos + 1 < filename.size())
        {
            return filename.substr(pos + 1);
        }
        
        return "";
    }
    return "";
}

std::vector<std::string> StringUtils::SplitString(const std::string &s, const std::string &delimiter)
{
    if(delimiter.empty()) 
    return std::vector<std::string>{};
    std::vector<std::string> result;
    size_t last = 0;
    size_t next =0;
    while ((next =s.find(delimiter, last))!=std::string::npos)
    {
        if(next > last)
        {
            result.emplace_back(s.substr(last, next - last));
        }
        last = next + delimiter.size();

    }
    if(last < s.size())
    {
        result.emplace_back(s.substr(last));
    }
    return result;
}

std::vector<std::string> StringUtils::SplitStringwithFSM(const std::string &s, const char delimiter)
{
    enum{
        kStateInit = 0,
        kStateNormal = 1,
        kStateDelimiter = 2,
        kStateEnd = 3,
    };
    std::vector<std::string> result;
    int state = kStateInit;
    std::string tmp;
    state=kStateNormal;
    for(int pos = 0; pos < s.size(); pos++)
    {
        if(state == kStateNormal)
        {
            if(s.at(pos) == delimiter)
            {
                state = kStateDelimiter;
                continue;
            }
            tmp.push_back(s.at(pos));
            pos++;
        }else if(state == kStateDelimiter)
        {
            result.push_back(tmp);
            tmp.clear();
            state = kStateNormal;
            pos++;
        }
        
    }
    if(!tmp.empty())
    {
        result.push_back(tmp);
    }
    return result;
}