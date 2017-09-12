

#ifndef TMUDUO_INETADDRESS_H
#define TMUDUO_INETADDRESS_H

#include <muduo/base/copyable.h>
#include <muduo/base/StringPiece.h>
#include <netinet/in.h>

namespace muduo
{
    namespace net
    {
        namespace sockets
        {
            const struct sockaddr *sockaddr_cast(const struct sockaddr_in6 *addr);
        }

        class InetAddress : public muduo::copyable
        {
        public:

            explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

            InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);

            explicit InetAddress(const struct sockaddr_in &addr)
                    : addr_(addr)
            {
            }

            explicit InetAddress(const struct sockaddr_in6& addr)
                    : addr6_(addr)
            {}

            sa_family_t family() const
            {
                return addr_.sin_family;
            }

            std::string toIp() const;

            std::string toIpPort() const;

            uint16_t toPort() const;

            const struct sockaddr *getSockAddr() const
            {
                return sockets::sockaddr_cast(&addr6_);
            }

            void setSockAddrInet6(const struct sockaddr_in6 &addr6)

            {
                addr6_=addr6;
            }

            uint32_t ipNetEndian() const;

            uint16_t portNetEndian() const
            {
                return addr_.sin_port;
            }

            static bool resolve(StringArg hostname, InetAddress *result);


        private:
            union
            {
                struct sockaddr_in addr_;
                struct sockaddr_in6 addr6_;
            };
        };


    }


}
#endif //TMUDUO_INETADDRESS_H
