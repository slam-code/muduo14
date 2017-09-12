

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
    uint64_t howmany;
    //ssize_t read(int fd, void *buf, size_t count);
    ::read(timerfd, &howmany, sizeof(howmany));
    g_loop->quit();
}


/*
int timerfd_create(int clockid, int flags);

int timerfd_settime(int fd, int flags,
                           const struct itimerspec *new_value,
                           struct itimerspec *old_value);

int timerfd_gettime(int fd, struct itimerspec *curr_value);
*/

int main()
{
    EventLoop loop;

    g_loop = &loop;
    timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    LOG_TRACE << "timerfd= " << timerfd;
    Channel *channel(new Channel(&loop, timerfd));

    //使用Channel响应io事件
    channel->setReadCallback(std::bind(timeout, std::placeholders::_1));//,std::placeholders::_1
    channel->enableReading();//产生了哪些操作?

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));

    howlong.it_value.tv_sec = 3;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);


    loop.loop();
    ::close(timerfd);
    LOG_TRACE << "　done ";
}