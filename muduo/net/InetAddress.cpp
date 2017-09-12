
#include <muduo/net/InetAddress.h>
#include <muduo/log/Logging.h>
#include <muduo/net/Endian.h>
#include <muduo/net/SocketsOps.h>
#include <netdb.h>
#include <strings.h>
#include <netinet/in.h>
#include <stddef.h>

static const in_addr_t kInaddrAny = INADDR_ANY;

static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;

using namespace muduo;
using namespace muduo::net;

//    struct sockaddr_in
//    {
//
//        in_port_t sin_port;			/* Port number.  */
//        struct in_addr sin_addr;		/* Internet address.  */
//
//        /* Pad to size of `struct sockaddr'.  */
//        unsigned char sin_zero[sizeof (struct sockaddr) -
//                               __SOCKADDR_COMMON_SIZE -
//                               sizeof (in_port_t) -
//                               sizeof (struct in_addr)];
//    };

//    /* Ditto, for IPv6.  */
//    struct sockaddr_in6
//    {
//        in_port_t sin6_port;	/* Transport layer port # */
//        uint32_t sin6_flowinfo;	/* IPv6 flow information */
//        struct in6_addr sin6_addr;	/* IPv6 address */
//        uint32_t sin6_scope_id;	/* IPv6 scope-id */
//    };
//



static_assert(sizeof(InetAddress) == sizeof(struct sockaddr_in6),"InetAddress==sockaddr_in6");
static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    static_assert(offsetof(InetAddress, addr6_) == 0, "addr6_ offset 0");
    static_assert(offsetof(InetAddress, addr_) == 0, "addr_ offset 0");
    if (ipv6)
    {
        bzero(&addr6_, sizeof(addr6_));
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = sockets::hostToNetWork16(port);
    } else
    {
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetWork32(ip);
        addr_.sin_port = sockets::hostToNetWork16(port);
    }
}

InetAddress::InetAddress(StringArg ip, uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        bzero(&addr6_, sizeof(addr6_));
        sockets::fromIpPort(ip.c_str(), port, &addr6_);
    } else
    {
        bzero(&addr_, sizeof(addr_));
        sockets::fromIpPort(ip.c_str(), port, &addr_);
    }
}

std::string InetAddress::toIpPort() const
{
    char buf[64] = "";
    sockets::toIpPort(buf, sizeof(buf), getSockAddr());
    return buf;
}

std::string InetAddress::toIp() const
{
    char buf[64] = "";
    sockets::toIp(buf, sizeof(buf), getSockAddr());
    return buf;
}

uint32_t InetAddress::ipNetEndian() const
{
    assert(family() == AF_INET);
    return addr_.sin_addr.s_addr;
}

uint16_t InetAddress::toPort() const
{
    return sockets::networkToHost16(portNetEndian());
}

static thread_local char t_resolveBuffer[64 * 1024];

bool InetAddress::resolve(StringArg hostname, InetAddress *out)
{
    assert(out!= nullptr);
    struct hostent hent;
    struct hostent* he= nullptr;
    int herrno=0;
    bzero(&hent, sizeof(hent));
    int ret=gethostbyname_r(hostname.c_str(),&hent,t_resolveBuffer, sizeof(t_resolveBuffer),&he,&herrno);
    if(ret==0&& he!= nullptr)
    {
        assert(he->h_addrtype==AF_INET && he->h_length== sizeof(uint32_t));
        out->addr_.sin_addr=*reinterpret_cast<struct in_addr*>(he->h_addr_list);
        return true;
    }
    else
    {
        if(ret)
        {
            LOG_SYSERR<<"InetAddress::resolve";
        }
        return false;
    }
}






