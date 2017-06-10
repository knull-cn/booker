//server.cpp

#include <sys/types.h>
#include <sys/socket.h>//socket/bind;
#include <arpa/inet.h>//htons;
#include <unistd.h>//read/write/close;

#include <cstdio>

#include <cerrno>
#include <cstring>

extern int errno;

#include <fcntl.h>
#include <cstdlib>
void SetNoblock(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    if (ret == -1 )
    {
        perror("set noblock failed!");
        exit(0);
    }
}

#include "kepoll/kepoll.h"
int g_efd = -1;
void InitEpool()
{
    g_efd = KEPOLL::create();
    if (g_efd < 0)
    {
    	printf("epoll_create error(%d):%s\n",errno,strerror(errno));
    	exit(0);
    }
}

void SetEpollRead(int fd)
{
	SetNoblock(fd);//set no block;
    KEPOLL::add(g_efd,fd,NULL);
}

int main()
{
	InitEpool();
    printf("server running...\n");
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    //set fd attributy
    //SetNoblock(lfd);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9866);

    bind(lfd,(sockaddr*)&addr,sizeof(sockaddr));

    listen(lfd,SOMAXCONN);
    SetEpollRead(lfd);

    KEPOLL::event evbuf[128];
    int esize = 128;
    int n = 0;
    char buf[1024];
    bool running  = true;
    while (running)
    {       
        n = KEPOLL::wait(g_efd,evbuf,esize);
        if (n < 0)
        {
        	printf("epoll_wait error(%d):%s\n",errno,strerror(errno));
        	return 0;
        }
        //printf("recv events(%d)\n",n);
        for (int i=0; i<n; i++)
        {
        	KEPOLL::event ev = evbuf[i];
        	if (ev.sock == lfd)
        	{//accept;
            	if (ev.read)
            	{
            		int rwfd = accept(lfd,NULL,NULL);
            		if (rwfd < 0)
            		{

            		}
            		else
            		{
            			printf("accept fd(%d)\n",rwfd);
            			SetEpollRead(rwfd);
            		}
            	}
            	if (ev.error)
            	{
            		exit(0);
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
                		printf("fd(%d) read/write(%d):%s\n",ev.sock,n,buf);
            		}
            		else
            		{//n==0;closed;
            			KEPOLL::del(g_efd,ev.sock);
            			close(ev.sock);
            			printf("fd(%d) close\n",ev.sock);
            		}
        		}
        		if (ev.error)
        		{
        			KEPOLL::del(g_efd,ev.sock);
        			close(ev.sock);
        			printf("fd(%d) error close \n",ev.sock);
        		}
        	}
        }
    }
    return 0;
}
