#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>

using std::cout;
using std::endl;

EventLoop::EventLoop(Acceptor &acceptor)
: _epfd(createEpollFd())
, _evfd(createEventFd())
, _evtList(1024)
, _isLooping(false)
, _acceptor(acceptor)
{
    //监听listenfd
    addEpollReadFd(_acceptor.fd());
    //eventfd的返回值进行监听
    addEpollReadFd(_evfd);
}

EventLoop::~EventLoop()
{
    close(_epfd);
    close(_evfd);
}

void EventLoop::loop()
{
    _isLooping = true;
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
            {
                if(_evtList[idx].events & EPOLLIN)
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
    int peerfd = _acceptor.accept();
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
    int fd = epoll_create1(0);
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
