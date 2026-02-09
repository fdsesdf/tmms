#pragma once
#include "network/base/InetAddress.h"
#include "base/NonCopyable.h"
#include "base/Singleton.h"
#include <unordered_map>
#include <mutex>
#include <thread>
#include <vector>
namespace tmms
{
    namespace network
    {
        using InetAddressPtr = std::shared_ptr<InetAddress>;
        class DnsService : public base::NonCopyable
        {
        public:
            DnsService()=default;
            ~DnsService();

            void AddHost(const std::string &host);//添加主机
            InetAddressPtr GetHostAddress(const std::string &host,int index);//获取主机地址
            std::vector<InetAddressPtr> GetHostAddress(const std::string &host);//获取主机地址
            
            void UpdateHost(const std::string &host, std::vector<InetAddressPtr> &list);//更新主机地址
            std::unordered_map<std::string, std::vector<InetAddressPtr>>GetHosts();//获取所有主机地址

            void SetDnsServiceParams(int32_t retry, int32_t sleep, int32_t interval);//设置DNS服务参数
            void Start();//启动DNS服务
            void Stop();//停止DNS服务
            void OnWork();//工作线程
            static void GetHostInfo(const std::string &host, std::vector<InetAddressPtr> &list);//获取主机地址
        private:
            std::thread thread_;//工作线程
            bool running_=false;
            std::mutex lock_;
            std::unordered_map<std::string, std::vector<InetAddressPtr>> host_info_;
            int32_t retry_{3};
            int32_t sleep_{200};
            int32_t interval_{180*1000};
        };
        #define sDnsService tmms::base::Singleton<tmms::network::DnsService>::Instance()
    }
}