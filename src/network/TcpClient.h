#pragma once
#include "network/net/TcpConnection.h"
#include "network/net/EventLoop.h"
#include "network/base/InetAddress.h"
#include <functional>

namespace tmms
{
    namespace network
    {
        enum
        {
            kTcpConStatusInit = 0,
            kTcpConStatusConnecting = 1,
            kTcpConStatusConnected = 2,
            kTcpConStatusDisconnected = 3,
        };
        using ConnectionCallback = std::function<void(const TcpConnectionPtr &con,bool)>;
        class TcpClient:public TcpConnection
        {
        public:
            TcpClient(EventLoop *loop, const InetAddress &server);
            virtual ~TcpClient();

            void SetConnectedCallback(const ConnectionCallback &cb);//连接成功回调
            void SetDisconnectedCallback(ConnectionCallback &&cb);//连接断开回调
            void Connect();//连接服务器
            
            void OnRead() override;//读取事件
            void OnWrite() override;//写入事件
            void OnClose() override;//关闭事件

            void Send(std::list<BufferNodePtr>&list);//发送数据
            void Send(const char *buf,size_t size);//发送数据
        private:
            void ConnectInLoop();//连接服务器
            void UpdateConnectionStatus();//更新连接状态
            bool CheckError();//检查错误
            InetAddress server_addr_;
            int32_t status_{kTcpConStatusInit};
            ConnectionCallback connected_cb_;
        };
    }
}