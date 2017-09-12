
#include <muduo/log/BlockingQueue.h>
#include <thread>
#include <condition_variable>
#include <string>
#include <vector>
#include <iostream>
#include <zconf.h>
#include <muduo/base/Atomic.h>
#include <syscall.h>

using namespace std;

class Test
{
public:
    Test(int numthreads)
    {
        for (int i = 0; i < numthreads; ++i)
        {
            char name[32];
            snprintf(name, sizeof name, "work thread %d ", i);
            threads_.emplace_back(new std::thread(std::bind(&Test::threadFunc, this)));
            printf("%s\n", name);
        }
        done = true;
        cv.notify_one();//显式唤醒等待的条件变量.
        cout << "all thread start \n";
    }

    void run(int times)
    {
        printf("run ");
        {

            std::unique_lock<std::mutex> lk(mutex_);
            while (done != true)
                cv.wait(lk, [&]
                { return done == true; });
        }
        for (int i = 0; i < times; ++i)
        {
            char buf[32];
            snprintf(buf, sizeof buf, "hello %d", i);
            queue_.put(buf);
            printf("linux_tid=%d ,put data=%s ,size=%zd\n", static_cast<pid_t >(::syscall(SYS_gettid)),
                   buf, queue_.size());
            this_thread::yield();
        }

    }

    void joinAll()
    {
        for (int i = 0; i < threads_.size(); ++i)
        {
            queue_.put("stop");
        }
        for (auto &thr:threads_)
            thr->join();
    }

private:

    void threadFunc()
    {
        printf("linux_tid=%d ,started\n", static_cast<pid_t >(::syscall(SYS_gettid)));
        bool running = true;
        while (running)
        {
            std::string d(queue_.take());
            printf("linux_tid=%d ,get data=%s ,size=%zd \n", static_cast<pid_t >(::syscall(SYS_gettid)),
                   d.c_str(), queue_.size());
            running = (d != "stop");
            this_thread::yield();
        }

        printf("linux_tid=%d , stopped\n", static_cast<pid_t >(::syscall(SYS_gettid)));
    }

    muduo::BlockingQueue<std::string> queue_;
    condition_variable cv;
    std::mutex mutex_;
    std::vector<std::unique_ptr<std::thread>> threads_;
    muduo::Atomic_bool done{false};

};

void testMove()
{
    muduo::BlockingQueue<std::unique_ptr<int>>queue;
    queue.put(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x=queue.take();
    printf("took %d \n",*x);

    *x=123;
    queue.put(std::move(x));
    std::unique_ptr<int>y=queue.take();
    printf("took %d \n",*y);
}

int main()
{
    printf("linux_tid=%d, main pid=%d \n", static_cast<pid_t >(::syscall(SYS_gettid)), ::getpid());
    Test t(5);
    t.run(1000000);//主线程put 1M data.无界队列.
    t.joinAll();

    testMove();
    return 0;
}
