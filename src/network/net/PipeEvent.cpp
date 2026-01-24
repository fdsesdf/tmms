#include "PipeEvent.h"
#include "network/base/Network.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

using namespace tmms::network;
PipeEvent::PipeEvent(EventLoop *loop) : Event(loop)
{
    int fds[2]={0,};
    auto ret = ::pipe2(fds, O_NONBLOCK);
    if (ret < 0)
    {
        NETWORK_LOG_ERROR << "PipeEvent pipe2 error: " ;
        exit(1);
    }
    fd_ = fds[0];
    write_fd_ = fds[1];
}
PipeEvent::~PipeEvent()
{
    if (fd_ > 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
    if (write_fd_ > 0)
    {
        ::close(write_fd_);
        write_fd_ = -1;
    }
}

void PipeEvent::OnRead()
{
    while (true)
    {
        int64_t tmp = 0;
        ssize_t ret = ::read(fd_, &tmp, sizeof(tmp));
        if (ret > 0)
        {
            std::cout << "PipeEvent read " << ret << " bytes, value=" << tmp << std::endl;
            continue; // 继续把 pipe 读空
        }

        if (ret == 0)
        {
            // 读端被关闭
            OnClose();
            return;
        }

        // ret < 0
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 已经读空了
            return;
        }

        NETWORK_LOG_ERROR << "PipeEvent read error errno=" << errno;
        OnError("read error");
        return;
    }
}

void PipeEvent::OnClose()
{
    if(write_fd_ > 0)
    {
        ::close(write_fd_);
        write_fd_ = -1;
    }
}
void PipeEvent::OnError(const std::string &err)
{
    std::cout << "PipeEvent error: " << err << std::endl;
}

void PipeEvent::Write(const char *data, size_t len)
{
    ssize_t n = ::write(write_fd_, data, len);
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return;
        }
        NETWORK_LOG_ERROR << "PipeEvent write error errno=" << errno;
        OnError("write error");
    }
}
