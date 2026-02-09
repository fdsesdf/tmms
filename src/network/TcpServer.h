#pragma once

#include "network/net/TcpConnection.h"
#include "network/net/Acceptor.h"
#include "network/net/EventLoop.h"
#include "network/base/Network.h"
#include <functional>
#include <memory>
#include <unordered_set>

namespace tmms 
{
    namespace network 
    {
        using NewConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
        using DestoryConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
        class TcpServer 
        {
            public:
                TcpServer(EventLoop *loop, const InetAddress &addr);
                virtual~TcpServer();
                void SetNewConnectionCallback(const NewConnectionCallback &cb);//设置新连接回调
                void SetNewConnectionCallback( NewConnectionCallback &&cb);//设置新连接回调
                void SetDestoryConnectionCallback(const DestoryConnectionCallback &cb);//设置关闭连接回调
                void SetDestoryConnectionCallback(DestoryConnectionCallback &&cb);//设置关闭连接回调
                void SetActiveCallback(const ActiveCallback &cb);//设置活动回调
                void SetActiveCallback(ActiveCallback &&cb);//设置活动回调
                void SetWriteCompleteCallback(const WriteCompleteCallback &cb);//设置写完成回调
                void SetWriteCompleteCallback(WriteCompleteCallback &&cb);//设置写完成回调
                void SetMessageCallback(const MessageCallback &cb);//设置消息回调
                void SetMessageCallback(MessageCallback &&cb);//设置消息回调
                virtual void Start();//启动服务器
                virtual void Stop();//停止服务器

                void OnAccept(int fd,const InetAddress &addr);//接受新连接
                void OnConnectionClose(const TcpConnectionPtr &con);//关闭连接
            private:
                EventLoop *loop_{nullptr};
                std::shared_ptr<Acceptor> acceptor_;
                InetAddress addr_;
                NewConnectionCallback new_connection_cb_;
                std::unordered_set<TcpConnectionPtr> connections_;
                MessageCallback message_cb_;
                ActiveCallback active_cb_;
                WriteCompleteCallback write_complete_cb_;
                DestoryConnectionCallback destory_connection_cb_;
        };
    }
}
