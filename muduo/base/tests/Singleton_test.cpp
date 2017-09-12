

#include <iostream>
#include <thread>
#include <muduo/base/Singleton.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <mutex>

class Test : muduo::noncopyable
{
public:
    Test()
    {
        printf("tid=%zu, Test() %p \n",   std::hash<std::thread::id>()(std::this_thread::get_id()), this);
    }

    ~Test()
    {
        printf("tid=%zu,~Test()%p %s \n",  std::hash<std::thread::id>()(std::this_thread::get_id()), this, name_.c_str());
    }

    const std::string &name()const {return name_;}

    void setname(const std::string& n){name_=n;}

private:
    std::string name_;
};

class TestNoDestroy:muduo::noncopyable
{
public:
    void no_destroy();

    TestNoDestroy()
    {
        printf("tid=%zu ,constructing TestNoDestroy() %p \n", std::hash<std::thread::id>()(std::this_thread::get_id()),this);
    }

    ~TestNoDestroy()
    {
        printf("tid=%zu, destructing ~TestNoDestroy() %p \n", std::hash<std::thread::id>()(std::this_thread::get_id()),this);
    }

};

void threadFunc(int i)
{
    std::mutex mu;
    std::lock_guard< std::mutex> lock(mu);
    printf("tid=%zu ,%p ,name=%s\n",
           std::hash<std::thread::id>()(std::this_thread::get_id()),
           &muduo::Signleton<Test>::instance(),
           muduo::Signleton<Test>::instance().name().c_str());

    std::this_thread::yield();
    std::string s=std::to_string(i);
    muduo::Signleton<Test>::instance().setname(s);

}
int main()
{
    muduo::Signleton<Test>::instance().setname("only one -1");

    printf("tid=%zu ,%p name=%s \n",
           std::hash<std::thread::id>()(std::this_thread::get_id()),
           &muduo::Signleton<Test>::instance(),
           muduo::Signleton<Test>::instance().name().c_str());

    std::vector<std::unique_ptr<std::thread>> vthreads;
    for (int i = 0; i < 100; ++i)
    {
        vthreads.emplace_back(new std::thread(threadFunc,i));
    }

    for(auto & t:vthreads)
        t->join();

    printf("\n\n");


    printf("tid=%zu ,%p name=%s ----\n",
           std::hash<std::thread::id>()(std::this_thread::get_id()),
           &muduo::Signleton<Test>::instance(),
           muduo::Signleton<Test>::instance().name().c_str());



    printf("with valgrind: %zd-byte memory leak .\n",sizeof(TestNoDestroy));
    muduo::Signleton<TestNoDestroy>::instance();


}