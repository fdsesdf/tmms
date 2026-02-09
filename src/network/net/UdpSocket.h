#pragma once
#include "network/base/InetAddress.h"
#include "network/base/MsgBuffer.h"
#include "network/net/EventLoop.h"
#include "network/net/Connection.h"
#include <list>
#include <functional>
#include <memory>

namespace tmms
{
    namespace network
    {
        class UdpSocket;
        using UdpSocketPtr = std::shared_ptr<UdpSocket>;
        using InetAddressPtr = std::shared_ptr<InetAddress>;
        struct UdpTimeoutEntry;
        using MessageCallback = std::function<void(const InetAddress &,MsgBuffer &)>;
        using UdpSocketWriteCallback = std::function<void(const UdpSocketPtr &)>;
        using UdpSocketCloseCallback = std::function<void(const UdpSocketPtr &)>;
        using UdpSocketTimeoutCallback = std::function<void(const UdpSocketPtr &)>;
        struct UdpBufferNode:public BufferNode{
            UdpBufferNode(void *buf,size_t s,struct sockaddr *addr,socklen_t sock_len):BufferNode(buf,s),sock_addr(addr),sock_len_(sock_len){}
            struct sockaddr *sock_addr{nullptr};
            socklen_t sock_len_{0};
        };
        using UdpBufferNodePtr = std::shared_ptr<UdpBufferNode>;
        class UdpSocket : public Connection
        {
        public:
            UdpSocket(EventLoop *loop, 
                int socketfd, 
                const InetAddress &localaddr, 
                const InetAddress &peeraddr);
            ~UdpSocket();

            void SetCloseCallback(const UdpSocketCloseCallback &cb);//设置关闭回调
            void SetCloseCallback(UdpSocketCloseCallback &&cb);//设置关闭回调
            void SetRecvMsgCallback(const MessageCallback &cb);//设置接收消息回调
            void SetRecvMsgCallback(MessageCallback &&cb);//设置接收消息回调
            void SetWriteCompleteCallback(const UdpSocketWriteCallback &cb);//设置写入完成回调
            void SetWriteCompleteCallback(UdpSocketWriteCallback &&cb);//设置写入完成回调
            void SetTimeoutCallback(int timeout,const UdpSocketTimeoutCallback &cb);//设置超时回调
            void SetTimeoutCallback(int timeout,UdpSocketTimeoutCallback &&cb);//设置超时回调

            void OnTimeout();//超时回调
            void OnError(const std::string &msg) override;//错误回调
            void OnRead() override;//读取回调
            void OnWrite() override;//写入回调
            void OnClose() override;//关闭回调
            void EnableCheckIdleTimeout(int32_t max_time);//启用空闲超时检查
            void Send(std::list<UdpBufferNodePtr> &list);//发送数据
            void Send(const char *buf,size_t size,struct sockaddr *addr,socklen_t sock_len);//发送数据
            void SendInLoop(std::list<UdpBufferNodePtr> &list);//发送数据
            void SendInLoop(const char *buf,size_t size,struct sockaddr *addr,socklen_t sock_len);//发送数据
            void ForceClose() override;//强制关闭
        private:
            void ExtendLife();//延长生命周期
            std::list<UdpBufferNodePtr> buffer_list_;
            bool closed_{false};//是否关闭
            int32_t max_idle_time_{30};//最大空闲时间
            std::weak_ptr<UdpTimeoutEntry> timeout_entry_;//超时条目
            MsgBuffer msg_buffer_;//消息缓冲区
            int32_t message_buffer_size_{65535};//消息缓冲区大小
            MessageCallback message_callback_;//接收消息回调
            UdpSocketWriteCallback write_complete_callback_;//写入完成回调
            UdpSocketCloseCallback close_callback_;//关闭回调
        };
        struct UdpTimeoutEntry{
        public:
            UdpTimeoutEntry(const UdpSocketPtr &c):conn(c)
            {

            }
            ~UdpTimeoutEntry()
            {
                auto c=conn.lock();
                if(c)
                {
                    c->OnTimeout();
                }
            }
            std::weak_ptr<UdpSocket> conn;
        };
    }
}