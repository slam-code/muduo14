
#include "muduo/log/BlockingQueue.h"
#include <thread>
#include <condition_variable>
#include <string>
#include <vector>
#include <iostream>
#include <zconf.h>
#include <syscall.h>
#include <muduo/base/ChronoTimestamp.h>
#include <map>

using namespace std;

class Bench
{
public:
    Bench(int numThreads)
    {
        for (int i = 0; i < numThreads; ++i)
        {
            char name[32];
            snprintf(name,sizeof(name),"work thread %d ",i);
            vthreads_.emplace_back(new std::thread(std::bind(&Bench::threadFunc,this,i)));
        }
    };

    void run(int times)
    {
        printf("all threads started \n");
        for (int i = 0; i < times; ++i)
        {
            muduo::ChronoTimestamp now(muduo::ChronoTimestamp::now());
            queue_.put(now);
            usleep(1000);
        }
    }

    void  joinAll()
    {
        for(size_t i=0;i<vthreads_.size() ;++i)
        {
            queue_.put(muduo::ChronoTimestamp());
        }
        for(auto &it:vthreads_)
            it->join();
    }
private:

    void threadFunc(int i)
    {
        printf("tid=%ld,started %d\n",(::syscall(SYS_gettid)),i);
        std::map<int,int>delays;
        bool  running=true;
        while (running)
        {
            muduo::ChronoTimestamp t(queue_.take());
            muduo::ChronoTimestamp now(muduo::ChronoTimestamp::now());
            if(muduo::ChronoTimestamp()<t)
            {
                int delay= static_cast<int>(muduo::timeDifference(now,t)*1000000);
                ++delays[delay];
            }
            running=(muduo::ChronoTimestamp()<t);
        }


        for(auto it=delays.begin();it!=delays.end();++it)
        {
            printf("tid=%ld,delay=%d ,count=%d\n",::syscall(SYS_gettid),it->first,it->second);
            //this_thread::yield();
        }

        printf("tid=%ld,stopped\n",::syscall(SYS_gettid));
    }
    muduo::BlockingQueue<muduo::ChronoTimestamp> queue_;
    std::condition_variable cv;
    std::vector<std::unique_ptr<std::thread>> vthreads_;
};

int main()
{
    printf("linux_tid=%d, main pid=%d \n", static_cast<pid_t >(::syscall(SYS_gettid)), ::getpid());
    Bench t(5);
    t.run(10000);//主线程put 1M data.无界队列.
    t.joinAll();

    return 0;
}
