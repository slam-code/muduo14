

#include <muduo/net/Channel.h>
#include <muduo/log/Logging.h>
#include <muduo/net/EventLoop.h>

#include <sstream>
#include <poll.h>

using namespace std;
using namespace muduo::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
        : loop_(loop), fd_(fd), events_(0), index_(-1), logHup_(true),
          tied_(false), eventHandling_(false), addedToLoop_(false)
{
}

Channel::~Channel()
{
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->isInLoopThread())
    {
        assert(!loop_->hasChannel(this));
    }
}

void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    loop_->updateChannel(this);
}

void Channel::update()
{
    addedToLoop_ = true;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(ChronoTimestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    } else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(ChronoTimestamp receiveTime)
{
    eventHandling_ = true;
    LOG_TRACE << reventsToString();
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN))
    {
        if (logHup_)
        {
            LOG_WARM << " fd= " << fd_ << " Channel::handle_event() POLLHUP";
        }
        if (closeCallback_)
            closeCallback_();
    }
    if (revents_ & POLLNVAL)
    {
        LOG_WARM << " fd= " <<
                 fd_ << " Channel::handle_event() POLLNVAL";
    }

    if (revents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
            errorCallback_();
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
            readCallback_(receiveTime);
    }

    if (revents_ & POLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
    eventHandling_ = false;
}

std::string Channel::reventsToString() const
{
    return eventsToString(fd_, revents_);
}

std::string Channel::eventsToString() const
{
    return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": "; //文件描述符  <poll.h>
    if (ev & POLLIN)
        oss << "IN "; //可读 0x001
    if (ev & POLLPRI)
        oss << "PRI ";//紧急可读 0x002
    if (ev & POLLOUT)
        oss << "OUT ";//可写 0x004
    if (ev & POLLHUP)
        oss << "HUP ";//挂起 0x0010
    if (ev & POLLRDHUP)
        oss << "RDHUP ";//对端关闭 0x2000
    if (ev & POLLERR)
        oss << "ERR ";//错误 0x008
    if (ev & POLLNVAL)
        oss << "NVAL ";//无效请求:fd未打开.0x0020

    return oss.str().c_str();
}