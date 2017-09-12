

#ifndef TMUDUO_POLLPOLLER_H
#define TMUDUO_POLLPOLLER_H

#include <muduo/net/Poller.h>
#include <vector>

namespace muduo
{
    namespace net
    {
        class PollPoller : public Poller
        {
        public:
            PollPoller(EventLoop *loop);

            virtual ~PollPoller();

            virtual ChronoTimestamp poll(int timeoutMs, ChannelList *activeChannels);

            virtual void updateChannel(Channel *channel);

            virtual void removeChannel(Channel *channel);

        private:
            void fillActivityChannels(int numEvents, ChannelList *activeChannels) const;

            using PollFdList=std::vector<struct pollfd>;
            PollFdList pollfds_;
        };
    }
}
#endif //TMUDUO_POLLPOLLER_H
