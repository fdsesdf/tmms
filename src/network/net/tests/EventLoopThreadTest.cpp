#include "network/net/EventLoopThread.h"
#include "network/net/EventLoop.h"
#include <iostream>
#include "network/net/PipeEvent.h"
#include "base/TTime.h"
#include <thread>
#include "network/net/EventLoopThreadPool.h"
using namespace tmms::network;

EventLoopThread eventloop_thread;
std::thread th;
void TestEventLoopThread()
{
    eventloop_thread.Run();
    EventLoop *loop=eventloop_thread.Loop();
    if(loop)
    {
        std::cout<<"loop:"<<loop<<std::endl;
        PipeEventPtr pipe_event = std::make_shared<PipeEvent>(loop);
        loop->AddEvent(pipe_event);
        int64_t tmp = 1234567890;
        pipe_event->Write((const char*)&tmp, sizeof(tmp));
        th=std::thread([&pipe_event](){
            
            while(1){
                int64_t now=tmms::base::TTime::Now();
                pipe_event->Write((const char*)&now, sizeof(now));
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
        while(1){
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
void TestEventLoopThreadPool()
{
    EventLoopThreadPool thread_pool(2,0,2);
    thread_pool.Start();
    // std::cout<<"thread id:"<<std::this_thread::get_id()<<std::endl;
    // std::vector<EventLoop*> list=thread_pool.GetLoops();
    // for(auto &loop:list){
    //     loop->RunInLoop([&loop](){
    //         std::cout<<"run in loop"<<loop<<"thread id:"<<std::this_thread::get_id()<<std::endl;
    //     });
    // }
    EventLoop *loop=thread_pool.GetNextLoop();
    std::cout<<"loop:"<<loop<<std::endl;
    loop->RunAfter(1,[](){
        std::cout<<"run after 1s now:"<<tmms::base::TTime::Now()<<std::endl;
    });
    loop->RunAfter(5,[](){
        std::cout<<"run after 5s now:"<<tmms::base::TTime::Now()<<std::endl;
    });
    loop->RunEvery(1,[](){
        std::cout<<"run every 1s now:"<<tmms::base::TTime::Now()<<std::endl;
    });
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int main()
{
    
    TestEventLoopThreadPool();
    while(1){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
