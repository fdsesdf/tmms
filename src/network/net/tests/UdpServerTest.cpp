#include "network/net/Acceptor.h"
#include "network/net/EventLoopThread.h"
#include "network/net/Event.h"
#include <thread>
#include <iostream>
#include "network/UdpServer.h"
#include <cstring>
using namespace tmms::network;
EventLoopThread eventloop_thread;

std::thread th;
int main()
{
    eventloop_thread.Run();
    EventLoop *loop = eventloop_thread.Loop();
    if(loop)
    {
        InetAddress listen("192.168.75.132:34444");
        std::shared_ptr<UdpServer> server = std::make_shared<UdpServer>(loop,listen);
        server->SetRecvMsgCallback([&server](const InetAddress &addr,MsgBuffer &buf){
            std::cout<<"host:"<<addr.ToIpPort()<<":"<<buf.peek()<<std::endl;
            struct sockaddr_in6 sock_addr ;
            addr.GetSockAddr((struct sockaddr *)&sock_addr);
            server->Send(buf.peek(),buf.readableBytes(),(struct sockaddr *)&sock_addr,sizeof(sock_addr));
            buf.retrieveAll();
        });
        server->SetCloseCallback([](const UdpSocketPtr &con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" close."<<std::endl;
            }
        });
        server->SetWriteCompleteCallback([](const UdpSocketPtr &con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" write complete."<<std::endl;
            }
        });
      
        server->Start();
        while(1){
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}