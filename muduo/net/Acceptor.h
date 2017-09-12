

#ifndef TMUDUO_ACCEPTOR_H
#define TMUDUO_ACCEPTOR_H

#include <functional>
#include <muduo/net/Channel.h>
#include <muduo/net/Sockets.h>

namespace muduo
{
    namespace net
    {
        class EventLoop;

        class InetAddress;

        class Acceptor : noncopyable
        {

        public:
            using NewConnectionCallback =std::function<void(int sockfd, const InetAddress &)>;

            Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);

            ~Acceptor();


            void setNewConnectionCallback(const NewConnectionCallback &cb)
            {
                newConnectionCallback_=cb;
            }

            bool listenning() const
            {
                return listenning_;
            }

            void listen();

        private:
            void handleRead();

            EventLoop *loop_;
            Socket acceptSocket_;
            Channel acceptChannel_;
            NewConnectionCallback newConnectionCallback_;
            bool listenning_;
            int idleFd_;

        };
    }
}
#endif //TMUDUO_ACCEPTOR_H
