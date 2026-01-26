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

            void SetAcceptCallback(const AcceptCallback &cb);
            void SetAcceptCallback(const AcceptCallback &&cb);
            void Start();
            void Stop();
            void OnRead() override;
            void OnError(const std::string & msg) override;
            void OnClose() override;
        private:
            void Open();
            AcceptCallback accept_cb_;
            InetAddress addr_;
            SocketOpt *socket_opt_{nullptr};
        };
    }
}