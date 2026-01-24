#include "TaskMgr.h"
#include "TTime.h"
using namespace tmms::base;

void TaskMgr::OnWork()
{
    std::lock_guard<std::mutex> lk(lock_);
    for (auto iter = tasks_.begin(); iter != tasks_.end(); )
{
    int64_t now = TTime::NowMS();
    if ((*iter)->When() <= now)
    {
        (*iter)->Run();

        if ((*iter)->When() <= now)
        {
            iter = tasks_.erase(iter);  // 用返回值更新 iter
            continue;                   // 绝对不能再 ++iter
        }
    }
    ++iter;  // 只有在没有 erase 时才 ++
}

    
}
bool TaskMgr::Add(TaskPtr &task)
{
    std::lock_guard<std::mutex> lk(lock_);
    auto iter = tasks_.find(task);
    if(iter!=tasks_.end())
    {
        return false;
    }
    tasks_.emplace(task);
    return true;
}       

bool TaskMgr::Del(TaskPtr &task)
{
    std::lock_guard<std::mutex> lk(lock_);
    tasks_.erase(task);
    return true;
}      
