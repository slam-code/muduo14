

#ifndef TMUDUO_LOGGING_H
#define TMUDUO_LOGGING_H

#include <muduo/base/ChronoTimestamp.h>
#include <muduo/log/LogStream.h>

namespace muduo
{
    class TimeZone;

    class Logger
    {
    public:

        enum LogLevel
        {
            TRACE,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            NUM_LOG_LEVELS,
        };

        class SourceFile
        {
        public:

            template<int N>
            inline SourceFile(const char(&arr)[N])
                    :data_(arr), size_(N - 1)
            {
                const char *slash = strrchr(data_, '/');
                if (slash)
                {
                    data_ = slash + 1;
                    size_ -= static_cast<int>(data_ - arr);
                }
            }

            explicit SourceFile(const char *filename)
                    : data_(filename)
            {
                const char *slash = strrchr(filename, '/');
                if (slash)
                {
                    data_ = slash + 1;
                }
                size_ = static_cast<int >(strlen(data_));
            }


            const char *data_;
            int size_;
        };

        Logger(SourceFile file, int line);

        Logger(SourceFile file, int line, LogLevel level);


        Logger(SourceFile file, int line, LogLevel level, const  char *func);

        Logger(SourceFile file, int line, bool toAbort);

        ~Logger();

        LogStream &stream()
        { return impl_.stream_; }

        static LogLevel logLevel();

        static void setLogLevel(LogLevel level);

        using OutputFunc= void(*)(const char *msg, int len);

        using FlushFunc= void(*)();

        static void setOutput(OutputFunc);

        static void setFlush(FlushFunc);

        static void setTimeZone(const TimeZone &tz);


        class Impl
        {
        public:
            using LogLevel= Logger::LogLevel ;

            Impl(LogLevel level, int old_errno, const SourceFile &file, int line);

            void formatTime();

            void finish();

            ChronoTimestamp time_;
            LogStream stream_;
            LogLevel level_;
            int line_;
            SourceFile basename_;
        };
    private:
        Impl impl_;
    };

    extern Logger::LogLevel g_logLevel;

    inline Logger::LogLevel Logger::logLevel()
    {
        return g_logLevel;
    }

#define LOG_TRACE if(muduo::Logger::logLevel()<=muduo::Logger::TRACE) \
    muduo::Logger(__FILE__,__LINE__,muduo::Logger::TRACE,__func__).stream()

#define LOG_DEBUG if(muduo::Logger::logLevel()<=muduo::Logger::DEBUG)\
     muduo::Logger(__FILE__,__LINE__,muduo::Logger::DEBUG,__func__).stream()

#define LOG_INFO if(muduo::Logger::logLevel()<=muduo::Logger::INFO)  \
         muduo::Logger(__FILE__,__LINE__).stream()


#define LOG_WARM muduo::Logger(__FILE__,__LINE__,muduo::Logger::WARN).stream()

#define LOG_ERROR muduo::Logger(__FILE__,__LINE__,muduo::Logger::ERROR).stream()

//#define LOG_FATAL muduo::Logger(__FILE__,__LINE__,muduo::Logger::FATAL,__func__).stream()

#define LOG_FATAL muduo::Logger(__FILE__,__LINE__,muduo::Logger::FATAL).stream()
#define LOG_SYSERR muduo::Logger(__FILE__,__LINE__, false).stream()

#define LOG_SYSFATAL muduo::Logger(__FILE__,__LINE__,true).stream()

    const char *strerror_tl(int savedErrno);//返回savedErrno对应的字符串描述．

#define CHECK_NOTNULL(val) \
    ::muduo::CheckNotNull(__FILE__,__LINE__," '"#val"'must be non NULL ",(val))

    template<typename T>
    T *CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
    {
        if (ptr == nullptr)
            Logger(file, line, Logger::FATAL).stream() << names;
        return ptr;
    }

}
#endif //TMUDUO_LOGGING_H
