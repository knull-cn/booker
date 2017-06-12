#include <ctime>

#include "kbase/klogger.h"

#include "ksocket/kfdset.h"
#include "ksocket/ksocket.h"
#include "ksocket/kepoll.h"

#include "kaddress.h"

#include <string>

using namespace KBASE;

int32_t g_efd = -1;

uint32_t idx = 0;
time_t tx = time(0)%10000;

bool ConnectServer(KIPv4Addr &addr)
{
    int32_t fd = KSOCKET::SocketIPv4();
    if (fd < 0)
    {
        LOGICERRNO("SocketIPv4 ");
        return false;
    }
    struct sockaddr_in addrin = addr.Sockaddr_in();
    if (!KSOCKET::ConnectIPv4(fd,addrin))
    {
        LOGICERRNO("ConnectIPv4(%s:%d) ",addr.Address(),addr.Port());
        return false;
    }
    if (!KSOCKET::SetClosefdOnFork(fd))
    {
        LOGICERRNO("SetClosefdOnFork ");
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
    //触发读写;
    char buf[128];
    int n = sprintf(buf,"time(%d)_idx(-1)",tx);
    write(fd,buf,n);
    //add;
    if (!KSOCKET::Add(g_efd,fd,NULL))
    {
        LOGICERRNO("Add ");
        KSOCKET::Release(fd);
        fd = -1;
        return false;
    }
    return true;
}

void StartClient(int32_t port )
{
	g_efd = KSOCKET::Create();
	KIPv4Addr addr("127.0.0.1",port);
	for (int32_t i=0; i<10;i++)
	{
		ConnectServer(addr);
	}
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
    		if (ev.read)
    		{
        		int n = read(ev.sock,buf,1024);
        		if (n > 0 )
        		{
        		    int nw = sprintf(buf,"time(%d)_idx(%d)",tx,idx++);
            		write(ev.sock,buf,n);
            		LOGDEBUG("fd(%d) read/write(%d):%s\n",ev.sock,n,buf);
            		sleep(1);
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

int main(int argc,char *argv[])
{
    int32_t port = 9876;
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    StartClient(port);
    return 0;
}
