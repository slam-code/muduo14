
#include <muduo/timer/Timer.h>
#include <iostream>
#include <muduo/base/ChronoTimestamp.h>

using namespace std;
using  namespace muduo::net;
using  namespace muduo;
void f1()
{
    cout << "f()...\n";
}

void f2()
{
    cout << "f2()...\n";
}

int main()
{

    Timer t1(f1,ChronoTimestamp::now(), 3);
    Timer t2(f2, ChronoTimestamp::now(), 4);

    t1.run();
    t2.run();
    cout<<t1.numCreated()<<endl;
    cout<<t1.expiration().toFormattedString()<<endl;
    cout<<t1.sequence()<<endl;

}