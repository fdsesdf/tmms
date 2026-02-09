#include "DnsService.h"
#include <thread>
#include <functional>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cstring>
using namespace tmms::network;
namespace
{
    static InetAddressPtr inet_address_null;
}
void DnsService::AddHost(const std::string &host)
{
    std::lock_guard<std::mutex> l(lock_);
    auto iter=host_info_.find(host);
    if(iter!=host_info_.end())
    {
        return;
    }
    host_info_[host]=std::vector<InetAddressPtr>();
}

InetAddressPtr DnsService::GetHostAddress(const std::string &host,int index)
{
    std::lock_guard<std::mutex> l(lock_);
    auto iter=host_info_.find(host);
    if(iter!=host_info_.end())
    {
        auto list=iter->second;
        if(list.size()>0)
        {
            return list[index%list.size()];
        }
    }
    return inet_address_null;
}
std::vector<InetAddressPtr> DnsService::GetHostAddress(const std::string &host)
{
    std::lock_guard<std::mutex> l(lock_);
    auto iter=host_info_.find(host);
    if(iter!=host_info_.end())
    {
        auto list=iter->second;
        return list;
    }
    return std::vector<InetAddressPtr>();
}
void DnsService::UpdateHost(const std::string &host,  std::vector<InetAddressPtr> &list)
{
    std::lock_guard<std::mutex> l(lock_);
    host_info_[host].swap(list);
}

std::unordered_map<std::string, std::vector<InetAddressPtr>> DnsService::GetHosts()
{
    std::lock_guard<std::mutex> l(lock_);
    return host_info_;
}

void DnsService::SetDnsServiceParams(int32_t retry, int32_t sleep, int32_t interval)
{
    interval_=interval;
    retry_=retry;
    sleep_=sleep;
}

void DnsService::Start()
{
    running_=true;
    thread_=std::thread(std::bind(&DnsService::OnWork, this));
}

void DnsService::Stop()
{
    running_=false;
    if(thread_.joinable())
    {
        thread_.join();
    }
}


void DnsService::GetHostInfo(const std::string &host, std::vector<InetAddressPtr> &list)
{
    struct addrinfo addrinfo,*res;
    memset(&addrinfo, 0, sizeof(addrinfo));
    addrinfo.ai_family=AF_UNSPEC;
    addrinfo.ai_flags=AI_PASSIVE;
    addrinfo.ai_socktype=SOCK_DGRAM;
    auto ret=::getaddrinfo(host.c_str(), nullptr, &addrinfo,&res);
    if(ret==-1||res==nullptr)
    {
        return;
    }
    for(auto p=res;p!=nullptr;p=p->ai_next)
    {
        InetAddressPtr peeraddr=std::make_shared<InetAddress>();
        if(p->ai_addr->sa_family==AF_INET)
        {
            if(p->ai_addr->sa_family == AF_INET){
            char ip[16]={0,};
            struct sockaddr_in *saddr = (struct sockaddr_in *)p->ai_addr;
            ::inet_ntop(AF_INET,&saddr->sin_addr,ip,sizeof(ip));
            peeraddr->SetAddr(ip);
            peeraddr->SetPort(ntohs(saddr->sin_port));
        }else if(p->ai_addr->sa_family == AF_INET6){
            char ip[INET6_ADDRSTRLEN]={0,};
            struct sockaddr_in6 *saddr6 = (struct sockaddr_in6 *)p->ai_addr;
            ::inet_ntop(AF_INET6,&saddr6->sin6_addr,ip,sizeof(ip));
            peeraddr->SetAddr(ip);
            peeraddr->SetPort(ntohs(saddr6->sin6_port));
            peeraddr->SetIsIPV6(true);
        }
        list.push_back(peeraddr);
        }
    }
}
void DnsService::OnWork()
{
    while(running_)
    {
        auto host_infos=GetHosts();
        for(auto &iter:host_infos)
        {
            for(int i=0;i<retry_;i++)
            {
                std::vector<InetAddressPtr> list;
                GetHostInfo(iter.first, list);
                if(list.size()>0)
                {
                    UpdateHost(iter.first, list);
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
    }   
}

