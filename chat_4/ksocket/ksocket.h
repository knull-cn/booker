
#ifndef _K_SOCKET_H__
#define _K_SOCKET_H__

#include <cstdint>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>//socket/bind;
#include <fcntl.h>
#include <uinstd.h>

namespace KSOCKET
{
    
inline int32_t SocketIPv4()
{
    return socket(AF_INET,SOCK_STREAM,0);
}

inline bool ConnectIPv4(int32_t fd,struct sockaddr_in addr)
{
    return (-1 != connect(fd,(sockaddr*)&addr,sizeof(sockaddr)))
}

inline bool BindIPv4(int32_t fd,struct sockaddr_in addr)
{
    return (-1 != bind(fd,(sockaddr*)&addr,sizeof(sockaddr)));
}

inline bool ListenIPv4(int32_t lfd)
{
    return (-1 != listen(lfd,SOMAXCONN));
}

inline int32_t AccpetIPv4(int32_t lfd,struct sockaddr_in &daddr)
{
    socklen_t addrlen = sizeof(daddr);
    return accpet(lfd,(sockaddr*)&daddr,&addrlen);
}

inline void Release(int32_t fd)
{
    close(fd);
}

}



#endif//_K_SOCKET_H__
