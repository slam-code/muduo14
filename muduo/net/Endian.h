

#ifndef TMUDUO_ENDIAN_H
#define TMUDUO_ENDIAN_H

#include <stdint.h>
#include <endian.h>

//网络字节序是大端．
namespace muduo
{
    namespace net
    {
        namespace sockets
        {// be是表示大端，le是表示小端。
            inline uint64_t hostToNetWork64(uint64_t hosts64)
            {
                return htobe64(hosts64);
            }

            inline uint32_t hostToNetWork32(uint32_t hosts32)
            {
                return htobe32(hosts32);
            }

            inline uint16_t hostToNetWork16(uint16_t host16)
            {
                return htobe16(host16);
            }
            inline uint64_t networkToHost64(uint64_t net64)
            {
                return be64toh(net64);
            }
            inline uint32_t networkToHost32(uint32_t net32)
            {
                return be32toh(net32);
            }

            inline uint16_t networkToHost16(uint16_t net16)
            {
                return be16toh(net16);
            }
        }
    }
}
#endif //TMUDUO_ENDIAN_H
