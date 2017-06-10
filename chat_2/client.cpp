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

int main()
{
    printf("client running...\n");
    int fd = socket(AF_INET,SOCK_STREAM,0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9866);

    //bind;
    if (0 > connect(fd,(sockaddr*)&addr,sizeof(sockaddr)))
    {
    	printf("connet error(%d):%s;\n",errno,strerror(errno));
    	return 0;
    }
    printf("connect ok...");
    char buf[1024] ;
    int t = time(0)%10000;
    for (int i=0; i<20;i++)
    {
        int n = sprintf(buf,"test_%d_%d",t,i);

        write(fd,buf,n);

        n = read(fd,buf,1024);
        printf("read/write(%d):%s\n",n,buf);
    }



    close(fd);
    return 0;
}
