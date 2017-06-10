//client.cpp

#include <sys/types.h>
#include <sys/socket.h>//socket/bind;
#include <netinet/in.h>
#include <arpa/inet.h>//htons;
#include <unistd.h>//read/write;

#include <cstdio>
#include <ctime>
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
    printf("client running...\n");
    

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9866);

    int idx = 0;
    int t = time(0)%10000;
    char buf[1024];
    for (int i=0; i< 10;i++ )
    {
        int fd = socket(AF_INET,SOCK_STREAM,0);
        //bind;
        if (0 > connect(fd,(sockaddr*)&addr,sizeof(sockaddr)))
        {
        	printf("connet error(%d):%s;\n",errno,strerror(errno));
        	return 0;
        }
        int n = sprintf(buf,"test_%d_%d_%d",t,fd,idx++);
        write(fd,buf,n);
        SetEpollRead(fd);
    }
    KEPOLL::event evbuf[128];
    int esize = 128;
    bool running = true;
    while (running)
    {
        int n = KEPOLL::wait(g_efd,evbuf,esize);
        if (n < 0)
        {
        	printf("epoll_wait error(%d):%s\n",errno,strerror(errno));
        	return 0;
        }
        //printf("recv events(%d)\n",n);
        for (int i=0; i<n; i++)
        {
        	KEPOLL::event ev = evbuf[i];
			if (ev.read)
			{
				int nr = read(ev.sock,buf,1024);
				if (nr > 0 )
				{
					int nw = sprintf(buf,"test_%d_%d_%d",t,ev.sock,idx++);
					write(ev.sock,buf,nw);
					printf("fd(%d) read/write(%d):%s\n",ev.sock,nw,buf);
				}
				else
				{//n==0;closed;
					close(ev.sock);
					printf("fd(%d) close\n",ev.sock);
				}
			}
			if (ev.error)
			{
				KEPOLL::del(g_efd,ev.sock);
				close(ev.sock);
				printf("err : fd(%d) close\n",ev.sock);
			}
			else
			{
				if (idx > 128)
				{
					KEPOLL::del(g_efd,ev.sock);
					close(ev.sock);
					printf("fd(%d) close\n",ev.sock);
				}
			}
        }
    }

    return 0;
}
