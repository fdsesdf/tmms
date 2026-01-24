#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/socket.h>
#include <string>
#include <memory>
namespace tmms
{
    namespace network
    {
        class InetAddress;
        using InetAddressPtr = std::shared_ptr<InetAddress>;

        class InetAddress
        {
        public:
            InetAddress(const std::string &host, uint16_t port,bool is_v6=false);//构造函数，host是主机名，port是端口号，is_v6是是否是IPV6地址
            InetAddress(const std::string &host, bool is_v6=false);//构造函数，host是主机名，is_v6是是否是IPV6地址
            InetAddress()=default;
            ~InetAddress()=default;


            void SetHost(const std::string &host);//设置主机名
            void SetAddr(const std::string &addr);//设置IP地址
            void SetPort(uint16_t port);//设置端口号
            void SetIsIPV6(bool is_v6);//设置是否是IPV6地址
            
            const std::string &IP() const;//获取IP地址
            uint32_t IPv4()const;
            std::string ToIpPort() const;//将IP地址和端口号转换为字符串
            uint16_t Port() const;//获取端口号
            void GetSockAddr(struct sockaddr *saddr) const;//获取sockaddr结构体指针

            
            bool IsIPV6() const;//是否是IPV6地址
            bool IsWanlp() const;//是否是公网地址
            bool IsLanlp() const;//是否是局域网地址
            bool IsLoopback() const;//是否是回环地址

            static void GetIpAndPort(const std::string &host, std::string &ip,std::string &port);//获取IP地址和端口号
        private:
            uint32_t IPv4(const char *ip) const;//将IP地址转换为uint32_t类型
            std::string addr_;//IP地址
            std::string port_;//端口号
            bool is_ipv6_{false};//是否是IPV6地址
        };
    }
}
