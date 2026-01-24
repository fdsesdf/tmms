#include "network/base/SocketOpt.h"
#include "network/base/InetAddress.h"
#include <iostream>
using namespace tmms::network;
void TestClient()
{
    int sock = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    if(sock<0){
        std::cerr<<"create socket failed"<<sock<<std::endl;
    }
    InetAddress server("192.168.75.132:34444");
    SocketOpt opt(sock);
    opt.SetNonblocking(false);
    auto ret= opt.Connect(server);
    std::cout<<"connect ret:"<<ret<<"error:"<<errno<<std::endl
            <<"local:"<<opt.GetLocalAddress()<<std::endl
            <<"peer:"<<opt.GetPeerAddress()<<std::endl;
}
void TestServer()
{
    int sock = SocketOpt::CreateNonblockingTcpSocket(AF_INET);
    if(sock<0){
        std::cerr<<"create socket failed"<<sock<<std::endl;
    }
    InetAddress server("0.0.0.0:34444");
    SocketOpt opt(sock);
    opt.SetNonblocking(false);
    opt.BindAddress(server);
    opt.Listen();
    InetAddress addr;
    auto ns=opt.Accept(addr);
    
    std::cout<<"Accept ret:"<<ns<<"error:"<<errno<<std::endl
            <<"local:"<<addr.ToIpPort()<<std::endl
            <<"peer:"<<addr.ToIpPort()<<std::endl;
}
int main()
{
    TestServer();
    return 0;

}
