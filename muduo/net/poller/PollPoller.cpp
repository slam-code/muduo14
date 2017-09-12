
#include <muduo/log/Logging.h>
#include <muduo/base/Types.h>
#include <muduo/net/Channel.h>
#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <muduo/net/poller/PollPoller.h>

using namespace muduo;
using namespace muduo::net;

PollPoller::PollPoller(EventLoop *loop)
        : Poller(loop)//,pollfds_()
{};

PollPoller::~PollPoller()
{}

ChronoTimestamp PollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    int savedErrno = errno;

    ChronoTimestamp now(ChronoTimestamp::now());
    if (numEvents > 0)
    {
        LOG_TRACE << numEvents << " events happended";
        fillActivityChannels(numEvents, activeChannels);    /*将pollfds_中的事件返回到活动通道中*/
    } else if (numEvents == 0)
    {
        LOG_TRACE << " nothing happended";
    } else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_SYSERR << "PollPoller::poll()";
        }
    }
    return now;
}

void PollPoller::fillActivityChannels(int numEvents, ChannelList *activeChannels) const
{
    for (auto it = pollfds_.begin(); it != pollfds_.end(); ++it)
    {
        if (it->revents > 0)
        {
            --numEvents;
            auto ch = channels_.find(it->fd);       //找到对应的文件描述符
            assert(ch != channels_.end());
            Channel *channel = ch->second;          //找到对应的Channel
            assert(channel->fd() == it->fd);
            channel->set_revents(it->revents);      //设置poll/epoll的返回事件
            activeChannels->push_back(channel);     //注册到活动通道中
        }
    }
}

void PollPoller::updateChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd() << " events =" << channel->events();
    if (channel->index() < 0)//说明channel是一个新通道
    {
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short >(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);//pollfds_ ->  std::vector<struct pollfd>;
        int idx = static_cast<int >(pollfds_.size() - 1);
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else//说明已有channel通道
    {
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int idx = channel->index();
        assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
        struct pollfd &pfd = pollfds_[idx];
        assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd() - 1);
        pfd.events = static_cast<short >(channel->events());
        pfd.revents = 0;

        /*将一个通道暂时更改为不关注事件,但不从Poller中移除,目的:为了removeChannel优化.pfd.fd可以设置为-1(不再关注) */
        if (channel->isNoneEvent())
            pfd.fd = -channel->fd() - 1;
    }
}


void PollPoller::removeChannel(Channel *channel)
{
    Poller::assertInLoopThread();
    LOG_TRACE << "fd= " << channel->fd();
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channels_[channel->fd()] == channel);
    assert(channel->isNoneEvent());
    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int >(pollfds_.size()));

    const struct pollfd &pfd = pollfds_[idx];
    assert(pfd.fd == -channel->fd() - 1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    assert(n == 1);
    if (implicit_cast<size_t>(idx) == pollfds_.size() - 1)
    {
        pollfds_.pop_back();
    } else//移除算法,O(1)
    {
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);

        if (channelAtEnd < 0)                   //主要是为了下一步更新channels_ ->  std::map<int, Channel *>;
            channelAtEnd = -channelAtEnd - 1;   //还原fd

        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}