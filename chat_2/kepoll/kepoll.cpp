#include <cstdint>
#include <cstddef>//NULL;

#include <sys/epoll.h>
#include <unistd.h>

#include "kepoll.h"

using namespace KEPOLL;

const int32_t ONE_TIME_EPOLL_BUFFER = 128;
struct epoll_event evbuf[ONE_TIME_EPOLL_BUFFER];

int KEPOLL::create()
{
	return epoll_create1(EPOLL_CLOEXEC);
}

void KEPOLL::release(int efd)
{
	close(efd);
}

int KEPOLL::add(int efd, int sock, void *ud)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	ev.data.fd = sock;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1)
	{
		return 1;
	}
	return 0;
}

void KEPOLL::del(int efd, int sock)
{
	epoll_ctl(efd, EPOLL_CTL_DEL, sock , NULL);
}

int KEPOLL::wait(int efd, struct event *e, int max)
{
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
