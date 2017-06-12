#ifndef _K_FD_SET_H__
#define _K_FD_SET_H__

#include <cstdint>

#include <sys/types.h>
#include <netinet/tcp.h>//TCP_NODELAY
#include <netinet/in.h>//IPPROTO_TCP
#include <sys/socket.h>//socket/bind;
#include <fcntl.h>
#include <cstdlib>

namespace KSOCKET
{

inline bool setfd(int32_t sockfd,int32_t arg)
{
    int32_t flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= arg;
    int32_t ret = ::fcntl(sockfd, F_SETFL, flags);
    return  (ret != -1 );
}

inline bool SetNoblock(int32_t sockfd)
{
    return  (setfd(sockfd,O_NONBLOCK) != -1 );
}

inline bool SetClosefdOnFork(int32_t sockfd)
{
    return  (setfd(sockfd,FD_CLOEXEC) != -1 );
}

//
inline bool setTcp(int32_t sockfd,int32_t op)
{
    int32_t on = 1;
    return (-1 != ::setsockopt(sockfd, IPPROTO_TCP, op, (const char *)&on, sizeof(on)));
}

inline bool SetNodelay(int32_t sockfd)
{
    return setTcp(sockfd,TCP_NODELAY);
}

inline bool KeepAlive(int32_t sockfd)
{
    return setTcp(sockfd,SO_KEEPALIVE);
}

}



#endif//_K_FD_SET_H__
