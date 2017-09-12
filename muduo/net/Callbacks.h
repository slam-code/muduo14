

#ifndef TMUDUO_CALLBACKS_H
#define TMUDUO_CALLBACKS_H

#include <muduo/base/ChronoTimestamp.h>
#include <functional>
#include <memory>
#include <muduo/base/Types.h>

namespace muduo
{
    template<typename T>
    inline T *get_pointer(const std::shared_ptr<T> &ptr)
    {
        return ptr.get();
    }

    template<typename T>
    inline T *get_pointer(const std::unique_ptr<T> &ptr)
    {
        return ptr.get();
    }


    template<typename To, typename From>
    inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From> &f)
    {
        if (false)
            muduo::implicit_cast<From *, To *>(0);
#ifndef NDEBUG
        assert(f == nullptr || dynamic_cast<To *>(get_pointer(f)) != nullptr);
#endif
        return ::std::static_pointer_cast<To>(f);
    };


    namespace net
    {
        class Buffer;

        class TcpConnection;

        typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
        typedef std::function<void()> TimerCallback;
        typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
        typedef std::function<void(TcpConnectionPtr &)> CloseCallback;

        typedef std::function<void(TcpConnectionPtr &)> WriteCompleteCallback;
        typedef std::function<void(TcpConnectionPtr &, size_t)> HighWaterMarkCallback;


        void defaultConnectionCallback(const TcpConnectionPtr &,
                                       Buffer *, ChronoTimestamp);

        void defaultMessageCallback(const TcpConnectionPtr &,
                                    Buffer *, ChronoTimestamp receiveTime);

    }
}
#endif //TMUDUO_CALLBACKS_H
