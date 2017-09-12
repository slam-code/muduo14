

#ifndef TMUDUO_EVENTLOOP_H
#define TMUDUO_EVENTLOOP_H


#include <functional>
#include <vector>
#include <mutex>
#include <muduo/base/ChronoTimestamp.h>
#include <muduo/net/Callbacks.h>
#include <muduo/timer/TimerId.h>
#include <boost/any.hpp>
#include <muduo/base/Atomic.h>
#include <syscall.h>

namespace muduo
{
    namespace net
    {
        class Channel;

        class Poller;

        class TimerQueue;
/*************************************************
EventLoop 事件循环
*************************************************/
        class EventLoop : noncopyable
        {
        public:
            using Functor=std::function<void()>;

            EventLoop();

            ~EventLoop();

            void loop();//只能在创建EventLoop对象的线程中调用.不能跨线程调用

            void quit();

            ChronoTimestamp pollReturnTime() const
            {
                return pollReturnTime_;
            }

            int64_t iteration() const
            { return iteration_; }

            void runInLoop(const Functor &cb);

      //      void runInLoop(Functor &&cb);

            void queueInLoop(const Functor &cb);

     //       void queueInLoop(Functor &&cb);

            size_t queueSize() ;


            TimerId runAt(const ChronoTimestamp &time, const TimerCallback &cb);

     //       TimerId runAt(const ChronoTimestamp &time, const TimerCallback &&cb);


            TimerId runAfter(double delay, const TimerCallback &cb);

     //       TimerId runAfter(double delay, const TimerCallback &&cb);

            TimerId runEvery(double interval, const TimerCallback &cb);

     //       TimerId runEvery(double interval, const TimerCallback &&cb);

            void cancel(TimerId timerId);


            void wakeup();

            void updateChannel(Channel *channel);/*在Poller中添加或者更新Channel*/

            void removeChannel(Channel *channel);/*在Poller中移除通道*/

            bool hasChannel(Channel *channel);

            void assertInLoopThread()
            {
                if (!isInLoopThread())
                    abortNotInLoopThread();
            };

            bool isInLoopThread() const
            {
                return threadId_ == (::syscall(SYS_gettid));
            }

            bool eventHandling() const
            {
                return eventHandling_;
            }

            void setContext(const boost::any &context)
            {
                context_ = context;
            }

            const boost::any &getContext() const
            {
                return context_;
            }

            boost::any *getMutableContext()
            {
                return &context_;
            }

            static EventLoop *getEventLoopOfCurrentThread();

        private:

            void abortNotInLoopThread();

            void handleRead();

            void doPendingFunctors();

            void printActiveChannels() const;

            using ChanneList= std::vector<Channel *> ;

            Atomic_bool looping_;
            Atomic_bool quit_;
            Atomic_bool eventHandling_;
            Atomic_bool callingPendingFunctors_;
            int64_t iteration_;
            const pid_t threadId_;/*当前对象所属线程ID*/

            ChronoTimestamp pollReturnTime_;
            std::unique_ptr<Poller> poller_;

            std::unique_ptr<TimerQueue> timerQueue_;
            int wakeupFd_;

            std::unique_ptr<Channel> wakeupChannel_;

            boost::any context_;

            ChanneList activeChannels_;/*Poller返回的活动的Channel*/

            Channel *currentActiveChannel_;/*当前正在处理的活动Channel,是activeChannels_中的一个*/

            std::mutex mutex_;
            std::vector<Functor> pendingFunctors_;

        };
    }
}
#endif //TMUDUO_EVENTLOOP_H
