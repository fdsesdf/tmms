#include "InetAddress.h"
#include <cstring>
#include <sstream>
#include <network/base/Network.h>
using namespace tmms::network;
void InetAddress::GetIpAndPort(const std::string &host, std::string &ip,std::string &port) 
{
    size_t pos=host.find(':');
    if(pos==std::string::npos){
        ip=host;
        port="0";
    }else{
        ip=host.substr(0,pos);
        port=host.substr(pos+1);
    }
}
InetAddress::InetAddress(const std::string &host, uint16_t port,bool is_v6):addr_(host),port_(std::to_string(port)),is_ipv6_(is_v6){
    GetIpAndPort(host,addr_,port_);
    is_ipv6_=is_v6;
}
InetAddress::InetAddress(const std::string &host,bool is_v6){
    GetIpAndPort(host,addr_,port_);
    is_ipv6_=is_v6;
}
void InetAddress::SetHost(const std::string &host){
    GetIpAndPort(host,addr_,port_);
}
void InetAddress::SetAddr(const std::string &addr){
    addr_=addr;
}
void InetAddress::SetPort(uint16_t port){
    port_=std::to_string(port);
}
void InetAddress::SetIsIPV6(bool is_v6){
    is_ipv6_=is_v6;
}    
            
const std::string &InetAddress::IP() const{
    return addr_;
}           
uint32_t InetAddress::IPv4(const char *ip)const{
    struct sockaddr_in addr_in;
    memset(&addr_in,0,sizeof(addr_in));
    addr_in.sin_family=AF_INET;
    addr_in.sin_port=0;
    if(::inet_pton(AF_INET,ip,&addr_in.sin_addr)<=0){
        NETWORK_LOG_ERROR<<"InetAddress::IPv4:inet_pton error,ip:"<<ip;
    }
    return ntohl(addr_in.sin_addr.s_addr);
}   
uint32_t InetAddress::IPv4() const{
    return IPv4(addr_.c_str());
}
std::string InetAddress::ToIpPort() const{
    std::stringstream ss;
    ss<<addr_<<":"<<port_;
    return ss.str();
}
uint16_t InetAddress::Port() const{
    return std::stoi(port_.c_str());
}   
void InetAddress::GetSockAddr(struct sockaddr *saddr) const{
    if(is_ipv6_){
        struct sockaddr_in6 *addr_in6=(struct sockaddr_in6 *)saddr;
        memset(addr_in6,0x00,sizeof(struct sockaddr_in6));
        addr_in6->sin6_family=AF_INET6;
        addr_in6->sin6_port=htons(std::atoi(port_.c_str()));
        if(::inet_pton(AF_INET6,addr_.c_str(),&addr_in6->sin6_addr)<=0){}
        return ;
    }
        struct sockaddr_in *addr_in=(struct sockaddr_in *)saddr;
        memset(addr_in,0,sizeof(*addr_in));
        addr_in->sin_family=AF_INET;
        addr_in->sin_port=htons(std::atoi(port_.c_str()));
        if(::inet_pton(AF_INET,addr_.c_str(),&addr_in->sin_addr)<=0){}

}

            
bool InetAddress::IsIPV6() const{
    return is_ipv6_;
}
bool InetAddress::IsWanlp() const{
    uint32_t ip = IPv4();

    // 10.0.0.0/8
    if (ip >= IPv4("10.0.0.0") && ip <= IPv4("10.255.255.255"))
        return false;

    // 172.16.0.0/12
    if (ip >= IPv4("172.16.0.0") && ip <= IPv4("172.31.255.255"))
        return false;

    // 192.168.0.0/16
    if (ip >= IPv4("192.168.0.0") && ip <= IPv4("192.168.255.255"))
        return false;

    // 127.0.0.0/8 loopback
    if (ip >= IPv4("127.0.0.0") && ip <= IPv4("127.255.255.255"))
        return false;

    // 169.254.0.0/16 link-local
    if (ip >= IPv4("169.254.0.0") && ip <= IPv4("169.254.255.255"))
        return false;

    return true;
}
bool InetAddress::IsLanlp() const{
    uint32_t a_start=IPv4("10.0.0.0");
    uint32_t a_end=IPv4("10.255.255.255");
    uint32_t b_start=IPv4("172.16.0.0");
    uint32_t b_end=IPv4("172.31.255.255");
    uint32_t c_start=IPv4("192.168.0.0");
    uint32_t c_end=IPv4("192.168.255.255");
    uint32_t ip=IPv4();
    return (ip>=a_start&&ip<=a_end)||(ip>=b_start&&ip<=b_end)||(ip>=c_start&&ip<=c_end);
}
bool InetAddress::IsLoopback() const{
    return addr_=="127.0.0.1";
}


