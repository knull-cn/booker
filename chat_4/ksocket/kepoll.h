#ifndef _K_EPOLL_H__
#define _K_EPOLL_H__

#include <cstdint>
#include <cstddef>//NULL;

#include <sys/epoll.h>
#include <unistd.h>

namespace KSOCKET
{
struct KEvent {
	void * s;
	int  sock;
	bool read;
	bool write;
	bool error;
};

const int32_t ONE_TIME_EPOLL_BUFFER = 128;

inline int Create()
{
	return epoll_create1(EPOLL_CLOEXEC);
}

inline void Release(int efd)
{
	close(efd);
}

inline bool Add(int efd, int sock, void *ud)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	ev.data.fd = sock;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1)
	{
		return false;
	}
	return true;
}

inline void Del(int efd, int sock)
{
	epoll_ctl(efd, EPOLL_CTL_DEL, sock , NULL);
}

inline void Write(int efd, int sock, void *ud, bool enable) 
{
	struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = ud;
	ev.data.fd = sock;
	epoll_ctl(efd, EPOLL_CTL_MOD, sock, &ev);
}

inline int Wait(int efd, struct KEvent *e, int max)
{
    struct epoll_event evbuf[ONE_TIME_EPOLL_BUFFER];
	if (max > ONE_TIME_EPOLL_BUFFER)
	{
		max = ONE_TIME_EPOLL_BUFFER;
	}
	int n = epoll_wait(efd , evbuf, max, -1);
	for (int i=0; i<n; i++)
	{
		e[i].s = evbuf[i].data.ptr;
		e[i].sock = evbuf[i].data.fd;
		uint32_t flag = evbuf[i].events;
		e[i].write = (flag & EPOLLOUT) != 0;
		e[i].read = (flag & EPOLLIN) != 0;
		e[i].error = (flag & EPOLLERR) != 0;
	}

	return n;
}

}


#endif //_K_EPOLL_H__
