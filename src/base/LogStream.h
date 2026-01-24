#pragma once
#include <sstream>
#include <string>
#include "base/Logger.h"
namespace tmms
{
    namespace base
    {
        class Logger;      // ✅ 前向声明
        class LogStream
        {
        public:
            LogStream(Logger* logger,const char* file,int line,LogLevel l,const char* func=nullptr);
            ~LogStream();
            template<typename T>
            LogStream& operator<<(const T& value)
            {
                stream_<<value;
                return *this;  
            }
            
        private:
            std::ostringstream stream_;
            Logger* logger_{nullptr};
        };
    }
}   
#define LOG_TRACE \
    if(tmms::base::g_logger && tmms::base::g_logger->GetLogLevel()<=tmms::base::kTrace) \
        tmms::base::LogStream(tmms::base::g_logger,__FILE__,__LINE__,tmms::base::kTrace,__func__)

#define LOG_DEBUG \
    if(tmms::base::g_logger && tmms::base::g_logger->GetLogLevel()<=tmms::base::kDebug) \
        tmms::base::LogStream(tmms::base::g_logger,__FILE__,__LINE__,tmms::base::kDebug,__func__)

#define LOG_INFO \
    if(tmms::base::g_logger && tmms::base::g_logger->GetLogLevel()<=tmms::base::kInfo) \
        tmms::base::LogStream(tmms::base::g_logger,__FILE__,__LINE__,tmms::base::kInfo)

#define LOG_WARN \
    if(tmms::base::g_logger) \
        tmms::base::LogStream(tmms::base::g_logger,__FILE__,__LINE__,tmms::base::kWarn)

#define LOG_ERROR \
    if(tmms::base::g_logger && tmms::base::g_logger->GetLogLevel()<=tmms::base::kError) \
        tmms::base::LogStream(tmms::base::g_logger,__FILE__,__LINE__,tmms::base::kError)


