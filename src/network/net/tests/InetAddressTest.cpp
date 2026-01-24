#include "network/base/InetAddress.h"
#include <string>
#include <iostream>
using namespace tmms::network;

int main(int argc, char *argv[])
{
    std::string host;
    while(std::cin>>host){
        InetAddress addr(host);
        
        std::cout<<"host:"<<host<<std::endl<<"ip:"<<addr.IP()<<std::endl<<"port:"<<addr.Port()<<std::endl<<"lan:"<<addr.IsLanlp()<<std::endl<<"wan:"<<addr.IsWanlp()<<std::endl <<"loopback:"<<addr.IsLoopback()<<std::endl;

            }   
    return 0;
}