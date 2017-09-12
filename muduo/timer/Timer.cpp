
#include <muduo/timer/Timer.h>

using namespace muduo;
using namespace muduo::net;
AtomicInt64 Timer::s_numCreated_(0);

void Timer::restart(ChronoTimestamp now)
{
    if (repeat_)
        experation_ = addTime(now,interval_);
    else
        experation_ = ChronoTimestamp::invalid();
}
