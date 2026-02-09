#include "network/net/Acceptor.h"
#include "network/net/EventLoopThread.h"
#include "network/net/Event.h"
#include <thread>
#include <iostream>
#include "network/net/TcpConnection.h"
#include <cstring>
using namespace tmms::network;
EventLoopThread eventloop_thread;
std::thread th;
const char *http_request="HTTP/1.0 200 OK\r\nServer: tmms\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
int main()
{
    eventloop_thread.Run();
    EventLoop *loop = eventloop_thread.Loop();
    if(loop)
    {
        std::vector<TcpConnectionPtr> list;
        InetAddress server("192.168.75.132:34444");
        std::shared_ptr acceptor = std::make_shared<Acceptor>(loop,server);
        acceptor->SetAcceptCallback([&loop,&server,&list](int fd,const InetAddress & addr){
            std::cout<<"host:"<<addr.ToIpPort()<<std::endl;
            TcpConnectionPtr connection = std::make_shared<TcpConnection>(loop,fd,server,addr);
            connection->SetMessageCallback([](const TcpConnectionPtr&con,MsgBuffer &buf){
                std::cout<<"recv msg:"<<buf.peek()<<std::endl;
                buf.retrieveAll();
                con->Send(http_request,strlen(http_request));
            });
            connection->SetWriteCompleteCallback([&loop](const TcpConnectionPtr&con){
                std::cout<<"write complete host:"<<con->PeerAddr().ToIpPort()<<std::endl;
                loop->DelEvent(con);
                con->ForceClose();
            });
            //connection->EnableCheckTimeout(3);
            list.push_back(connection);
            loop->AddEvent(connection);
        });
        acceptor->Start();
        while(1){
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}