
#include <stdio.h>
#include <muduo/net/Poller.h>
#include <muduo/net/poller/PollPoller.h>
using namespace muduo::net;
Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if(::getenv("USE_EPOOL"))
    {
        return static_cast<Poller* >(0)/*new Poller(loop)*/;
    }
    else
    {
        return new PollPoller(loop);
    }
}