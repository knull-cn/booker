>本轮，主要是将server进行改造：server的accept是阻塞的，read是阻塞的，根本没法进行多个连接并发（除非用多个线程，但是一个连接一个线程也相对比较浪费）。这里进行非阻塞改造。

server非阻塞改造需要注意的问题：
- 1、设置fd为非阻塞属性；
- 2、加入到epoll中去；
- 3、当对端close的时候，event即有read又有error(EOF).
- 4、结束一个连接，需要做两个事情：1、从epoll中删除该fd；2、close该fd。