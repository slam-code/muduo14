
#ifndef TMUDUO_TIMERID_H
#define TMUDUO_TIMERID_H

#include <muduo/base/copyable.h>

namespace muduo
{
    namespace net
    {
        class Timer;
/*************************************************
Timer,TimerId,TimerQueue三者是定时器.后两个是内部实现,外部接口只可见到TimerId
*************************************************/
        class TimerId : public muduo::copyable
        {
        public:
            TimerId()
                    : timer_(nullptr),
                      sequence_(0)
            {}

            TimerId(Timer *timer, int64_t seq)
                    : timer_(timer),
                      sequence_(seq)
            {}

            friend  class TimerQueue;
        private:
            Timer *timer_;
            int64_t sequence_;
        };
    }
}
#endif //TMUDUO_TIMERID_H
