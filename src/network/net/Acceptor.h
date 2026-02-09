#pragma once
#include "network/base/InetAddress.h"
#include "network/base/SocketOpt.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include <functional>
namespace tmms
{
    namespace network
    {
        using AcceptCallback = std::function<void(int sock, const InetAddress &addr)>;
        class Acceptor:public Event
        {
        public:
            Acceptor(EventLoop *loop, const InetAddress &addr);
            ~Acceptor();

            void SetAcceptCallback(const AcceptCallback &cb);//设置接受回调
            void SetAcceptCallback(const AcceptCallback &&cb);//设置接受回调
            void Start();//启动接受
            void Stop();//停止接受
            void OnRead() override;//读取事件
            void OnError(const std::string & msg) override;//错误事件
            void OnClose() override;//关闭事件
        private:
            void Open();//打开监听套接字
            AcceptCallback accept_cb_;
            InetAddress addr_;
            SocketOpt *socket_opt_{nullptr};
        };
    }
}