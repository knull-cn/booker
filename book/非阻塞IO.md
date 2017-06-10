#零、序言
??基础版本的NET/IO通讯，是阻塞式的，特别是Server端，完全就没法用于实际场景（client端，还可以写个测试代码什么的，还是有一定存在价值的）。所以，我们这里首先要做的是用非阻塞I/O。
??非阻塞I/O，有许多非常好的已经实现的代码，比如skynet/redis。个人不推荐muduo的，因为太全面，反而显的有些复杂，导致太关注细节，不适合学习、练习版本。个人推荐版本是用skynet的(socket_epoll.h)。因为相对非常简单、清楚。这里就是模仿的skynet的网络代码。
#一、准备知识
##1、这里需要首先掌握几个知识点
- 1、阻塞IO：之前举的例子中，read/write/connect/accept都是阻塞式的——需要等待对方做出处理，本地才能做对应的响应。
- 2、非阻塞IO：之前的read/write/connect/accept，如果在非阻塞式中，那么就是read之后直接返回——如果有数据就返回数据；没有数据，就返回错误码（EWOULDBLOCK/EAGAIN）。
      阻塞式IO太不爽了，太浪费了。凭什么要我等他？有就给我，没有就走——这就是非阻塞IO。
- 3、轮询：非阻塞是IO的确还不错，但是，有些人（数据），你还是不得不等的。所以，你不得不一次一次去看ta来了没。这就是轮询。
- 4、IO多路复用(IO multiplexing)：轮询，一个fd也是去看，N个fd也是去查看。所以，如果有多个IO fd，当然一起轮询啦，效率高啊。当然，这个事情操作系统可以帮你做，即select/poll/epoll(linux)/port(sun)/kqueue(bsd)。
- 5、异步IO：异步IO最大的特点是系统帮你准备好数据，然后通知你可以做事情了。
      1、我们的非阻塞IO是知道有数据了，但是还是自己去将数据从内核拷贝到用户buffer。异步IO是操作系统将数据拷贝到用户提供的buffer。
      2、 linux异步IO没用过，相关函数在aio.h中，函数也是aio_***形式的。

##2、总结
- _阻塞_ / _非阻塞_ 是fd的一个属性而已。阻塞 就是耿直boy，ta不来就死等，一直到ta来了再做对应的事情；非阻塞 就是会去看看什么情况，再决定做什么。
- IO multiplexing是_轮询_ 的一种具体实现方式。
- 异步IO，一般是会传个回调函数cb，而在做完要做的事情之后，会调用对应的回调函数cb。许多blog中，将epoll等也归为异步IO，这里看来满足这个条件，以read为例：aio_read是操作系统准备好数据，将数据给上层应用给的read_cb；epoll是网络库这一层将数据准备好，数据给read_cb。即一个是操作系统做了，一个网络库做了。但是，对上层来说，是一样一样的。
        回调函数，本质上其实是被动触发，即是异步的特质。

#二、非阻塞IO
- 1、epoll的准备工作：创建/销毁。
```c++
//要使用epoll，先要创建一个epoll的fd。在以后的epoll相关操作中，都是用该fd进行标识；
static int
sp_create() {
	return epoll_create(1024);
}
//既然是fd，当然需要close（linux的同学都知道的）
static void
sp_release(int efd) {
	close(efd);
}
```
- 2、epoll添加read/write事件监听。
  - 1、void *ud
         这里ud指的是user data;在C语言中，直接用void *指针指向用户相关数据；当事件触发回调的时候，该数据作为参数带出来传递给回调函数。
  - 2、sp_add做的操作，仅仅是添加EPOLLIN事件（简单来说就是读事件——可以想象成数据IN）。
  - 3、sp_write是修改某个socket fd的write属性——可以是增加或者删除该属性。
         一般情况下，write是不会阻塞的，所以直接调用write系统调用；如果在write缓冲区满了的情况下，write会返回失败。这种情况下，再添加写事件到epoll中去。当再次可以写的时候，会返回写事件，通知上层（或者说是调用回调函数）



```c++
static int 
sp_add(int efd, int sock, void *ud) {
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		return 1;
	}
	return 0;
}
static void 
sp_write(int efd, int sock, void *ud, bool enable) {
	struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = ud;
	epoll_ctl(efd, EPOLL_CTL_MOD, sock, &ev);
}
```