#include "SocketOpt.h"
#include "network/base/Network.h"
#include "InetAddress.h"
#include <fcntl.h>
using namespace tmms::network;
SocketOpt::SocketOpt(int sock,bool v6)
    :sock_(sock),is_v6_(v6)
{
}
int SocketOpt::CreateNonblockingTcpSocket(int family )
{
    int sock = ::socket(family,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
    if(sock < 0){
        NETWORK_LOG_ERROR<<"socket failed.";
    }
    return sock;
}


int SocketOpt::CreateNonblockingUdpSocket(int family )
{
    int sock = socket(family,SOCK_DGRAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_UDP);
    if(sock < 0)
    {
        NETWORK_LOG_ERROR<<"socket failed.";
    }
    return sock;
}

int SocketOpt::BindAddress(const InetAddress &localaddr)
{
    int ret = 0;
    if(localaddr.IsIPV6())
    {
        struct sockaddr_in6 addr;
        localaddr.GetSockAddr((struct sockaddr *)&addr);
        socklen_t size = sizeof(addr);
        return ::bind(sock_,(struct sockaddr *)&addr,size);
    }
    else{
        struct sockaddr_in addr;
        localaddr.GetSockAddr((struct sockaddr *)&addr);
        socklen_t size = sizeof(addr);
        return ::bind(sock_,(struct sockaddr *)&addr,size);
    }
    return ret;
}
int SocketOpt::Listen()
{
    return ::listen(sock_,SOMAXCONN);
}
int SocketOpt::Accept(InetAddress &peeraddr)
{
    struct sockaddr_in6 addr;
    socklen_t size = sizeof(addr);
    int connfd = ::accept4(sock_,(struct sockaddr *)&addr,&size,SOCK_NONBLOCK|SOCK_CLOEXEC);
    if(connfd > 0)
    {
        if(addr.sin6_family == AF_INET){
            char ip[16]={0,};
            struct sockaddr_in *in = (struct sockaddr_in *)&addr;
            ::inet_ntop(AF_INET,&in->sin_addr,ip,sizeof(ip));
            peeraddr.SetAddr(ip);
            peeraddr.SetPort(ntohs(in->sin_port));
        }else if(addr.sin6_family == AF_INET6){
            char ip[INET6_ADDRSTRLEN]={0,};
            struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)&addr;
            ::inet_ntop(AF_INET6,&in6->sin6_addr,ip,sizeof(ip));
            peeraddr.SetAddr(ip);
            peeraddr.SetPort(ntohs(in6->sin6_port));
            peeraddr.SetIsIPV6(true);
        }
    }
    return connfd;
}


int SocketOpt::Connect(const InetAddress &peeraddr)
{
    struct sockaddr_in6 addr;
    peeraddr.GetSockAddr((struct sockaddr *)&addr);
    socklen_t size = sizeof(addr);
    return ::connect(sock_,(struct sockaddr *)&addr,size);
}

InetAddressPtr SocketOpt::GetLocalAddress()
{
    struct sockaddr_in6 addr;
    socklen_t size = sizeof(addr);
    ::getsockname(sock_,(struct sockaddr *)&addr,&size);
    InetAddressPtr localaddr = std::make_shared<InetAddress>();
    if(addr.sin6_family == AF_INET){
        char ip[16]={0,};
        struct sockaddr_in *in = (struct sockaddr_in *)&addr;
        ::inet_ntop(AF_INET,&in->sin_addr,ip,sizeof(ip));
        localaddr->SetAddr(ip);
        localaddr->SetPort(ntohs(in->sin_port));
    }else if(addr.sin6_family == AF_INET6){
        char ip[INET6_ADDRSTRLEN]={0,};
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)&addr;
        ::inet_ntop(AF_INET6,&in6->sin6_addr,ip,sizeof(ip));
        localaddr->SetAddr(ip);
        localaddr->SetPort(ntohs(in6->sin6_port));
        localaddr->SetIsIPV6(true);
    }
    return localaddr;
}

InetAddressPtr SocketOpt::GetPeerAddress()
{
    struct sockaddr_in6 addr;
    socklen_t size = sizeof(addr);
    ::getpeername(sock_,(struct sockaddr *)&addr,&size);
    InetAddressPtr peeraddr = std::make_shared<InetAddress>();
    if(addr.sin6_family == AF_INET){
        char ip[16]={0,};
        struct sockaddr_in *in = (struct sockaddr_in *)&addr;
        ::inet_ntop(AF_INET,&in->sin_addr,ip,sizeof(ip));
        peeraddr->SetAddr(ip);
        peeraddr->SetPort(ntohs(in->sin_port));
    }else if(addr.sin6_family == AF_INET6){
        char ip[INET6_ADDRSTRLEN]={0,};
        struct sockaddr_in6 *in6 = (struct sockaddr_in6 *)&addr;
        ::inet_ntop(AF_INET6,&in6->sin6_addr,ip,sizeof(ip));
        peeraddr->SetAddr(ip);
        peeraddr->SetPort(ntohs(in6->sin6_port));
        peeraddr->SetIsIPV6(true);
    }
    return  peeraddr;
}

void SocketOpt::SetTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sock_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof(optval));
}

void SocketOpt::SetReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sock_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
}

void SocketOpt::SetReusePort(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sock_,SOL_SOCKET,SO_REUSEPORT,&optval,sizeof(optval));
}

void SocketOpt::SetKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sock_,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
}

void SocketOpt::SetNonblocking(bool on)
{
    int flag=::fcntl(sock_,F_GETFL,0);
    if(on){
        flag|=O_NONBLOCK;
    }else{
        flag&=~O_NONBLOCK;
    }
    ::fcntl(sock_,F_SETFL,flag);
        
}