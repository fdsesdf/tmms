#include "UdpSocket.h"
#include "network/base/Network.h"
#include "network/base/MsgBuffer.h"
#include <iostream>
using namespace tmms::network;
UdpSocket::UdpSocket(EventLoop *loop, int socketfd, const InetAddress &localAddr, const InetAddress &peerAddr) : Connection(loop, socketfd, localAddr, peerAddr),message_buffer_size_(message_buffer_size_){}

UdpSocket::~UdpSocket()
{
    OnClose();
}

void UdpSocket::OnTimeout()
{
    NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" timeout.close it:";
    OnClose();
}
void UdpSocket::OnError(const std::string &msg)
{
    NETWORK_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" error:"<<msg;
    OnClose();
}

// void UdpSocket::OnRead()
// {
//     if(closed_){
//         NETWORK_LOG_WARN<<"host:"<<peer_addr_.ToIpPort()<<" had closed";
//         return;
//     }
//     ExtendLife();
//     while(true)
//     {
//         struct sockaddr_in6 sock_addr;
//         socklen_t len=sizeof(sock_addr);
//         auto ret=::recvfrom(fd_,msg_buffer_.beginWrite(),
//         message_buffer_size_,0,(struct sockaddr *)&sock_addr,&len);
        
//         if(ret > 0){
//             InetAddress peeraddr;
//             msg_buffer_.hasWritten(ret);
//             if(sock_addr.sin6_family == AF_INET){
//                 char ip[16]={0,};
//                 struct sockaddr_in *in = (struct sockaddr_in *)&sock_addr;
//                 ::inet_ntop(AF_INET,&in->sin_addr,ip,sizeof(ip));
//                 peeraddr.SetAddr(ip);
//                 peeraddr.SetPort(ntohs(in->sin_port));
//             }else if(sock_addr.sin6_family == AF_INET6){
//                 char ip[INET6_ADDRSTRLEN]={0,};
//                 ::inet_ntop(AF_INET6,&(sock_addr.sin6_addr),ip,sizeof(ip));
//                 peeraddr.SetAddr(ip);
//                 peeraddr.SetPort(ntohs(sock_addr.sin6_port));
//                 peeraddr.SetIsIPV6(true);
//             }
//             if(message_callback_){
//                 message_callback_(peeraddr,msg_buffer_);
//             }
//             msg_buffer_.retrieveAll();
//         }
//         else if(ret < 0){
//             if(errno != EINTR&&errno != EAGAIN&&errno != EWOULDBLOCK){
//                 NET_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" error:"<<errno;
//                 OnClose();
//                 return;
//             }
//             break;
//         }
//     }
// }
void UdpSocket::OnRead()
{
    if (closed_) return;

    ExtendLife();

    while (true) {
        struct sockaddr_storage addr;
        socklen_t len = sizeof(addr);

        size_t writable = msg_buffer_.writableBytes();
        if (writable == 0) break;

        auto ret = ::recvfrom(
            fd_,
            msg_buffer_.beginWrite(),
            writable,
            0,
            (struct sockaddr *)&addr,
            &len
        );

        if (ret > 0) {
            msg_buffer_.hasWritten(ret);

            InetAddress peeraddr;
            if (addr.ss_family == AF_INET) {
                auto *in = (sockaddr_in *)&addr;
                char ip[16];
                inet_ntop(AF_INET, &in->sin_addr, ip, sizeof(ip));
                peeraddr.SetAddr(ip);
                peeraddr.SetPort(ntohs(in->sin_port));
            } else if (addr.ss_family == AF_INET6) {
                auto *in6 = (sockaddr_in6 *)&addr;
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &in6->sin6_addr, ip, sizeof(ip));
                peeraddr.SetAddr(ip);
                peeraddr.SetPort(ntohs(in6->sin6_port));
                peeraddr.SetIsIPV6(true);
            }
  
            if (message_callback_) {
                message_callback_(peeraddr, msg_buffer_);
            }
        }
        else if (ret < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            OnClose();
            return;
        }
    }
}

void UdpSocket::OnWrite()
{
    if(closed_){
        NETWORK_LOG_TRACE<<"host:"<<peer_addr_.ToIpPort()<<" closed";
        return;
    }
    ExtendLife();
    while(true)
    {
        if(!buffer_list_.empty())
        {
            auto buf=buffer_list_.front();
            auto ret=::sendto(fd_,buf->sock_addr,buf->size,0,buf->sock_addr,buf->sock_len_);
            if(ret > 0){
                buffer_list_.pop_front();
            }
            else if(ret < 0){
                if(errno != EINTR&&errno != EAGAIN&&errno != EWOULDBLOCK){
                    NET_LOG_ERROR<<"host:"<<peer_addr_.ToIpPort()<<" error:"<<errno;
                    OnClose();
                    return;
                }
                break;
            }
        }
        if(buffer_list_.empty()){
            if(write_complete_callback_){
                write_complete_callback_(std::dynamic_pointer_cast<UdpSocket>(shared_from_this()));
            }
            break;
        }
    }
}
void UdpSocket::OnClose()
{
    if(!closed_){
        closed_=true;
        if(close_callback_){
            close_callback_(std::dynamic_pointer_cast<UdpSocket>(shared_from_this()));
        }
        Event::Close();
    }
}

void UdpSocket::SetCloseCallback(UdpSocketCloseCallback &&cb)
{
    close_callback_=std::move(cb);
}
void UdpSocket::SetCloseCallback(const UdpSocketCloseCallback &cb)
{
    close_callback_=cb;
}
void UdpSocket::SetRecvMsgCallback(MessageCallback &&cb)
{
    message_callback_=std::move(cb);
}

void UdpSocket::SetWriteCompleteCallback(UdpSocketWriteCallback &&cb)
{
    write_complete_callback_=std::move(cb);
}   
void UdpSocket::SetRecvMsgCallback(const MessageCallback &cb)
{
    message_callback_=cb;
}

void UdpSocket::SetWriteCompleteCallback(const UdpSocketWriteCallback &cb)
{
    write_complete_callback_=cb;
}

void UdpSocket::SetTimeoutCallback(int timeout,UdpSocketTimeoutCallback &&cb)
{
    auto us=std::dynamic_pointer_cast<UdpSocket>(shared_from_this());
    loop_->RunAfter(timeout,[this,cb,us](){     
        cb(us);
    });
}
void UdpSocket::SetTimeoutCallback(int timeout,const UdpSocketTimeoutCallback &cb)
{
    auto us=std::dynamic_pointer_cast<UdpSocket>(shared_from_this());
    loop_->RunAfter(timeout,[this,cb,us](){     
        cb(us);
    });
}


void UdpSocket::EnableCheckIdleTimeout(int32_t max_time)
{
    auto tp=std::make_shared<UdpTimeoutEntry>(
    std::dynamic_pointer_cast<UdpSocket>(shared_from_this()));
    max_idle_time_=max_time;
    timeout_entry_=tp;
    loop_->InsertEntry(max_idle_time_,tp);
}


void UdpSocket::ExtendLife()
{
    auto tp=timeout_entry_.lock();
    if(tp){
        loop_->InsertEntry(max_idle_time_,tp);
    }
}


void UdpSocket::Send(std::list<UdpBufferNodePtr> &list)
{
    loop_->RunInLoop([this,&list](){
        SendInLoop(list);
    });
}

void UdpSocket::Send(const char *buf,size_t size,struct sockaddr *addr,socklen_t sock_len)
{
    loop_->RunInLoop([this,buf,size,addr,sock_len](){
        SendInLoop(buf,size,addr,sock_len);
    });
}
void UdpSocket::SendInLoop(std::list<UdpBufferNodePtr> &list)
{
    for(auto &node:list){
        buffer_list_.emplace_back(node);
    }
    if(!buffer_list_.empty()){
        EnableWrite(true);
    }
    
}

void UdpSocket::SendInLoop(const char *buf,size_t size,struct sockaddr *addr,socklen_t sock_len)
{
    if(buffer_list_.empty()){
        auto ret=::sendto(fd_,buf,size,0,addr,sock_len);
        if(ret > 0){
            return;
        }
    }
    auto node=std::make_shared<UdpBufferNode>((void *)buf,size,addr,sock_len);
    buffer_list_.emplace_back(node);
    EnableWrite(true);
}

void UdpSocket::ForceClose()
{
    loop_->RunInLoop([this](){
        OnClose();
    });
}
