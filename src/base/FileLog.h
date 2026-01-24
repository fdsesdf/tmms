#pragma once
#include <string>
#include <memory>
namespace tmms
{
    namespace base
    {
        enum class RotateType
        {
            kRotateHour,
            kRotateMinute,//测试用
            KRotateDay,
            kRoteNone
        };
        class FileLog
        {
        public:
            FileLog()=default;
            ~FileLog()=default;
            bool Open(const std::string &filename);
            size_t WriteLog(const std::string &msg);
            void Rotate(const std::string &file);
            void SetRotate(RotateType type);
            RotateType GetRotate() const;
            int64_t FileSize() const;
            std::string FilePath() const;
        private:
            int fd_{-1};
            std::string file_path_;
            RotateType rotate_type_{RotateType::kRoteNone};
        };
        using FileLogPtr=std::shared_ptr<FileLog>;
    }
}
