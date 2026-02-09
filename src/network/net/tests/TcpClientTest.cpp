#include "network/net/Acceptor.h"
#include "network/net/EventLoopThread.h"
#include "network/net/Event.h"
#include <thread>
#include <iostream>
#include "network/TcpClient.h"
#include <cstring>
using namespace tmms::network;
EventLoopThread eventloop_thread;

std::thread th;
const char *http_request="GET / HTTP/1.0\r\nHost: 192.168.75.132:34444\r\nAccept: */*\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";

int main()
{
    eventloop_thread.Run();
    EventLoop *loop = eventloop_thread.Loop();
    if(loop)
    {
        InetAddress server("192.168.75.132:34444");
        std::shared_ptr<TcpClient> client = std::make_shared<TcpClient>(loop,server);
        client->SetMessageCallback(
        [](const TcpConnectionPtr&con,MsgBuffer &buf){
            std::string msg(buf.peek(), buf.readableBytes());
            std::cout<<"recv msg:"<<msg<<std::endl;
            buf.retrieveAll();
        });

        client->SetCloseCallback([](const TcpConnectionPtr&con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" close."<<std::endl;
            }
        });
        client->SetWriteCompleteCallback([](const TcpConnectionPtr&con){
            if(con)
            {
                std::cout<<"host:"<<con->PeerAddr().ToIpPort()<<" write complete."<<std::endl;
            }
        });
        client->SetConnectedCallback([](const TcpConnectionPtr&con,bool connected){
            if(connected)
            {
                
                auto size= htonl(strlen(http_request));
                con->Send((const char*)&size,sizeof(size));
                con->Send(http_request,strlen(http_request));
            }
        });
        client->Connect();
        while(1){
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}