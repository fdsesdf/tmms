#pragma once
#include "Connection.h"
#include "network/base/InetAddress.h"
#include "network/base/MsgBuffer.h"
#include <functional>
#include <list>
#include <vector>
#include <sys/uio.h>
namespace tmms
{
    namespace network
    {
        class TcpConnection;
        struct TimeoutEntry;

        using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
        using MessageCallback = std::function<void(const TcpConnectionPtr &, MsgBuffer &buffer)>;
        
        using CloseConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
        using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
        struct TcpBufferNode{
            TcpBufferNode(void *buf,size_t s):addr(buf),size(s){}
            void *addr{nullptr};
            size_t size{0};
        };
        using TimeoutCallback = std::function<void(const TcpConnectionPtr &)>;
        using BufferNodePtr = std::shared_ptr<BufferNode>;
        
        class TcpConnection:public Connection
        {
        public:
            TcpConnection(EventLoop *loop,
                        int socketfd,
                        const InetAddress &locakAddr,
                        const InetAddress &peerAddr);
            virtual ~TcpConnection();
            
            void OnClose() override;//连接关闭时调用
            void ForceClose() override;//强制关闭连接
            void OnError(const std::string &msg) override;//连接错误时调用
            void OnWrite() override;//连接可写时调用
            void OnRead() override;//连接可读时调用

            void Send(std::list<BufferNodePtr> &list);//发送数据
            void Send(const char *buf,size_t size);//发送数据
            

            void SetCloseCallback(const CloseConnectionCallback &cb);//设置连接关闭回调
            void SetCloseCallback(CloseConnectionCallback &&cb);//设置连接关闭回调
            void SetMessageCallback(const MessageCallback &cb);//设置消息回调
            void SetMessageCallback(MessageCallback &&cb);//设置消息回调
            void SetWriteCompleteCallback(const WriteCompleteCallback &cb);//设置写完成回调
            void SetWriteCompleteCallback(WriteCompleteCallback &&cb);//设置写完成回调
            void SetTimeoutCallback(int timeout, const TimeoutCallback &cb);//设置超时回调
            void SetTimeoutCallback(int timeout, TimeoutCallback &&cb);//设置超时回调
            void OnTimeout();//超时回调
            void EnableCheckTimeout(int32_t max_time);//启用超时检查
        private:
            bool closed_{false};//连接是否关闭
            void SendInLoop(std::list<BufferNodePtr> &list);//在事件循环中发送数据
            void SendInLoop(const char *buf,size_t size);//在事件循环中发送数据
            void ExtendLife();//延长连接生命周期
            CloseConnectionCallback close_cb_;
            MsgBuffer message_buffer_;
            MessageCallback message_cb_;
            std::vector<struct iovec> io_vec_list_;
            WriteCompleteCallback write_complete_cb_;
            std::weak_ptr<TimeoutEntry> timeout_entry_;
            int32_t max_idle_time_{30};
        };
        struct TimeoutEntry{
        public:
            TimeoutEntry(const TcpConnectionPtr &c):conn(c)
            {

            }
            ~TimeoutEntry()
            {
                auto c=conn.lock();
                if(c)
                {
                    c->OnTimeout();
                }
            }
            std::weak_ptr<TcpConnection> conn;
        };
    }
}