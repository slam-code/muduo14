

#ifndef TMUDUO_THREAD_H
#define TMUDUO_THREAD_H

#include <thread>
#include <memory>
#include <noncopyable.h>
#include <functional>
#include <string>
#include <Atomic.h>
 namespace muduo
{
    class Thread:noncopyable
    {
    public:
        typedef  std::function<void()>Threadfunc;
        explicit Thread(const Threadfunc& ,const std::string& name=std::string());
        explicit Thread(Threadfunc&& ,const std::string& name=std::string());
        ~Thread();

        static int numCreated(){return numCreated_;}
    private:
        void setDefaultName();
        bool joined_;
        bool started;
        std::string name_;
        static AtomicInt32 numCreated_;

    };
}
#endif //TMUDUO_THREAD_H
