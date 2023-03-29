#include "../include/EventLoop.h"
#include "../include/Acceptor.h"
#include "../include/TcpConnection.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include <iostream>

using std::cout;
using std::endl;

int timerCreate();
void setTimerFd(int fd,long sec,long ns);

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())//创建epoll的句柄
, _evfd(createEventFd())//创建eventfd句柄，用于生产者和消费者之间的事件通知
, _evtList(1024)//初始化保存epoll_event结构体的容器，大小为1024
, _isLooping(false)
,_timer_fd(timerCreate())
, _acceptor(acceptor)
{
    //监听listenfd
    addEpollReadFd(_acceptor.fd());//将服务器的监听套接字加入epoll的 读 监听
    //eventfd的返回值进行监听
    addEpollReadFd(_evfd);//将eventfd句柄也加入，epoll的读监听，因为eventfd始终可写，所有监听写事件没有意义
}

EventLoop::~EventLoop()
{
    close(_epfd);
    close(_evfd);
}

void EventLoop::loop()
{
    _isLooping = true;
    addEpollReadFd(_timer_fd);
    setTimerFd(_timer_fd,10,0);
    while(_isLooping)
    {
        waitEpollFd();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::waitEpollFd()
{
    int nready;
    do
    {
        nready = epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), 5000);//第二个参数的传递
    }while(-1 == nready && errno == EINTR);
   
    if(-1 == nready)
    {
        perror("epoll_wait");
        return;
    }
    else if(0 == nready)
    {
        printf(">>epoll_wait timeout\n");
    }
    else
    {
        if(nready == (int)_evtList.size())
        {
            _evtList.resize(2 * nready);//扩容操作
        }

        for(int idx = 0; idx < nready; ++idx)
        {
            int fd = _evtList[idx].data.fd;
            if(fd == _acceptor.fd())//新的连接
            //如果监听套接字可以读，说明有新的连接加入
            {
                if(_evtList[idx].events & EPOLLIN)//判断是不是读事件
                {
                    handleNewConnection();
                }
            }
            else if(fd == _evfd)//监听的_evfd有事件发生
            {
                handleRead();//阻塞
                //多个回调函数全部都执行一下
                //也就是遍历vector
                doPengdingFunctors();//执行所有任务
            }
            else if(fd == _timer_fd)//timer fd
            {
                handleTimer(fd);
            }
            else
            {
                if(_evtList[idx].events & EPOLLIN)
                {
                    handleMessage(fd);//老的连接上有数据
                }

            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerfd = _acceptor.accept();//获取监听套接字里，连接成功的套接字
    if(peerfd < 0)
    {
        perror("peerfd");
        return;
    }
    //将该文件描述符放在红黑树上进行监听
    addEpollReadFd(peerfd);

    TcpConnectionPtr con(new TcpConnection(peerfd, this));
    /* TcpConnection con(peerfd); */
    //三个事情的注册
    con->setConnectionCallback(_onConnection);
    con->setMessageCallback(_onMessage);
    con->setCloseCallback(_onClose);

    _conns.insert(std::make_pair(peerfd, con));
    //执行连接的建立
    con->handleConnectionCallback();
}

void EventLoop::handleMessage(int fd)
{
    auto it = _conns.find(fd);
    if(it != _conns.end())
    {
        //直接判断读的结果是否为0
        bool flag = it->second->isClosed();
        if(flag)
        {
            //处理连接的断开
            it->second->handleCloseCallback();
            delEpollReadFd(fd);//将文件描述符从红黑树上删除
            _conns.erase(it);//将文件描述符以及对应的连接从map中删除
        }
        else
        {
            //进行正常的读写操作
            it->second->handleMessageCallback();
        }
    }
    else
    {
        cout << "该连接不存在" << endl;
    }
}

int EventLoop::createEpollFd()
{
    int fd = epoll_create1(0);//返回值就是一个epoll的文件描述符efd
    if(-1 == fd)
    {
        perror("epoll_create1");
        return fd;
    }

    return fd;
}

void EventLoop::addEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if(-1 == ret)
    {
        perror("epoll_ctl add");
        return;
    }
}

void EventLoop::delEpollReadFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if(-1 == ret)
    {
        perror("epoll_ctl del");
        return;
    }
}

void EventLoop::setConnectionCallback(TcpConnectionCallback &&cb)
{
    _onConnection = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
{
    _onMessage = std::move(cb);
}
void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
{
    _onClose = std::move(cb);
}
    
void EventLoop::handleRead()
{
    uint64_t one = 1;
    int ret = read(_evfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read");
        return;
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    int ret = write(_evfd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write");
        return;
    }

}

int EventLoop::createEventFd()
{
    int fd = eventfd(10, 0);
    //创建eventfd，初始化计数器为10
    //eventfd:专门用于事件通知的文件描述符
    //这里配合epoll，可以实现消费者和生产者之间优雅的事件通知。
    if(fd < 0)
    {
        perror("eventfd");
        return fd;
    }
    return fd;
}

//vector<int>(10);
//func(vector<int> &&cb)
void EventLoop::doPengdingFunctors()
{
    vector<Functor> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendings);
    }

    for(auto &cb : tmp)
    {
        cb();
    }
}

void EventLoop::runInLoop(Functor &&cb)
{
    //粒度,锁的时候粒度要小一点，代码返回
    {
        MutexLockGuard autoLock(_mutex);
        _pendings.push_back(std::move(cb));
    }

    wakeup();
}

void EventLoop::setTimerCallback(Functor &&cb) {
    _on_timer = std::move(cb);
}

void EventLoop::handleTimer(int fd) {
    uint64_t tmp=0;
    auto ret=::read(fd, &tmp, sizeof(tmp));
    if(ret==-1){
        if(errno==EINTR||errno==EAGAIN)return ;
    }
    if(ret!=sizeof(tmp)){
        perror("read");
        return ;
    }
    if(_on_timer)_on_timer();
}

int timerCreate(){
    int fd= timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
    if(fd==-1)perror("eventfd");
    return fd;
}

void setTimerFd(int fd,long sec,long ns){
    itimerspec ts={0};
    ts.it_value={sec,ns};
    ts.it_interval={sec,ns};
    timerfd_settime(fd,0,&ts,nullptr);
}
