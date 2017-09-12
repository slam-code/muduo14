

#include <muduo/log/BoundedBlockingQueue.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <thread>
#include <muduo/base/Atomic.h>
#include <unistd.h>
#include <syscall.h>

class  Test
{
public:

    Test(int numThreads,int maxsize):queue_(maxsize)//,vthreads_(numThreads)
    {
        for (int i = 0; i < numThreads; ++i)
        {
            char name[32];
            snprintf(name ,sizeof name,"work thread %d ",i);
            vthreads_.emplace_back(new std::thread(std::bind(&Test::threadFunc,this,i)));
        }
        done=true;
    }

    void run(int times)
    {
        std::unique_lock<std::mutex> g(mutex_);
        cv.wait(g,[&]{return done== true;});
        printf("all threads started\n");

        for (int i = 0; i < times; ++i)
        {
            char buf[32];
            snprintf(buf,sizeof buf,"hello %d",i);
            queue_.put(buf);
            printf("tid=%ld,put data=%s,size=%zd\n",::syscall(SYS_gettid),buf,queue_.size());
        }
    }

    void joinAll()
    {
        for (int i = 0; i < vthreads_.size(); ++i)
        {
            queue_.put("stop");
        }
       /* for (int j = 0; j < vthreads_.size(); ++j)
        {
            vthreads_[j]->join();
        }*/
        for(auto &it :vthreads_)
            it->join();
    }
private:

    void threadFunc(int i)
    {
        printf("tid=%ld,%d,started\n",::syscall(SYS_gettid),i);
        bool running = true;

        while (running)
        {
            std::string d(queue_.take());
            printf("tid=%ld,get data=%s ,size=%zd\n",::syscall(SYS_gettid),d.c_str(),queue_.size());
            running=(d!="stop");
        }
        printf("tid=%ld,%d,stopped\n",::syscall(SYS_gettid),i);
    }

    muduo::Atomic_bool done{false};
    std::mutex mutex_;
    muduo::BoundedBlockingQueue<std::string>queue_;
    std::condition_variable cv;
    std::vector<std::unique_ptr<std::thread>> vthreads_;
};

int main()
{
    printf("pid=%d,tid=%ld\n",::getpid(),::syscall(SYS_gettid));
    Test t(5,10);
    t.run(100000);
    t.joinAll();

}