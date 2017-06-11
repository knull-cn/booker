
#ifndef _K_ADDRESS_H__
#define _K_ADDRESS_H__

#include <cstdint>
#include <cstring>

#include <arpa/inet.h>

struct KIPv4Addr
{
    uint8_t ipv4_[16];
    uint16_t port_;

    //提供给上层的地址类型；对于上层来说，一般都是用string作为地址;
    KIPv4Addr(const char *ipv4,int32_t port):port_(port),
    {
        strncpy(IPv4,ipv4,16);
    }
    KIPv4Addr(int32_t port):port_(port),
    {
        strncpy(IPv4,"0.0.0.0",16);
    }
    //从socket底层上过来的地址，这里直接转换过来;
    KIPv4Addr(struct sockaddr_in &addr)
    {
        inet_ntop(AF_NET,(void*)&addr,ipv4_,sizeof(addr));
    }
    //这个仅仅是底层socket相关的用到,上层不用关心;
    struct sockaddr_in Sockaddr_in() 
    {
        struct sockaddr_in addr;
        inet_pton(AF_NET,ipv4_,(void*)&addr);
        return addr;
    } 
    const char *Address() const 
    {
        return ipv4_;
    }
    uint32_t Port() const 
    {
        return port_;
    }
};


#endif //_K_ADDRESS_H__
