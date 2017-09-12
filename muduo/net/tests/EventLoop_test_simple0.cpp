
#include <muduo/net/EventLoop.h>
#include <thread>
#include <assert.h>

using namespace muduo;
using namespace muduo::net;

EventLoop* g_loop;

void callback()
{
    printf("callback():pid=%d ,tid= %ld\n",getpid(),::syscall(SYS_gettid));
    EventLoop anotherLoop;//程序会中断
};

void threadFunc()
{
    printf("threadfunc():pid=%d ,tid= %ld\n",getpid(),::syscall(SYS_gettid));
    assert(EventLoop::getEventLoopOfCurrentThread()==NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread()==&loop);
    loop.runAfter(3.0,callback);
    loop.loop();
}

int main()
{
    printf("main():pid=%d ,tid= %ld\n",getpid(),::syscall(SYS_gettid));
    assert(EventLoop::getEventLoopOfCurrentThread()==NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread()==&loop);
    std::thread t(threadFunc);

    loop.loop();
    t.join();
}