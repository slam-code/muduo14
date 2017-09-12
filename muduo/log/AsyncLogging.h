

#ifndef TMUDUO_ASYNCLOGGING_H
#define TMUDUO_ASYNCLOGGING_H

#include <thread>
#include <condition_variable>
//#include <BlockingQueue.h>
//#include <BoundedBlockingQueue.h>
#include <muduo/log/LogStream.h>
#include <string>
#include <vector>
#include <muduo/base/Atomic.h>

namespace muduo
{
    class AsyncLogging : noncopyable
    {
    public:
        AsyncLogging(const std::string &basename, size_t rollSize, int flushInterval = 3);

        ~AsyncLogging()
        {
            if (running_)
                stop();
        }

        void append(const char *logline, int len);

        void start()
        {
            running_ = true;
            thread_ptr.reset(new std::thread(&AsyncLogging::threadFunc, this));


        }

        void stop()
        {
            running_ = false;
            thread_ptr->join();

        }

    private:
        void threadFunc();

        typedef muduo::detail::FixedBuffer<muduo::detail::kLargeBuffer> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        const int flushInterval_;
        std::string basename_;
        size_t rollSize_;
        std::mutex mutex_;
        std::condition_variable cv_;

/*************************************************
前后端各２块缓冲区．
*************************************************/
        BufferPtr currentBuffer_;//当前缓冲(前端)
        BufferPtr nextBuffer_;//预备缓冲(前端)
        BufferVector buffers_;//待写入文件的缓冲,(后端).A
        Atomic_bool running_{false};
        std::unique_ptr<std::thread> thread_ptr;

    };
}
#endif //TMUDUO_ASYNCLOGGING_H
