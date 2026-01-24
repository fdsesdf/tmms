#include "FileMgr.h"
#include "TTime.h"
#include "StringUtils.h"
#include <sstream>

#include <iostream>
using namespace tmms::base;
namespace
{
    static tmms::base::FileLogPtr file_log_nullptr;
}
void FileMgr::OnCheck()
{
    int year=0,month=0,day=0,hour=0,minute=0,second=0;
    bool day_changed=false;
    bool hour_changed=false;
    bool minute_changed=false;
    TTime::Now(year,month,day,hour,minute,second);
    if( last_year_==-1)
    {
        last_day_=day;
        last_hour_=hour;
        last_minute_=minute;
        last_year_=year;
        last_month_=month;
    }
    if(last_day_!=day)
    {
        day_changed=true;
    }
    if(last_hour_!=hour)
    {
        hour_changed=true;  
    }
    if(last_minute_!=minute)
    {
        minute_changed=true;
    }
    if(!day_changed&&!hour_changed&&!minute_changed)
    {
        return;
    }
    std::lock_guard<std::mutex> lk(lock_);
    for(auto &it:logs_)
    {
        if(minute_changed&&it.second->GetRotate()==RotateType::kRotateMinute)
        {
            RotateMinutes(it.second);
        }
        if(day_changed&&it.second->GetRotate()==RotateType::KRotateDay)
        {
            RotateDays(it.second);
        }
        else if(hour_changed&&it.second->GetRotate()==RotateType::kRotateHour)
        {
            RotateHours(it.second);
        }
        
    }
    last_day_=day;
    last_hour_=hour;
    last_year_=year;
    last_month_=month;
}
FileLogPtr FileMgr::GetFileLog(const std::string &file_name){
    std::lock_guard<std::mutex> lk(lock_);
    auto iter=logs_.find(file_name);
    if(iter!=logs_.end())
    {
        return iter->second;
    }
    FileLogPtr log=std::make_shared<FileLog>();
    if(!log->Open(file_name))
    {
        return file_log_nullptr;
    }
    logs_.emplace(file_name,log);
    return log;
}
void FileMgr::RemoveFileLog(const FileLogPtr &log)
{
    std::lock_guard<std::mutex> lk(lock_);
    logs_.erase(log->FilePath());
}
void FileMgr::RotateDays(const FileLogPtr &file)
{
    if(file->FileSize()>0)
    {
        char buf[128]={0,};
        sprintf(buf,"_%04d-%02d-%02d",last_year_,last_month_,last_day_ );
        std::string file_path=file->FilePath();
        std::string path = StringUtils::FilePath(file_path);
        std::string filename = StringUtils::FileName(file_path);
        std::string file_ext = StringUtils::FileExtension(file_path);
        std::ostringstream ss;
        ss<<path
            <<filename
            <<buf
            <<file_ext;
        file->Rotate(ss.str());
    }
}
void FileMgr::RotateHours(const FileLogPtr &file)
{
    if(file->FileSize()>0)
    {
        char buf[128]={0,};
        sprintf(buf,"_%04d-%02d-%02dT%2d",last_year_,last_month_,last_day_,last_hour_);
        std::string file_path=file->FilePath();
        std::string path = StringUtils::FilePath(file_path);
        std::string filename = StringUtils::FileName(file_path);
        std::string file_ext = StringUtils::FileExtension(file_path);
        std::ostringstream ss;
        ss<<path
            <<filename
            <<buf
            <<file_ext;
        file->Rotate(ss.str());
    }
}
void FileMgr::RotateMinutes(const FileLogPtr &file)
{
    if(file->FileSize()>0)
    {
        char buf[128]={0,};
        sprintf(buf,"_%04d-%02d-%02dT%02d-%02d",last_year_,last_month_,last_day_,last_hour_,last_minute_);
        std::string file_path=file->FilePath();
        std::string path = StringUtils::FilePath(file_path);
        std::string filename = StringUtils::FileName(file_path);
        std::string file_ext = StringUtils::FileExtension(file_path);
        std::ostringstream ss;
        ss<<path
            <<filename
            <<buf
            <<file_ext;
        file->Rotate(ss.str());
    }
}