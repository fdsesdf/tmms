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
        struct BufferNode{
            BufferNode(void *buf,size_t s):addr(buf),size(s){}
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
            ~TcpConnection();
            
            void OnClose() override;
            void ForceClose() override;
            void OnError(const std::string &msg) override;
            void OnWrite() override;
            void OnRead() override;

            void Send(std::list<BufferNodePtr> &list);
            void Send(const char *buf,size_t size);
            

            void SetCloseCallback(const CloseConnectionCallback &cb);
            void SetCloseCallback(CloseConnectionCallback &&cb);
            void SetMessageCallback(const MessageCallback &cb);
            void SetMessageCallback(MessageCallback &&cb);
            void SetWriteCompleteCallback(const WriteCompleteCallback &cb);
            void SetWriteCompleteCallback(WriteCompleteCallback &&cb);
            void SetTimeoutCallback(int timeout, const TimeoutCallback &cb);
            void SetTimeoutCallback(int timeout, TimeoutCallback &&cb);
            void OnTimeout();
            void EnableCheckTimeout(int32_t max_time);
        private:
            bool closed_{false};
            void SendInLoop(std::list<BufferNodePtr> &list);
            void SendInLoop(const char *buf,size_t size);
            void ExtendLife();
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