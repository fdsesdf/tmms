#include "network/net/Acceptor.h"
#include "network/net/EventLoopThread.h"
#include "network/net/Event.h"
#include <thread>
#include <iostream>
#include "network/net/TcpConnection.h"
#include <cstring>
#include "network/TestContext.h"
#include "network/TcpServer.h"
using namespace tmms::network;
EventLoopThread eventloop_thread;
std::thread th;
using TestContextPtr = std::shared_ptr<TestContext>;
const char *http_request="HTTP/1.0 200 OK\r\nServer: tmms\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";
int main()
{
    eventloop_thread.Run();
    EventLoop *loop = eventloop_thread.Loop();
    if(loop)
    {
        InetAddress listen_addr("192.168.75.132:34444");
        TcpServer server(loop,listen_addr);
        server.SetMessageCallback([](const TcpConnectionPtr&con,MsgBuffer &buf){
            TestContextPtr context = con->GetContext<TestContext>(kNormalContext);
            context->ParseMessage(buf);
        });
        server.SetNewConnectionCallback([&loop](const TcpConnectionPtr &con){
            TestContextPtr context = std::make_shared<TestContext>(con);
            context->SetTestMessageCallback([](const TcpConnectionPtr&con,const std::string&msg){
                std::cout<<"message:"<<msg<<std::endl;
                
            });
            con->SetContext(kNormalContext,context);
            con->SetWriteCompleteCallback([&loop](const TcpConnectionPtr&con){
                std::cout<<"write complete host:"<<con->PeerAddr().ToIpPort()<<std::endl;
                //con->ForceClose();
            });
        });
        server.Start();
        while(1){
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}