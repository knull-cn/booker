
#include "kbase/klogger.h"

#include "ksocket/kfdset.h"
#include "ksocket/ksocket.h"
#include "ksocket/kepoll.h"

#include "kaddress.h"

#include <string>

using namespace KBASE;

//模仿golang中的tcp;

int32_t Tcpv4Listen(KIPv4Addr &addr)
{
    int32_t fd = KSOCKET::SocketIPv4();
    if (fd < 0)
    {
        LOGICERRNO("SocketIPv4 ");
        return fd;
    }
    struct sockaddr_in addrin = addr.Sockaddr_in();
    if (!KSOCKET::BindIPv4(fd,addrin))
    {
        LOGICERRNO("BindIPv4 ");
        KSOCKET::Release(fd);
        fd = -1;
    }
    if (!KSOCKET::ListenIPv4(fd))
    {        
        LOGICERRNO("Tcpv4Listen(%s:%d) ",addr.Address(),addr.Port());
        KSOCKET::Release(fd);
        fd = -1;
    }
    return fd;
}

bool SocketSet(int32_t fd)
{
    if (!KSOCKET::SetClosefdOnFork(fd))
    {
        LOGICERRNO("SetClosefdOnFork");
        KSOCKET::Release(fd);
        fd = -1;
        return false;
    }
    if (!KSOCKET::SetNoblock(fd))
    {
        LOGICERRNO("SetNoblock ");
        KSOCKET::Release(fd);
        fd = -1;
        return false;
    }
    return true;
}

int32_t g_efd = -1;

void StartServer(int32_t port)
{
    g_efd = KSOCKET::Create();
    //
    KIPv4Addr addr(port);
    int32_t lfd = Tcpv4Listen(addr);
    if (lfd < 0)
    {
        return ;
    }
    if (!SocketSet(lfd))
    {
        return ;
    }
    LOGDEBUG("Server Listen on %d",port);
    //
    KSOCKET::Add(g_efd,lfd,NULL);
    //
    KSOCKET::KEvent evbuf[128];
    char buf[1024];
    int32_t n = 0;
    while (true)
    {
        n = KSOCKET::Wait(g_efd,evbuf,128);
        if (n < 0)
        {
        	LOGERROR("epoll_wait error(%d):%s\n",errno,strerror(errno));
        	return ;
        }
        for (int i=0; i<n; i++)
        {
            KSOCKET::KEvent &ev = evbuf[i];
            if (ev.sock == lfd)
            {
                if (ev.read)
                {
                    struct sockaddr_in daddr;
                    int32_t fd = KSOCKET::AccpetIPv4(lfd,daddr);
                    if (fd < 0)
                    {
                        LOGERROR("AccpetIPv4 ERROR");
                        continue;
                    }
                    KIPv4Addr accepter(daddr);
                    LOGDEBUG("accept fd(%d) from (%s:%d)",fd,accepter.Address(),accepter.Port());
                    SocketSet(fd);
                    KSOCKET::Add(g_efd,fd,NULL);
                    continue;
                }
                if (ev.error)
                {
                    LOGICERRNO("On Epoll wait ");
                    return ;
                }
            }
            else
            {
        		if (ev.read)
        		{
            		int n = read(ev.sock,buf,1024);
            		if (n > 0 )
            		{
                		write(ev.sock,buf,n);
                		LOGDEBUG("fd(%d) read/write(%d):%s\n",ev.sock,n,buf);
            		}
            		else
            		{//n==0;closed;
            			KSOCKET::Del(g_efd,ev.sock);
            			KSOCKET::Release(ev.sock);
            			//on close: read&error;
            			LOGICERRNO("fd(%d) close(%d)\n",ev.sock,ev.error);
            		}
        		}
        		else if (ev.error)
        		{
                    KSOCKET::Del(g_efd,ev.sock);
                    KSOCKET::Release(ev.sock);
        			LOGICERRNO("fd(%d) error close\n",ev.sock);
        		}
            }            
        }
         
    }
}

int main(int argc,char *argv[])
{
    int32_t port = 9876;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    StartServer(port);
    return 0;
}
