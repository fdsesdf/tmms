#include "Event.h"
#include "EventLoop.h"
#include "network/base/Network.h"
#include <unistd.h>

using namespace tmms::network;
Event::Event(EventLoop* loop,int fd) : loop_(loop), fd_(fd)
{
    
}
Event::Event(EventLoop *loop) : loop_(loop)
{
    
}
Event::~Event()
{
    Close();
}
void Event::Close()
{
    if(fd_ > 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

bool Event::EnableRead(bool enable)
{
    return loop_->EnableRead(shared_from_this(),enable);
}

bool Event::EnableWrite(bool enable)
{
    return loop_->EnableWrite(shared_from_this(),enable);
}

int Event::Fd() const
{
    return fd_;
}