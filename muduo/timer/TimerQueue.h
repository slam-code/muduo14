
#ifndef TMUDUO_TIMERQUEUE_H
#define TMUDUO_TIMERQUEUE_H

#include <set>
#include <vector>
#include <mutex>
#include <sys/timerfd.h>

#include <muduo/base/ChronoTimestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/net/Channel.h>

namespace muduo
{
    namespace net
    {
        class EventLoop;

        class Timer;

        class TimerId;
/*************************************************
定时器队列.
*************************************************/
        class TimerQueue : noncopyable
        {
        public:
            TimerQueue(EventLoop *loop);

            ~TimerQueue();

            TimerId addTimer(const TimerCallback &cb, ChronoTimestamp when, double interval);

            //TO DO: &&
            //TimerId addTimer(ChronoTimestamp &&cb, ChronoTimestamp when, double interval);

            void cancel(TimerId timerId);


        private:

            using Entry= std::pair<ChronoTimestamp, Timer *>;

            using TimerList= std::set<Entry>;

            using ActiveTimer= std::pair<Timer *, int64_t>;

            using ActiveTimerSet= std::set<ActiveTimer>;

            void addTimerInLoop(Timer *timer);

            void cancelInLoop(TimerId timerId);

            void handleRead();

            std::vector<TimerQueue::Entry> getExpired(ChronoTimestamp now);

            void reset(const std::vector<Entry> &expired, ChronoTimestamp now);

            bool insert(Timer *timer);

            EventLoop *loop_;

            const int timerfd_;
            Channel timerfdChannel_;
            TimerList timers_;

            ActiveTimerSet activeTimers_;

            bool callingExpiredTimers_;

            ActiveTimerSet cancelingTimers_;


        };
    }
}

#endif //TMUDUO_TIMERQUEUE_H
