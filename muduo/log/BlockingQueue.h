

#ifndef TMUDUO_BLOCKINGQUEUE_H
#define TMUDUO_BLOCKINGQUEUE_H

#include <condition_variable>
#include <mutex>
#include <deque>
#include <assert.h>
#include <muduo/base/noncopyable.h>
#include <muduo/base/Atomic.h>
namespace muduo
{
    template<typename T>
    class BlockingQueue : noncopyable//无界队列．
    {
    public:
        BlockingQueue()
        {
        }

        void put(const T &x)
        {
            std::lock_guard<std::mutex> g(mutex_);
            queue_.push_back(x);
            cv.notify_one();
        }

        void put(T &&x)
        {
            std::lock_guard<std::mutex>  g(mutex_);
            queue_.push_back(std::move(x));
            cv.notify_one();
        }

        T take()
        {

            std::unique_lock<std::mutex> g(mutex_);
            while (queue_.empty())
            {
               // cv.wait(g,std::bind(&BlockingQueue::notEmpty,this));
                cv.wait(g,[&]{return !queue_.empty();});
            }

            assert(!queue_.empty());
            T front(std::move(queue_.front()));
            queue_.pop_front();
            return front;
        }

        size_t size()
        {
            std::lock_guard<std::mutex> g(mutex_);
            return queue_.size();
        }



    private:

  /*      bool notEmpty()
        {
            //std::lock_guard<std::mutex>  g(mutex_);//不能再加锁了.否则造成死锁.
            return  !queue_.empty();
        }*/
        std::mutex mutex_;
        std::condition_variable cv;
        std::deque<T> queue_;
    };
}
#endif //TMUDUO_BLOCKINGQUEUE_H
