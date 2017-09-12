
#include <muduo/log/Logging.h>
#include <string.h>
#include <sstream>
#include <errno.h>
#include <stdio.h>
#include <syscall.h>
#include <zconf.h>

namespace muduo
{
    thread_local char t_errnobuf[512];
    thread_local char t_time[32];

    thread_local time_t t_lastScond;

    const char *strerror_tl(int savedErrno)
    {
        return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
    }

    Logger::LogLevel initLogLevel()
    {
        if(1)
        //if (::getenv("MUDUO_LOG_TRACE"))
            return Logger::TRACE;
        else if (::getenv("MUDUO_LOG_DEBUG"))
            return Logger::DEBUG;
        else
            return Logger::INFO;
    }

    Logger::LogLevel g_logLevel = initLogLevel();

    const char *LogLevelName[Logger::NUM_LOG_LEVELS] =//6
            {
                    "TRACE ",
                    "DEBUG ",
                    "INFO  ",
                    "WARN  ",
                    "ERROR ",
                    "FATAL ",
            };


    class T
    {
    public:
        T(const char *str, unsigned len) :
                str_(str), len_(len)
        {
            assert(strlen(str) == len_);
        }

        const char *str_;
        const unsigned len_;
    };

    inline LogStream &operator<<(LogStream &s, T v)
    {
        s.append(v.str_, v.len_);
        return s;
    }

    inline LogStream &operator<<(LogStream &s, const Logger::SourceFile &v)
    {
        s.append(v.data_, v.size_);
        return s;
    }

    void defaultOutput(const char *msg, int len)//默认是标准输出．
    {
        size_t n = fwrite(msg, 1, len, stdout);
        (void) n;
    }

    void defaultFlush()
    {
        fflush(stdout);
    }


    Logger::OutputFunc g_output = defaultOutput;
    Logger::FlushFunc g_flush = defaultFlush;

    Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile &file, int line)
            : time_(ChronoTimestamp::now()),
              stream_(),
              level_(level),
              line_(line),
              basename_(file)
    {
        formatTime();
        std::string s = std::to_string((::syscall(SYS_gettid)));
        s += " ";
        stream_ << T(s.c_str(), s.size());
        stream_ << T(LogLevelName[level], 6);
        if (savedErrno != 0)
            stream_ << strerror(savedErrno) << "(errno=" << savedErrno << ") ";
    }

    void Logger::Impl::formatTime()
    {
        int64_t usSinceEpoch = time_.microSecondsSinceEpoch().count();
        time_t seconds = static_cast<time_t >(usSinceEpoch / ChronoTimestamp::kMicroSecondsPerSecond);
        int us = static_cast<int>(usSinceEpoch % ChronoTimestamp::kMicroSecondsPerSecond);
        if (seconds != t_lastScond)
        {
            t_lastScond = seconds;
            struct tm tm_time;
            //   if (g_logTimeZone.valid())
            if (0)
            {
//                tm_time = g_logTimeZone.toLocalTime(seconds);
            } else
            {
                ::gmtime_r(&seconds, &tm_time);
            }

            int len = snprintf(t_time, sizeof(t_time),
                               "%4d%02d%02d %02d:%02d:%02d",
                               tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                               tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
            assert(len == 17);
            (void) len;
        }

        // if(g_logTimeZone.valid())
        if (0)
        {
            Fmt us_(".%06d ", us);
            assert(us_.length() == 8);
            stream_ << T(t_time, 17) << T(us_.data(), 8);
        } else
        {
            Fmt us_(".%06dZ ", us);
            assert(us_.length() == 9);
            stream_ << T(t_time, 17) << T(us_.data(), 9);
        }
    }

    void Logger::Impl::finish()
    {
        stream_ << " - "<< basename_<< ':'<< line_ << '\n';
    }

    Logger::Logger(SourceFile file, int line): impl_(INFO, 0, file, line)
    {
    }

    //Logger(SourceFile file, int line, LogLevel level, const char *func);

    Logger::Logger (SourceFile file, int line, LogLevel level, const char *func)
            : impl_(level, 0, file, line)
    {
        impl_.stream_ << func << ' ';
    }

    Logger::Logger(SourceFile file, int line, LogLevel level)
            : impl_(level, 0, file, line)
    {
    };

    Logger::Logger(SourceFile file, int line, bool toAbort)
            : impl_(toAbort ? FATAL : ERROR, errno, file, line)
    {}

    Logger::~Logger()
    {
        impl_.finish();
        const LogStream::Buffer &buf(stream().buffer());
        g_output(buf.data(), buf.length());
        if (impl_.level_ == FATAL)
        {
            g_flush();
            abort();
        }
    }

    void Logger::setLogLevel(LogLevel level)
    {
        g_logLevel = level;
    }

    void Logger::setOutput(OutputFunc out)
    {
        g_output = out;
    }

    void Logger::setFlush(FlushFunc flush)
    {
        g_flush = flush;
    }

    void Logger::setTimeZone(const TimeZone &tz)
    {

    }

}