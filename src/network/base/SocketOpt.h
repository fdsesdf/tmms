#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <bits/socket.h>
#include <string>
#include <memory>
#include "InetAddress.h"
namespace tmms
{
    namespace network
    {
        using InetAddressPtr = std::shared_ptr<InetAddress>;
        class SocketOpt
        {
        public:
            SocketOpt(int sock,bool v6 = false);
            
            ~SocketOpt()=default;
            static int CreateNonblockingTcpSocket(int family ); //创建非阻塞的tcp socket
            static int CreateNonblockingUdpSocket(int family ); //创建非阻塞的udp socket
            //服务端相关
            int BindAddress(const InetAddress &localaddr); //绑定地址
            int Listen();//监听
            int Accept(InetAddress &peeraddr);//接受连接

            //客户端相关
            int Connect(const InetAddress &peeraddr); //连接服务器

            InetAddressPtr GetLocalAddress(); //获取本地地址
            InetAddressPtr GetPeerAddress(); //获取对端地址

            //socket选项
            void SetTcpNoDelay(bool on); //设置tcp no delay
            void SetReuseAddr(bool on); //设置reuse addr
            void SetReusePort(bool on); //设置reuse port
            void SetKeepAlive(bool on); //设置keep alive
            void SetNonblocking(bool on); //设置非阻塞
        private:
            int sock_{-1};
            bool is_v6_{false};
        };
    }
}