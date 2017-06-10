//server.cpp

#include <sys/types.h>
#include <sys/socket.h>//socket/bind;
#include <arpa/inet.h>//htons;
#include <unistd.h>//read/write/close;

#include <cstdio>

int main()
{
    printf("server running...\n");
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9866);

    bind(lfd,(sockaddr*)&addr,sizeof(sockaddr));

    listen(lfd,SOMAXCONN);

    while (true)
    {       
        int rwfd = accept(lfd,NULL,NULL);
        char buf[1024];
        int n = read(rwfd,buf,1024);
        write(rwfd,buf,n);
        printf("read/write(%d):%s\n",n,buf);
        close(rwfd);
    }
    return 0;
}
