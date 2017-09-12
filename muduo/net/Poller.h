

#ifndef TMUDUO_POLLER_H
#define TMUDUO_POLLER_H

#include <map>
#include <vector>
#include <muduo/base/ChronoTimestamp.h>
#include <muduo/net/EventLoop.h>

namespace muduo
{
    namespace net
    {
        class Channel;

/*************************************************
Poller,不可拷贝,
*************************************************/
        class Poller : noncopyable
        {
        public:

            using ChannelList=std::vector<Channel *>;

            Poller(EventLoop *loop);

            virtual  ~Poller();

            virtual ChronoTimestamp poll(int timeoutMs, ChannelList *activeChannels)=0;

            virtual void updateChannel(Channel *channel)=0;

            virtual void removeChannel(Channel *channel)=0;

            virtual bool hasChannel(Channel *channel) const;

            static Poller *newDefaultPoller(EventLoop *loop);

            void assertInLoopThread() const
            {
                ownerLoop_->assertInLoopThread();
            }

        protected:
            using ChannelMap=std::map<int, Channel *>;     /*key是文件描述符,value是Channel* */
            ChannelMap channels_;
        private:
            EventLoop *ownerLoop_;

        };
    }
}
#endif //TMUDUO_POLLER_H
