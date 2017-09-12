
#include <iostream>
#include <stdio.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

void mysleep(int seconds)
{
    timespec t = {seconds, 0};
    nanosleep(&t, NULL);
}

void threadFunc()
{
    std::cout << "tid=" << std::this_thread::get_id() << endl;
}

void threadFunc2(int x)
{
    std::cout << "tid= " << std::this_thread::get_id() << " ,x=" << x << endl;
}
int i=0;
void threadFunc3()
{
    std::cout << "tid=" << std::this_thread::get_id()<<" i="<<i << endl;
    ++i;
    //mysleep(1);
}

class Foo
{
public:
    explicit Foo(double x) : x_(x)
    {}

    void memberFunc()
    {
        cout << "tid= " << this_thread::get_id() << ",x_=" << x_ << endl;
    }

    void memberFunc2(const string &text)
    {
        cout << "tid= " << this_thread::get_id() << ",x_=" << x_ << " ," << text << endl;
    }

private:
    double x_;
};


int main()
{
    cout << "pid=" << ::getpid() <<",tid="<< this_thread::get_id() << endl;
    thread t1(threadFunc);
    thread t2(bind(threadFunc2, 42));
    Foo foo(87.3);
    thread t3(bind(&Foo::memberFunc, &foo));
    thread t4(bind(&Foo::memberFunc2, ref(foo), "t4"));

    {
        thread t5(threadFunc3);
        t5.join();
    }

    mysleep(2);
    {
        thread t6(threadFunc3);
        mysleep(2);
        t6.join();
    }

   vector< unique_ptr<thread>> vthreads;


    for (int j = 0; j < 10; ++j)
    {
        vthreads.emplace_back(new thread(threadFunc3));

    }

    sleep(1);
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    for(auto & it:vthreads)
            it->join();
    cout << "done all\n";
}