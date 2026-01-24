#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include "base/NonCopyable.h"
#include "network/net/EventLoop.h"
#include <future>
namespace tmms
{
    namespace network
    {
        class EventLoopThread:public base::NonCopyable
        {
        public:
            EventLoopThread();
            ~EventLoopThread();
            void Run();//创建线程
            
            EventLoop *Loop()const;//返回事件循环指针
            std::thread &Thread();//返回线程指针
        private:
            void StartEventLoop();//等待线程运行起来
            EventLoop *loop_{nullptr};
            std::thread thread_;
            bool running_{false};
            std::mutex lock_;
            std::condition_variable condition_;
            std::once_flag once_;
            std::promise<int> promise_loop_;
        };
    }
}