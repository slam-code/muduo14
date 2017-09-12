

#include <iostream>
#include <muduo/base/ChronoTimestamp.h>

using namespace muduo;
using namespace std;

const int kNumber = 1000 * 1000 * 100;//*5

void benchmark()
{
    for (int i = 1; i < kNumber; ++i)
    {
        ChronoTimestamp t;
        t = t.now();
        t.toFormattedString();
    }

}

int main()
{
    ChronoTimestamp t;
    t = t.now();
    cout << t.toFormattedString() << endl;

    benchmark();
    ChronoTimestamp t2 = t.now();
    cout << t2.toFormattedString() << endl;
    cout << kNumber<<" total use " << timeDifference(t2, t) << " s \n";
    return 0;
}

/*
kNumber=5M

20170527 15:22:27.477461
20170527 15:22:29.719607
5000000 total use 2.24215 s
*/

/*
kNumber=100M

ChronoTimestamp
20170526 14:56:30.711533
20170526 14:57:14.411524
100000000 total use 43.7 s


Timestamp
20170526 14:55:27.177002
20170526 14:56:10.686032
100000000 total use 43.509 s
*/