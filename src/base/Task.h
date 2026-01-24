#pragma once
#include <cstdint>
#include <functional>
#include <memory>

namespace tmms {
namespace base {

class Task;
using TaskPtr = std::shared_ptr<Task>;
using TaskCallback = std::function<void(const TaskPtr&)>;

class Task : public std::enable_shared_from_this<Task>
{
public:
    Task(const TaskCallback& cb, int64_t interval);
    Task(TaskCallback&& cb, int64_t interval);

    void Run();     // 执行任务
    void Restart(); // 重启任务

    int64_t When() const { return when_; }

private:
    int64_t interval_{0};
    int64_t when_{0};
    TaskCallback cb_{nullptr};
};

} // namespace base
} // namespace tmms
