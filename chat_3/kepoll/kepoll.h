#ifndef _KEPOLL_H__
#define _KEPOLL_H__



namespace KEPOLL
{
	struct event {
		void * s;
		int  sock;
		bool read;
		bool write;
		bool error;
	};
	//
	int create();
	void release(int efd);
	int add(int efd, int sock, void *ud);
	void del(int efd, int sock);
	int wait(int efd, struct event *e, int max);
}

#endif//_KEPOLL_H__
