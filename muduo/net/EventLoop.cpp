
#include <muduo/net/EventLoop.h>
#include <muduo/log/Logging.h>
#include <mutex>
#include <muduo/net/Channel.h>
#include <muduo/net/Poller.h>
#include <muduo/net/SocketsOps.h>
#include <muduo/timer/TimerQueue.h>


#include <algorithm>
#include <signal.h>
#include <sys/eventfd.h>

using namespace muduo;
using namespace muduo::net;

namespace
{
    thread_local EventLoop *t_loopInThisThread = 0;
    const int kPollTimeMs = 10000;//默认10s轮询一次．

    int createEventfd()
    {
        int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (evtfd < 0)
        {
            LOG_SYSFATAL << "Failed in eventfd";
            abort();
        }
        return evtfd;
    }


    class IgnoreSigPipe
    {
    public:
        IgnoreSigPipe()
        {
            ::signal(SIGPIPE, SIG_IGN);
        }
    };

    IgnoreSigPipe initObj;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

EventLoop::EventLoop()
        : looping_(false),
          quit_(false),
          eventHandling_(false),
          callingPendingFunctors_(false),
          iteration_(0),
          threadId_(::syscall(SYS_gettid)),
          poller_(Poller::newDefaultPoller(this)),
          timerQueue_(new TimerQueue(this)),
          wakeupFd_(createEventfd()),
          wakeupChannel_(new Channel(this, wakeupFd_)),
          currentActiveChannel_(nullptr)
{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread)
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
    else
        t_loopInThisThread = this;

    wakeupChannel_->setReadCallback(
            std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}


EventLoop::~EventLoop()
{
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_ <<
              " destructs in thread " << ::syscall(SYS_gettid);

    wakeupChannel_->disableAll();
    wakeupChannel_->remove();

    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()//核心环节
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    LOG_TRACE << "EventLoop " << this << " start looping";


    while (!quit_)//调用quit()之后则不再循环
    {
        activeChannels_.clear();//why? 避免过期.

        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);//每3秒轮洵一次,返回活跃的通道
        ++iteration_;
        if (Logger::LogLevel() <= Logger::TRACE)
        {
            printActiveChannels();
        }

        eventHandling_ = true;
        for (ChanneList::iterator it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
        {
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(pollReturnTime_);//开始处理IO事件。
        }

        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

//可以跨线程调用
void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread())
        wakeup();//why this?　
}

void EventLoop::runInLoop(const Functor &cb)
{
    if (isInLoopThread())
        cb();
    else
        queueInLoop(cb);
}

void EventLoop::queueInLoop(const Functor &cb)
{
    {
        std::lock_guard<std::mutex> g(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}

size_t EventLoop::queueSize()
{
    std::lock_guard<std::mutex> g(mutex_);
    return pendingFunctors_.size();
}

TimerId EventLoop::runAt(const ChronoTimestamp &time, const TimerCallback &cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback &cb)
{
    ChronoTimestamp time(addTime(ChronoTimestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback &cb)
{
    ChronoTimestamp time(addTime(ChronoTimestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::cancel(TimerId timerId)
{
    return timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHandling_)
    {
        assert(currentActiveChannel_ == channel ||
               std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}


bool EventLoop::hasChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread -EventLoop " << this
              << " was created in threadId_= " << threadId_ << ", current thread id =" << ::syscall(SYS_gettid);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes insted of 8";
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> g(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();
    }

    callingPendingFunctors_ = false;
}

void EventLoop::printActiveChannels() const
{
    for (auto it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
    {
        const Channel *ch = *it;
        LOG_TRACE << "{" << ch->reventsToString() << "} ";
    }
}