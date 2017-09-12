
#include <BlockingQueue.h>
#include <thread>
#include <condition_variable>
#include <string>
#include <vector>
#include <iostream>
#include <zconf.h>
#include <Atomic.h>
#include <muduoFunction.h>
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
        cout << "all thread start \n";
       // sleep(1);
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
            //cout<<"tid=%d "<<this_thread::get_id().id()<<"put data ="<<buf<<" ,size="<<queue_.size()<<endl;
            printf("linux_tid=%d ,tid=%d ,put data=%s ,size=%zd\n", static_cast<pid_t >(::syscall(SYS_gettid)),
                   getcpptid, buf, queue_.size());
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
        //cout<<" tid= "<<this_thread::get_id()<<endl;
        printf("linux_tid=%d ,cpptid= %d started\n", static_cast<pid_t >(::syscall(SYS_gettid)), getcpptid);
        bool running = true;
        while (running)
        {
            std::string d(queue_.take());
            //cout<<"tid= "<<this_thread::get_id()<<",c_str()="<<d.c_str()<<" ,size="<<queue_.size()<<endl;
            printf("linux_tid=%d ,cpptid=%d ,get data=%s ,size=%zd \n", static_cast<pid_t >(::syscall(SYS_gettid)),
                   getcpptid, d.c_str(), queue_.size());
            running = (d != "stop");
            this_thread::yield();
        }

        printf("linux_tid=%d ,tid=%d stopped\n", static_cast<pid_t >(::syscall(SYS_gettid)), getcpptid);
    }

    muduo::BlockingQueue<std::string> queue_;
    condition_variable cv;
    std::mutex mutex_;
    std::vector<std::unique_ptr<std::thread>> threads_;
    muduo::Atomic_bool done{false};

};

int main()
{
    //cout<<::getpid()<<" =pid,tid= "<<this_thread::get_id()<<endl;
    printf("linux_tid=%d,tid=%d \n", static_cast<pid_t >(::syscall(SYS_gettid)), getcpptid);
    printf("pid=%d,tid=%d\n\n", ::getpid(), getcpptid);

    Test t(5);
    t.run(100);
    //sleep(1);
    t.joinAll();


    return 0;
}
