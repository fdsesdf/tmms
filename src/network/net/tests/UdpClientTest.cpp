#include "network/net/Acceptor.h"
#include "network/net/EventLoopThread.h"
#include "network/net/Event.h"
#include <thread>
#include <iostream>
#include "network/UdpClient.h"
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
        InetAddress server("192.168.75.132:34444");
        std::shared_ptr<UdpClient> client = std::make_shared<UdpClient>(loop,server);
        client->SetRecvMsgCallback([](const InetAddress &addr,MsgBuffer &buf){
            std::cout<<"host:"<<addr.ToIpPort()<<":"<<buf.peek()<<std::endl;
            buf.retrieveAll();
        });
        client->SetCloseCallback([](const UdpSocketPtr &con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" close."<<std::endl;
            }
        });
        client->SetWriteCompleteCallback([](const UdpSocketPtr &con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" write complete."<<std::endl;
            }
        });
        client->SetConnectedCallback([&client](const UdpSocketPtr &con,bool connected){
            if(connected)
            {
                client->Send("1111",strlen("1111"));
            }
        });
        client->Connect();
        while(1){
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}