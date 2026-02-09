#include "TcpConnection.h"
#include "network/base/Network.h"
#include <errno.h>
#include <iostream>
using namespace tmms::network;

TcpConnection::TcpConnection(EventLoop *loop,int socketfd,const InetAddress &locakAddr,const InetAddress &peerAddr):Connection(loop,socketfd,locakAddr,peerAddr)
{}

TcpConnection::~TcpConnection()
{
    
}

void TcpConnection::SetCloseCallback(const CloseConnectionCallback &cb)
{
    close_cb_ = cb;
}
void TcpConnection::SetCloseCallback(CloseConnectionCallback &&cb)
{
    close_cb_ = std::move(cb);
}
void TcpConnection::OnClose()
{
    if (closed_) return;
    loop_->AssertInLoopThread();


     int err = 0;
    socklen_t len = sizeof(err);
    ::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len);

    std::cout << "[OnClose] so_error=" << err 
              << " errno=" << errno << std::endl;

    if(!closed_){
        closed_ = true;

        if(close_cb_){
            close_cb_(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
        }
        Event::Close();
    }
}
void TcpConnection::ForceClose()
{
    loop_->RunInLoop([this](){
        OnClose();
    });
}

void TcpConnection::OnRead()
{
    if(closed_){
        NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" closed";
        return;
    }
    while(true)
    {
        int err=0;
        auto ret=message_buffer_.readFd(fd_,&err);
        if(ret > 0){
            if(message_cb_){
                message_cb_(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()),message_buffer_);
            }
        }else if(ret == 0){
            OnClose();
            break;
        }else{
            std::cout << "[OnWrite ERROR] errno=" << errno << std::endl;
            if(err != EINTR&&err != EAGAIN&&err != EWOULDBLOCK){
                NETWORK_LOG_ERROR<<"read error:"<<err;
                OnClose();
            }
            break;
        }
    }
}

void TcpConnection::SetMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}
void TcpConnection::SetMessageCallback(MessageCallback &&cb)
{
    message_cb_ = std::move(cb);
}

void TcpConnection::OnError(const std::string &msg)
{
    NETWORK_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" error:"<<msg;
    OnClose();
}

void TcpConnection::SetWriteCompleteCallback(const WriteCompleteCallback &cb)
{
    write_complete_cb_ = cb;
}

void TcpConnection::SetWriteCompleteCallback(WriteCompleteCallback &&cb)
{
    write_complete_cb_ = std::move(cb);
}


void TcpConnection::OnWrite()
{
    if(closed_){
        NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" closed";
        return;
    }
    if(!io_vec_list_.empty()){
        while(true)
        {
            auto ret=::writev(fd_,&io_vec_list_[0],io_vec_list_.size());
            if(ret >= 0){
                while(ret > 0){
                    if(io_vec_list_.front().iov_len > ret){
                        io_vec_list_.front().iov_base=(char *)io_vec_list_.front().iov_base+ret;
                        io_vec_list_.front().iov_len-=ret;
                        break;
                    }else{
                        ret-=io_vec_list_.front().iov_len;
                        io_vec_list_.erase(io_vec_list_.begin());
                    }
                }
                if(io_vec_list_.empty()){
                    EnableWrite(false);
                    if(write_complete_cb_){
                        write_complete_cb_(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
                    }
                }
            }else{
                if(errno !=EINTR&&errno != EAGAIN&&errno != EWOULDBLOCK){
                    NETWORK_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" writev error:"<<errno;
                    OnClose();
                }
                break;
            }
        }
    }
    else{
        EnableWrite(false);
        if(write_complete_cb_){
            write_complete_cb_(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
        }
    }
}

void TcpConnection::Send(std::list<BufferNodePtr> &list)
{
    loop_->RunInLoop([this,&list](){
        SendInLoop(list);
    });

}

void TcpConnection::Send(const char *buf,size_t size)
{
    loop_->RunInLoop([this,buf,size](){
        SendInLoop(buf,size);
    });
}

void TcpConnection::SendInLoop(std::list<BufferNodePtr> &list)
{
    if (closed_)
    {
        NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" closed";
        return;
    }
    for(auto &l:list)
    {
        struct iovec vec;
        vec.iov_base = l->addr;
        vec.iov_len = l->size;
        io_vec_list_.push_back(vec);
    }
    if(io_vec_list_.empty())
    {
        EnableWrite(true);
    }

    
}
void TcpConnection::SendInLoop(const char *buf,size_t size)
{
    if(closed_){
        NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" closed";
        return;
    }
    size_t send_len = 0;
    if(io_vec_list_.empty())
    {
        send_len = ::write(fd_,buf,size);
        if(send_len < 0){
            if(errno !=EINTR&&errno != EAGAIN&&errno != EWOULDBLOCK){
                NETWORK_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" write error:"<<errno;
                OnClose();
                return;
            }
            send_len = 0;
        }
        size-=send_len;
        if(size==0){
            if(write_complete_cb_){
                write_complete_cb_(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
            }
            return;
        }
    }
    if(size > 0){
        struct iovec vec;
        vec.iov_base = (void *)(buf+send_len);
        vec.iov_len = size;
        io_vec_list_.push_back(vec);
        EnableWrite(true);
    }
}

void TcpConnection::SetTimeoutCallback(int timeout, const TimeoutCallback &cb)
{
    auto cp=std::dynamic_pointer_cast<TcpConnection>(shared_from_this());
    loop_->RunAfter(timeout,[&cp,cb](){
        cb(cp);
    });
}

void TcpConnection::SetTimeoutCallback(int timeout, TimeoutCallback &&cb)
{
    auto cp=std::dynamic_pointer_cast<TcpConnection>(shared_from_this());
    loop_->RunAfter(timeout,[&cp,cb](){
        cb(cp);
    });
}
void TcpConnection::EnableCheckTimeout(int32_t max_time)
{
    
    auto tp=std::make_shared<TimeoutEntry>
    (std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
    max_idle_time_=max_time;
    timeout_entry_=tp;
    loop_->InsertEntry(max_idle_time_,tp);
}
void TcpConnection::OnTimeout()
{
    //NETWORK_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" timeout";
    std::cout<<"host:"<<peer_addr_.ToIpPort()<<" timeout"<<std::endl;
    OnClose();
}

void TcpConnection::ExtendLife()
{
    auto tp=timeout_entry_.lock();
    if(tp){
        loop_->InsertEntry(max_idle_time_,tp);
    }
}

