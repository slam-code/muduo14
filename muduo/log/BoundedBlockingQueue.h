

#ifndef TMUDUO_BOUNDEDBLOCKINGQUEUE_H
#define TMUDUO_BOUNDEDBLOCKINGQUEUE_H

#include <muduo/base/noncopyable.h>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <assert.h>

namespace muduo
{
    template<typename T>
    class BoundedBlockingQueue : noncopyable
    {
    public:
        BoundedBlockingQueue(int maxSize) : maxSize_(maxSize)//, queue_(maxSize)
        {
        };

        void put(const T &x)
        {
            std::lock_guard<std::mutex> g(mutex_);
            while (queue_.size() == maxSize_)//o(1)
            {
                cv_notFull.wait(g, [&]
                { return queue_ != maxSize_; });
            }
            assert(queue_.size() != maxSize_);
            queue_.push_back(x);
            cv_notEmpty.notify_one();
        }

        void put(T &&x)
        {
            std::unique_lock<std::mutex> g(mutex_);
            while (queue_.size() == maxSize_)//o(1)
            {
                cv_notFull.wait(g, [&]
                { return queue_.size() != maxSize_; });
            }
            assert(queue_.size() != maxSize_);
            queue_.push_back(std::move(x));
            cv_notEmpty.notify_one();
        }

        T take()
        {
            std::unique_lock<std::mutex> g(mutex_);
            while (queue_.empty())
            {
                cv_notEmpty.wait(g, [&]
                { return !queue_.empty(); });
            }
            assert(!queue_.empty());
            T front(queue_.front());
            queue_.pop_front();
            cv_notFull.notify_one();
            return front;
        }

        bool full() const
        {
            std::lock_guard<std::mutex> g(mutex_);
            return queue_.size() == maxSize_;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex> g(mutex_);
            return queue_.empty();
        }

        size_t size()
        {

            std::lock_guard<std::mutex> g(mutex_);
            return queue_.size();
        }

        size_t capacity() const
        {

            std::lock_guard<std::mutex> g(mutex_);
            return maxSize_;
        }

    private:
        std::mutex mutex_;
        std::condition_variable cv_notEmpty;
        std::condition_variable cv_notFull;
        std::deque<T> queue_;
        int maxSize_;
    };
}
#endif //TMUDUO_BOUNDEDBLOCKINGQUEUE_H
