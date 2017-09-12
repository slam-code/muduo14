#include <muduo/base/Atomic.h>
#include <assert.h>
#include <iostream>
#include <thread>
#include <vector>

int knum = 1000000;
int kthread = 100;


void addnum(muduo::AtomicInt64 &a, int &b) {//noncopyable
    for (int i = 0; i < knum; ++i) {
        ++a;
        std::this_thread::yield();
        ++b;
    }
    std::cout << "thread_id " << std::this_thread::get_id() << "  done\n";
}



int main() {

    muduo::AtomicInt64 a(0);
    int b = 0;
    assert(a.load() == 0);
    assert(a.fetch_add(1) == 0);
    assert(a.load() == 1);
    a.fetch_sub(1);


    std::vector<std::thread> threads;

    for (int i = 0; i < kthread; ++i) {
        threads.push_back(std::thread(addnum, std::ref(a), std::ref(b)));
    }

    for (auto &it :threads)
        it.join();

    assert(a == knum * kthread);
    assert(b != knum * kthread);
    std::cout << __FILE__ << " , " << __LINE__ << (":  test ok : a=") << a << ",b=" << b << std::endl;

}

//clang++ Atomic_unittest.cc  -std=c++11 -I../../../ -pthread
