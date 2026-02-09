#include "network/DnsService.h"
#include <iostream>
using namespace tmms::network;

int main()
{
    std::vector<InetAddressPtr> list;
    sDnsService->AddHost("www.baidu.com");
    sDnsService->Start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    list=sDnsService->GetHostAddress("www.baidu.com");
    for(auto &iter:list)
    {
        std::cout<<"ip:"<<iter->ToIpPort()<<std::endl;
    }
    return 0;
}
