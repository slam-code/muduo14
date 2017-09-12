
#include <iostream>
#include <muduo/base/ChronoTimestamp.h>
#include <vector>
#include <stdio.h>
using namespace muduo;
using namespace std;
void passByConstReference(const ChronoTimestamp& x)
{
    printf("%s\n", x.toString().c_str());
}

void passByValue(ChronoTimestamp x)
{
    printf("%s\n", x.toString().c_str());
}


void benchmark()
{
    const int kNumber = 1000*1000;

    std::vector<ChronoTimestamp> stamps;
    stamps.reserve(kNumber);
    for (int i = 0; i < kNumber; ++i)
    {
        stamps.push_back(ChronoTimestamp::now());
    }
    printf("%s\n", stamps.front().toString().c_str());
    printf("%s\n", stamps.back().toString().c_str());
    printf("%f\n", timeDifference(stamps.back(), stamps.front()));

    int increments[100] = { 0 };
    int64_t start = stamps.front().microSecondsSinceEpoch().count();
    for (int i = 1; i < kNumber; ++i)
    {
        int64_t next = stamps[i].microSecondsSinceEpoch().count();
        int64_t inc = next - start;
        start = next;
        if (inc < 0)
        {
            printf("reverse!\n");
        }
        else if (inc < 100)
        {
            ++increments[inc];
        }
        else
        {
            printf("big gap %d\n", static_cast<int>(inc));
        }
    }

    for (int i = 0; i < 100; ++i)
    {
        printf("%2d: %d\n", i, increments[i]);
    }
}
int   main()
{
    ChronoTimestamp t;
    t=t.now();
    cout<<t.toString()<<endl;
    cout<<t.toFormattedString()<<endl;
    cout<<sizeof(ChronoTimestamp)<<endl;
    cout<<sizeof(std::chrono::microseconds)<<endl<<endl;

    ChronoTimestamp now(ChronoTimestamp::now());
    printf("%s\n", now.toString().c_str());
    passByValue(now);
    passByConstReference(now);
    benchmark();
    return 0;
}