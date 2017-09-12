

#include "Sockets.h"

#include <Logging.h>
#include <InetAddress.h>
#include <SocketsOps.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;

Socket::~Socket()
{
    sockets::close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info *tcpi) const
{
    socklen_t len = sizeof(*tcpi);
    bzero(tcpi, len);
    return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char *buf, int len) const
{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
                         "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                         "lost=%u retrans=%u rtt=%u rttvar=%u "
                         "sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpi.tcpi_retransmits,

                 tcpi.tcpi_rto,
                 tcpi.tcpi_ato,
                 tcpi.tcpi_snd_mss,
                 tcpi.tcpi_rcv_mss,

                 tcpi.tcpi_lost,
                 tcpi.tcpi_retrans,
                 tcpi.tcpi_rtt,
                 tcpi.tcpi_rttvar,

                 tcpi.tcpi_snd_ssthresh,
                 tcpi.tcpi_snd_cwnd,
                 tcpi.tcpi_total_retrans
        );
    }
    return ok;
}

void Socket::bindAddress(const InetAddress &localaddr)
{
    sockets::bindOrDie(sockfd_, localaddr.getSockAddr());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress *peeraddr)
{
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}


void Socket::shutdownWrite()
{
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, static_cast<socklen_t >(sizeof(optval)));
}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t > (sizeof(optval)));
}

void Socket::setReusePort(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t >(sizeof(optval)));
    if (ret < 0 && on)
        LOG_SYSFATAL << "SO_REUSEPORT failed";
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t >(sizeof(optval)));
}


