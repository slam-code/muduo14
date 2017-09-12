#include <ChronoTimestamp.h>
#include <vector>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <cstdlib>

using namespace muduo;
using namespace std;
using namespace std;
std::mutex  g_mutex;
vector<int> g_vec;
const int kCount = 10 * 1000 * 1000;//10M

void threadFunc() {
    for (int i = 0; i < kCount; ++i) {
        lock_guard< mutex> lock(g_mutex);
        g_vec.push_back(i);
    }
}

int foo() __attribute__ ((noinline));///fun函数不能作为inline函数优化

int g_count = 0;

int foo() {
     lock_guard< mutex> lock(g_mutex);


    ++g_count;
    return 0;
}

int main() {
    foo();
    if (g_count != 1) {
        printf("MCHECK calls twice.\n");
        abort();
    }

    const int kMaxThreads = 8;
    g_vec.reserve(kMaxThreads * kCount);

    ChronoTimestamp start(ChronoTimestamp::now());
    for (int i = 0; i < kCount; ++i) {
        g_vec.push_back(i);
    }

    printf("single thread without lock %f\n", timeDifference(ChronoTimestamp::now(), start));

    start = ChronoTimestamp::now();
    printf(" now -start= %f\n", timeDifference(ChronoTimestamp::now(), start));

    threadFunc();
    printf("single thread with lock %f\n", timeDifference(ChronoTimestamp::now(), start));

    for (int nthreads = 1; nthreads < kMaxThreads; ++nthreads) {
        std::vector<std::unique_ptr<thread>> threads;
        g_vec.clear();
        start = ChronoTimestamp::now();
        for (int i = 0; i < nthreads; ++i) {
            threads.emplace_back(new thread(&threadFunc));
            //threads.back()->start();
        }
        for (int i = 0; i < nthreads; ++i) {
            threads[i]->join();
        }
        printf("%d thread(s) with lock %f\n", nthreads, timeDifference(ChronoTimestamp::now(), start));
    }
}

