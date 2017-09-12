
#ifndef TMUDUO_ATOMIC_H
#define TMUDUO_ATOMIC_H

#include <atomic>
#include <stdint.h>

/*************************************************
c++11标准库<atomic>提供了多种原子类操作。
使用std的原子整数操作和bool型操作。
*************************************************/
namespace muduo
{

    using AtomicInt32= std::atomic_int_fast32_t;
    using AtomicInt64= std::atomic_int_fast64_t;
    using Atomic_bool= std::atomic_bool;


}

#endif  // TMUDUO_ATOMIC_H
