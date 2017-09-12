
#ifndef TMUDUO_CHANNEL_H
#define TMUDUO_CHANNEL_H

#include <functional>
#include <memory>
#include <muduo/base/noncopyable.h>
#include <muduo/base/ChronoTimestamp.h>

namespace muduo
{
    namespace net
    {
        class EventLoop;

/*************************************************
Channel是selectable IO channel,负责注册与响应IO事件,但不拥有文件描述符fd
一个Channel只属于一个Eventloop,Channel处理文件描述符的IO事件.
本质是对文件描述符IO处理事件的抽象
*************************************************/
        class Channel : noncopyable
        {
        public:
            using EventCallback= std::function<void()>;
            using ReadEventCallback= std::function<void(ChronoTimestamp)>;

            Channel(EventLoop *loop, int fd);/*构造函数传递一个Eventloop对象,一个文件描述符*/

            ~Channel();

            void handleEvent(ChronoTimestamp receiveTime);

            void setReadCallback(const ReadEventCallback &cb)
            {
                readCallback_ = cb;
            }

            void setWriteCallback(const EventCallback &cb)
            {
                writeCallback_ = cb;
            }

            void setCloseCallback(const EventCallback &cb)
            {
                closeCallback_ = cb;
            }

            void setErrorCallback(const EventCallback &cb)
            {
                errorCallback_ = cb;
            }

            //to do: &&

            void tie(const std::shared_ptr<void> &);

            int fd() const
            {
                return fd_;
            }

            int events() const
            {
                return events_;
            }

            void set_revents(int revt)
            {
                revents_ = revt;
            }

            bool isNoneEvent() const
            {
                return events_ == kNoneEvent;
            }

            void enableReading()
            {
                events_ |= kReadEvent;
                update();
            }

            void disableReading()
            {
                events_ &= ~kReadEvent;
                update();
            }

            void enableWriting()
            {
                events_ |= kWriteEvent;
                update();
            }

            void disableWriting()
            {
                events_ &= ~kWriteEvent;
                update();
            }

            void disableAll()
            {
                events_ = kNoneEvent;
                update();
            }

            bool isWriting() const
            {
                return events_ & kWriteEvent;
            }


            bool isReading() const
            {
                return events_ & kReadEvent;
            }

            int index()
            {
                return index_;
            }

            void set_index(int idx)
            {
                index_ = idx;
            }


            std::string reventsToString() const;

            std::string eventsToString() const;

            void doNotLogHup()
            {
                logHup_ = false;
            };

            /*Channel 对象所属的loop线程,一个Channel只能属于一个Eventloop*/
            EventLoop *ownerLoop()
            {
                return loop_;
            }

            void remove();

        private:
            static std::string eventsToString(int fd, int ev);

            void update();      /*调用update()导致Eventloop调用updateChannel,注册到loop中*/

            void handleEventWithGuard(ChronoTimestamp receiveTime);

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventLoop *loop_;
            const int fd_;

            int events_;    /*关注的事件*/
            int revents_;   /*poll/epoll返回的事件*/
            int index_;     /*在poll的事件数组中的序号*/

            bool logHup_;

            std::weak_ptr<void> tie_;

            bool tied_;

            bool eventHandling_;

            bool addedToLoop_;

            ReadEventCallback readCallback_;
            EventCallback writeCallback_;
            EventCallback closeCallback_;
            EventCallback errorCallback_;


        };


    }
}

#endif //TMUDUO_CHANNEL_H
