//client.cpp

#include <sys/types.h>
#include <sys/socket.h>//socket/bind;
#include <netinet/in.h>
#include <arpa/inet.h>//htons;
#include <unistd.h>//read/write;

#include <cstdio>

int main()
{
    printf("client running...\n");
    int fd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9866);

    //bind;
    connect(fd,(sockaddr*)&addr,sizeof(sockaddr));

    char buf[1024] ;
    int n = sprintf(buf,"%s","test01");

    write(fd,buf,n);

    n = read(fd,buf,1024);

    printf("read/write(%d):%s\n",n,buf);
    close(fd);
    return 0;
}