#include "Config.h"
#include "base/LogStream.h"
#include <fstream>

using namespace tmms::base;

bool Config::LoadConfig(const std::string &file)
{
    LOG_DEBUG<<"config file:"<<file;
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::ifstream in(file);
    std::string errs;
    auto ok=Json::parseFromStream(reader, in, &root, &errs);
    if(!ok){
        LOG_DEBUG <<"config file:"<<file<<" parse error.";
        return false;
    }
    Json::Value nameObj=root["name"];
    if(!nameObj.isNull()){
        name_=nameObj.asString();
    }
    Json::Value cpuStartObj=root["cpu_start"];
    if(!cpuStartObj.isNull()){
        cpu_start_=cpuStartObj.asInt();
    }
    Json::Value threadsObj=root["threads"];
    if(!threadsObj.isNull()){
        threads_nums_=threadsObj.asInt();
    }
    Json::Value logObj=root["log"];
    if(!logObj.isNull()){
        ParseLogInfo(logObj);
    }
    return true;
}

bool Config::ParseLogInfo(const Json::Value &root)
{
    log_info_ = std::make_shared<LogInfo>();
    Json::Value levelObj=root["level"];
    if(!levelObj.isNull()){
        std::string level=levelObj.asString();
        if(level=="TRACE"){
            log_info_->level=kTrace;
        } else if(level=="DEBUG"){
            log_info_->level=kDebug;
        } else if(level=="INFO"){
            log_info_->level=kInfo;
        } else if(level=="WARN"){
            log_info_->level=kWarn;
        } else if(level=="ERROR"){
            log_info_->level=kError;
        }
    }
    Json::Value pathObj=root["path"];
    if(!pathObj.isNull()){
        log_info_->path=pathObj.asString();
    }
    Json::Value nameObj=root["name"];
    if(!nameObj.isNull()){
        log_info_->name=nameObj.asString();
    }
    Json::Value rotateObj=root["rotate"];
    if(!rotateObj.isNull()){
        std::string rotate=rotateObj.asString();
        if(rotate=="HOUR"){
            log_info_->rotate_type=RotateType::kRotateHour;
        } else if(rotate=="MINUTE"){
            log_info_->rotate_type=RotateType::kRotateMinute;
        } else if(rotate=="DAY"){
            log_info_->rotate_type=RotateType::KRotateDay;
        } else {
            log_info_->rotate_type=RotateType::kRoteNone;
        }
    }
    return true;
}


LogInfoPtr Config::GetLogInfo()
{
    std::lock_guard<std::mutex> guard(lock_);
    return log_info_;
}

bool ConfigMgr::LoadConfig(const std::string &file)
{
    LOG_DEBUG << "load config file: " << file;
    ConfigPtr new_config = std::make_shared<Config>();
    if (!new_config->LoadConfig(file))
    {
        return false;
    }
        std::lock_guard<std::mutex> guard(lock_);
        config_ = new_config;
    return true;
}

    