#pragma once
#include <vector>
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <functional>
#include <queue>
#include <mutex>
#include "PipeEvent.h"
#include "TimingWheel.h"
namespace tmms
{
    namespace network
    {
        using Func=std::function<void()>;
        using EventPtr=std::shared_ptr<class Event>;
        class EventLoop
        {
        public:
            EventLoop() ;
            ~EventLoop() ;

            void Loop();
            void Quit();
            void AddEvent(const EventPtr & event);//add event to epoll
            void DelEvent(const EventPtr & event);//del event from epoll
            bool EnableRead(const EventPtr & event, bool enable);//enable/disable read event
            bool EnableWrite(const EventPtr & event, bool enable);//enable/disable write event
            void AssertInLoopThread();//assert in loop thread
            bool IsInLoopThread() const;//check in loop thread
            void RunInLoop(const Func & f);//run function in loop thread
            void RunInLoop(const Func && f);//run function in loop thread
            void InsertEntry(uint32_t delay, EntryPtr  entryPtr);//插入一个任务，delay是延迟时间，单位是秒
            void RunAfter(double delay, const Func &cb);//delay秒后执行一次
            void RunAfter(double delay,  Func &&cb);//delay秒后执行一次
            void RunEvery(double interval, const Func &cb);//interval秒间隔执行一次
            void RunEvery(double interval,  Func &&cb);//interval秒间隔执行一次
        private:
            void RunFuctions();//run functions in loop thread
            void WakeUp();//wake up epoll
            void QueueInLoop(const Func & f);//queue function in loop thread
            void QueueInLoop(const Func && f);//queue function in loop thread
            
        private:
            bool looping_{false};
            int epoll_fd_{-1};
            std::vector<struct epoll_event> epoll_events_;
            std::unordered_map<int,EventPtr> events_;
            std::queue<Func> functions_;
            std::mutex lock_;
            PipeEventPtr pipe_event_;
            TimingWheel wheel_;
        };
    }
}