#pragma once
#include <string>
#include <cstdint>
#include "base/Logger.h"
#include "base/FileLog.h"
#include "json/json.h"
#include "base/Singleton.h"
#include <memory>
#include <mutex>
#include "base/NonCopyable.h"
namespace tmms
{
    namespace base
    {
        struct LogInfo
        {
            LogLevel level;
            std::string path;
            std::string name;
            RotateType rotate_type{RotateType::kRoteNone};
        };
        using LogInfoPtr=std::shared_ptr<LogInfo>;
        class Config
        {
        public:
            Config() = default;
            ~Config() = default;

            bool LoadConfig(const std::string &file);//load config file
            bool ParseLogInfo(const Json::Value &root);//parse log info from json root
            LogInfoPtr GetLogInfo();//get log info
            std::string name_;
            int32_t cpu_start_{0};
            int32_t threads_nums_{1};
        private:
            LogInfoPtr log_info_;
            std::mutex lock_;
        };
        using ConfigPtr=std::shared_ptr<Config>;
        class ConfigMgr:public NonCopyable
        {
        private:
            std::mutex lock_;
            ConfigPtr config_;
        public:           
            ConfigMgr()=default;
            ~ConfigMgr()=default;
            bool LoadConfig(const std::string &file);
            ConfigPtr& GetConfig()
            {
                return config_;
            }
        };
        #define sConfigMgr tmms::base::Singleton<ConfigMgr>::Instance()
    }
}