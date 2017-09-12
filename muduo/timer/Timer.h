
#ifndef TMUDUO_TIMER_H
#define TMUDUO_TIMER_H

#include <muduo/base/Atomic.h>
#include <muduo/base/ChronoTimestamp.h>
#include <muduo/base/noncopyable.h>
#include <muduo/net/Callbacks.h>

namespace muduo
{
    namespace net
    {
/*************************************************
Timer是定时器.
*************************************************/
        class Timer : noncopyable
        {
        public:
            Timer(const TimerCallback &cb, ChronoTimestamp when, double interval)
                    : callback_(cb),
                      experation_(when),
                      interval_(interval),
                      repeat_(interval > 0.0),
                      sequence_(++s_numCreated_)
            {
            }

            Timer(TimerCallback &&cb, ChronoTimestamp when, double interval)
                    : callback_(std::move(cb)),
                      experation_(when),
                      interval_(interval),
                      repeat_(interval > 0.0),
                      sequence_(++s_numCreated_)
            {
            }

            void run()
            {
                callback_();
            }

            ChronoTimestamp expiration() const
            {
                return experation_;
            }

            bool repeat() const
            {
                return repeat_;
            }

            int64_t sequence() const
            {
                return sequence_;
            }

            void restart(ChronoTimestamp now);

            static int64_t numCreated()
            {
                return s_numCreated_;
            }



        private:
            const TimerCallback callback_;      //回调函数
            ChronoTimestamp experation_;        //下一次超时时间
            const double interval_;             //超时时间间隔,为0时表示一次性定时器.单位是s
            const bool repeat_;
            const int64_t sequence_;            //定时器序列号
            static AtomicInt64 s_numCreated_;
        };
    }
}
#endif //TMUDUO_TIMER_H
