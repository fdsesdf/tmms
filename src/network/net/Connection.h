#pragma once
#include "network/base/InetAddress.h"
#include "network/base/SocketOpt.h"
#include "network/net/Event.h"
#include "network/net/EventLoop.h"
#include "network/base/MsgBuffer.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
namespace tmms
{
    namespace network
    {
        class Connection;
        using ConnectionPtr = std::shared_ptr<Connection>;
        using ContextPtr = std::shared_ptr<void>;
        using ActiveCallback = std::function<void(const ConnectionPtr &)>;
        enum{
            kNormalContext = 0,
            kRtmpContext ,
            kHttpContext ,
            kUserContext ,
            kFlcContext ,
        };
        class Connection:public Event
        {
        public:
            Connection(EventLoop *loop, int fd,const InetAddress &locakAddr,const InetAddress &peerAddr);
            virtual~Connection()=default;

            void SetLocalAddress(const InetAddress &local);
            void SetPeerAddress(const InetAddress &peer);
            const InetAddress &LocalAddr() const;
            const InetAddress &PeerAddr() const;

            void SetContext(int type,const std::shared_ptr<void> &context);
            void SetContext(int type,std::shared_ptr<void> &&context);
            template<typename T>
            std::shared_ptr<T> GetContext(int type) const{
                auto iter = contexts_.find(type);
                if(iter != contexts_.end()){
                    return std::dynamic_pointer_cast<T>(iter->second);
                }
                return nullptr;
            }
            void ClearContext(int type);
            void ClearContext();

            void SetActiveCallback(const ActiveCallback &cb);
            void SetActiveCallback(ActiveCallback &&cb);
            void Active();
            void Deactive();

            virtual void ForceClose()=0;
        private:
            std::unordered_map<int,ContextPtr> contexts_;
            ActiveCallback active_cb_;
            
            std::atomic<bool> active_{false};
        protected:
            InetAddress local_addr_;
            InetAddress peer_addr_;
        };
    }
}