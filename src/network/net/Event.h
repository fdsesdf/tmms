#pragma once
#include <string>
#include <memory>
#include <sys/epoll.h>
namespace tmms
{
    namespace network
    {
        class EventLoop;
        const int kEventRead=(EPOLLIN|EPOLLPRI|EPOLLET);
        const int kEventWrite=(EPOLLOUT|EPOLLET);
        class Event:public std::enable_shared_from_this<Event>
        {
            friend class EventLoop;
        public:
            Event(EventLoop *loop);
            Event(EventLoop* loop,int fd);
            virtual ~Event();
            virtual void OnRead(){};//read event callback
            virtual void OnWrite(){};//write event callback
            virtual void OnClose(){};//close event callback
            virtual void OnError(const std::string & msg){};//error event callback
            bool EnableRead(bool enable);//enable/disable read event
            bool EnableWrite(bool enable);//enable/disable write event
            int Fd() const;//get fd
            void Close();//close fd
        protected:
            EventLoop* loop_{nullptr};
            int fd_{-1};
            int event_{0};
        };
    }
}