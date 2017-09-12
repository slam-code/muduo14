
#include <muduo/log/LogStream.h>
#include <stdio.h>
#include <inttypes.h>
#include <muduo/base/ChronoTimestamp.h>
#include <sstream>

using namespace muduo;
using  namespace std;
const size_t  N=1000000;
template <typename T>
void benchPrint(const char *fmt)
{
    char buf[32];
    ChronoTimestamp start(ChronoTimestamp::now());
    for (int i = 0; i < N; ++i)

        snprintf(buf,sizeof buf,fmt,(T)(i));
    ChronoTimestamp end(ChronoTimestamp::now());
    printf("benckPrintf %f \n",timeDifference(end,start));

}
template <typename T>
void benchStringStream()
{
    ChronoTimestamp start(ChronoTimestamp::now());
    std::ostringstream os;
    for (int i = 0; i < N; ++i)
    {
        os<<(T)(i);
        os.seekp(0,std::ios_base::beg);
    }

    ChronoTimestamp end(ChronoTimestamp::now());

    printf("benchStringStream %f \n",timeDifference(end,start));
}

template <typename T>
void benchLogStream()
{
    ChronoTimestamp start(ChronoTimestamp::now());
    LogStream os;
    for (int i = 0; i < N; ++i)
    {
        os<<(T)(i);
       // if(i% 999==0)
            os.resetBuffer();
    }
    ChronoTimestamp end(ChronoTimestamp::now());
    printf("benchLogStream %f \n",timeDifference(end,start));
}
int main()
{
    benchPrint<int>("%d");

    puts("int");
    benchPrint<int >("%d");
    benchStringStream<int >();
    benchLogStream<int >();

    puts("double");
    benchPrint<double >("%.12g");
    benchStringStream<double >();
    benchLogStream<double >();

    puts("int64_t");
    benchPrint<int64_t >("%ld");
    benchStringStream<int64_t >();
    benchLogStream<int64_t >();

    puts("void*");
    benchPrint<void* >("%p");
    benchStringStream<void* >();
    benchLogStream<void* >();
}
