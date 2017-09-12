#ifndef TMUDUO_CHRONOTIMESTAMP_H
#define TMUDUO_CHRONOTIMESTAMP_H

#include <chrono>
#include <muduo/base/copyable.h>
#include <string>

namespace muduo
{

    class ChronoTimestamp : public muduo::copyable
    {
    public:
        ChronoTimestamp() : microSecondsSinceEpoch_(0)
        {
        }

        explicit ChronoTimestamp(std::chrono::microseconds us)
        {
            microSecondsSinceEpoch_ = us;
        }

        bool valid() const
        { return microSecondsSinceEpoch_ > std::chrono::microseconds(0); }

        void swap(ChronoTimestamp &that)
        {
            std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
        }


        std::string toString() const;

        std::string toFormattedString(bool showMicroseconds=true) const;

        std::chrono::microseconds microSecondsSinceEpoch()
        {
            return microSecondsSinceEpoch_;
        }
        static ChronoTimestamp now()
        {
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            //auto us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();//微秒数,int64_t
            std::chrono::microseconds nowin_us=std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
            return ChronoTimestamp(nowin_us);
        };


        static ChronoTimestamp invalid()
        {
            return ChronoTimestamp();
        }

        static const int kMicroSecondsPerSecond = 1000 * 1000;
    private:
        std::chrono::microseconds microSecondsSinceEpoch_;
    };



    inline bool operator<(ChronoTimestamp lhs,ChronoTimestamp rhs)
    {
        return lhs.microSecondsSinceEpoch().count()<rhs.microSecondsSinceEpoch().count();
    }

    inline bool operator==(ChronoTimestamp lhs,ChronoTimestamp rhs)
    {
        return lhs.microSecondsSinceEpoch().count()==rhs.microSecondsSinceEpoch().count();
    }

    inline double timeDifference(ChronoTimestamp high,ChronoTimestamp low)//return :seconds
    {
        int64_t  diff=high.microSecondsSinceEpoch().count()-low.microSecondsSinceEpoch().count();

        return static_cast<double>(diff)/ChronoTimestamp::kMicroSecondsPerSecond;
    }
    inline ChronoTimestamp addTime(ChronoTimestamp timestamp, double seconds)
    {
        int64_t delta= static_cast<int64_t >(seconds*ChronoTimestamp::kMicroSecondsPerSecond);
        return ChronoTimestamp(std::chrono::microseconds((timestamp.microSecondsSinceEpoch().count()+delta)));
    }
}


#endif //TMUDUO_CHRONOTIMESTAMP_H
//成员变量microSecondsSinceEpoch_表示到1970-01-01 00:00:00 UTC的微秒数。