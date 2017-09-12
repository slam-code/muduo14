
#ifndef TMUDUO_SINGLETON_H
#define TMUDUO_SINGLETON_H

#include <muduo/base/noncopyable.h>
#include <mutex>
#include <assert.h>
#include <stdlib.h>

namespace muduo
{
    namespace detail
    {
        template<typename T>
        struct has_no_destory
        {
            template<typename C>
            static char test(decltype(&C::no_destory()));

            template<typename C>
            static int64_t test(...);

            const static bool value = sizeof(test<T>(0)) == 1;
        };
        std::mutex mu;
    }


    template<typename T>
    std::once_flag flag;


    template<typename T>
    class Signleton : noncopyable
    {
    public:
        static T &instance()
        {
           // std::call_once(flag, &Signleton::init);

             if(value_== nullptr)
                 init();
            assert(value_ != nullptr);
            return *value_;
        }


    private:
        //static std::once_flag flag;
        static T *value_;
        //static initializations are thread safe now in C++11
        //C++11 made it so static initialization is thread-safe, so a lot of the more complicated recipes aren't needed.

        static void init()
        {

            std::lock_guard< std::mutex> lock(detail::mu);
            if (value_ != nullptr)
                return;
            value_ = new T();
            if (!detail::has_no_destory<T>::value)
                ::atexit(destroy);
        }

        static void destroy()
        {
            typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
            T_must_be_complete_type dummy;
            (void) dummy;
            delete value_;
            value_ = nullptr;
        }

    private:
        Signleton();

        ~Signleton();
    };

    template<typename T>
    T *Signleton<T>::value_ = nullptr;


};


#endif //TMUDUO_SINGLETON_H
