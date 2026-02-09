#include "network/TcpClient.h"
#include "network/base/Network.h"
#include <iostream>
#include "network/base/SocketOpt.h"
using namespace tmms::network;

TcpClient::TcpClient(EventLoop *loop, const InetAddress &server)
    : TcpConnection(loop, -1, InetAddress(), server),
      server_addr_(server)
{
}

TcpClient::~TcpClient()
{
    
}

void TcpClient::SetConnectedCallback(const ConnectionCallback &cb)
{
    connected_cb_ = cb;
}

void TcpClient::SetDisconnectedCallback(ConnectionCallback &&cb)
{
    connected_cb_ = std::move(cb);
}

void TcpClient::Connect()
{
    NETWORK_LOG_INFO << "TcpClient::Connect called";
    loop_->RunInLoop([this](){ConnectInLoop();});
}

void TcpClient::ConnectInLoop()
{
    

    loop_->AssertInLoopThread();
    fd_ = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    if(fd_ < 0)
    {
        OnClose();
        return;
    }
    status_ = kTcpConStatusConnecting;


    loop_->AddEvent(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
    EnableWrite(true);


    SocketOpt opt(fd_);
    auto ret=opt.Connect(server_addr_);
    if(ret==0){
        UpdateConnectionStatus();
        return;
    }
    else if(ret==-1){
        if(errno!=EINPROGRESS){
            NET_LOG_ERROR<<"connect to server:"<<server_addr_.ToIpPort()<<" error: "<<errno;
            OnClose();
            return;
        }
    }
}

void TcpClient::UpdateConnectionStatus()
{
    status_ = kTcpConStatusConnected;   
    if(connected_cb_){
        connected_cb_(std::dynamic_pointer_cast<TcpClient>(shared_from_this()),true);
    }
}

bool TcpClient::CheckError()
{
    int error = 0;
    socklen_t len = sizeof(error);
    ::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len);
    return error!=0;
    
}

void TcpClient::OnRead()
{
    if(status_ ==kTcpConStatusConnecting){
        if(CheckError()){
            NET_LOG_ERROR<<"connect to server:"<<server_addr_.ToIpPort()<<" error: "<<errno;
            OnClose();
            return;
        }
        UpdateConnectionStatus();
    }
    else if(status_ == kTcpConStatusConnected)
    {
        TcpConnection::OnRead();
    }
}

void TcpClient::OnWrite()
{
    if(status_ ==kTcpConStatusConnecting){
        if(CheckError()){
            NET_LOG_ERROR<<"connect to server:"<<server_addr_.ToIpPort()<<" error: "<<errno;
            OnClose();
            return;
        }
        UpdateConnectionStatus();
    }
    else if(status_ == kTcpConStatusConnected)
    {
        TcpConnection::OnWrite();
    }
}   

void TcpClient::OnClose()
{
    if(status_ ==kTcpConStatusConnecting||status_ == kTcpConStatusConnected)
    {
        loop_->DelEvent(std::dynamic_pointer_cast<TcpConnection>(shared_from_this()));
    }
    status_ = kTcpConStatusDisconnected;
    TcpConnection::OnClose();
}

void TcpClient::Send(std::list<BufferNodePtr>&list)
{
    if(status_ == kTcpConStatusConnected){
        TcpConnection::Send(list);
    }
}

void TcpClient::Send(const char *buf,size_t size)
{
    if(status_ == kTcpConStatusConnected){
        TcpConnection::Send(buf,size);
    }   
}