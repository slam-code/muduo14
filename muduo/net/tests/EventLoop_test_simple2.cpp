

#include <sys/timerfd.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>
#include <muduo/log/Logging.h>

using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;
EventLoop *g_loop;
int timerfd;

void timeout(ChronoTimestamp receivetime)
{
    printf("time out! \n");
    uint64_t  howmany;
    ::read(timerfd,&howmany, sizeof(howmany));
    g_loop->quit();
}
int main()
{
/*    EventLoop loop;
    g_loop=&loop;*/

    g_loop=new EventLoop;
    EventLoop & loop=*g_loop;
    timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    Channel *channel(new Channel(&loop,timerfd));
    channel->setReadCallback(std::bind(timeout,std::placeholders::_1));//,std::placeholders::_1
    channel->enableReading();
    struct  itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec=3;
    ::timerfd_settime(timerfd,0,&howlong, nullptr);
    loop.loop();
    ::close(timerfd);
    delete g_loop;
   // delete  channel;
    LOG_TRACE<<"　done ";
}