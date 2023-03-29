#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <memory>
#include <functional>

using std::vector;
using std::map;
using std::shared_ptr;
using std::function;

class Acceptor;//前向声明(一个不完整的类型)
class TcpConnection;//前向声明

using TcpConnectionPtr = shared_ptr<TcpConnection>;

class EventLoop
{
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();

    void loop();
    void unloop();

    void setConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCallback(TcpConnectionCallback &&cb);
private:
    void waitEpollFd();//调用epoll_wait函数,进行阻塞监听
    void handleNewConnection();//处理新的连接
    void handleMessage(int fd);//处理消息的发送

    int createEpollFd();//创建epfd
    void addEpollReadFd(int fd);//将fd添加到红黑树监听
    void delEpollReadFd(int fd);//从红黑树上取消监听

private:
    int _epfd;//红黑树的根节点
    vector<struct epoll_event> _evtList;//存储满足条件的事件
    bool _isLooping;//循环的标志位
    Acceptor &_acceptor;
    map<int, TcpConnectionPtr> _conns;//键值对

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;

};

#endif
