#pragma once
#include "network/net/UdpSocket.h"
#include <functional>
namespace tmms
{
    namespace network
    {
        using UdpSocketConnectedCallback=std::function<void(const UdpSocketPtr &,bool)>;
        class UdpClient : public UdpSocket
        {
        public:
            UdpClient(EventLoop *loop,const InetAddress &server);
            virtual ~UdpClient();
            void Connect();
            void SetConnectedCallback(const UdpSocketConnectedCallback &cb);
            void SetConnectedCallback( UdpSocketConnectedCallback &&cb);
            void ConnectInLoop();
            void Send(const char *buf,size_t size);
            void Send(std::list<UdpBufferNodePtr> &list);
            void OnClose() override;
        private:
            bool connected_=false;
            InetAddress server_addr_;
            UdpSocketConnectedCallback connected_cb_;
            struct sockaddr_in6 sock_addr;
            socklen_t sock_addr_len=sizeof(sock_addr);
        };
    }
}