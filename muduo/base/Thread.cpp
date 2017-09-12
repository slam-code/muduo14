
#include <Thread.h>

namespace muduo
{
    namespace CurrentThread
    {
        thread_local int t_cachedTid = 0;
        thread_local char t_tidString[32];
        thread_local const char *t_threadName = "unknown";
        static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");
    }

    namespace detail
    {
        pid_t gettid()
        { return static_cast<pid_t >(::syscall(SYS_gettid)); }

        void afterFork()
        {
            CurrentThread::t_cachedTid = 0;
            CurrentThread::t_threadName = "main";
            CurrentThread::tid();
        }

        class ThreadNameInitializer
        {
        public:
            ThreadNameInitializer()
            {
                CurrentThread::t_threadName = "main";
                CurrentThread::tid();
                pthread_atfork(NULL, NULL, &afterFork);
            }
        };

        ThreadNameInitializer init;

        class ThreadData
        {
            typedef muduo::Thread::Threadfunc ThreadFunc;
            ThreadFunc func_;
            std::string name_;
            std::weak_ptr<pid_t> wkTid_;

            ThreadData(const ThreadFunc &func,
                       const std::string &name,
                       const std::shared_ptr<pid_t> &tid)
                    : func_(func),
                      name_(name),
                      wkTid_(tid)
            {}


        };
    }

    AtomicInt32 Thread::numCreated_;
    Thread::Thread(const Threadfunc &, const std::string &name)
    {
        
    }
}