#pragma once

#include <string>
#include "base/NonCopyable.h"
#include "base/FileLog.h"

namespace tmms {
    namespace base {
        enum LogLevel {
            kTrace,
            kDebug,
            kInfo,
            kWarn,
            kError,
            kMaxNumOfLogLevels
        };
        class Logger;
        using FileLogPtr = std::shared_ptr<FileLog>;
        class Logger : public NonCopyable {
        public:
            Logger(const FileLogPtr& log);
            ~Logger() = default;

            void SetLogLevel(LogLevel level);
            LogLevel GetLogLevel() const;
            void Write(const std::string& msg);

        private:
            LogLevel level_{kDebug};
            FileLogPtr log_;
        };
        extern Logger* g_logger;
    } 
} 
