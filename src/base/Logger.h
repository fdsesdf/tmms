#pragma once

#include <string>
#include "base/NonCopyable.h"
#include "base/FileLog.h"

namespace tmms {
namespace base {

/* 1️⃣ 先定义 enum（纯类型，无依赖） */
enum LogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kMaxNumOfLogLevels
};

/* 2️⃣ 再定义 class（完整定义） */
class Logger : public NonCopyable {
public:
    explicit Logger(const FileLogPtr& log);
    ~Logger() = default;

    void SetLogLevel(LogLevel level);
    LogLevel GetLogLevel() const;
    void Write(const std::string& msg);

private:
    LogLevel level_{kDebug};
    FileLogPtr log_;
};

/* 3️⃣ extern 必须放在 class 之后 */
extern Logger* g_logger;

} // namespace base
} // namespace tmms
