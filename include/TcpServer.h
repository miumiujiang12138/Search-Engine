#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"

//创建TCP监听套接字
class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port)//参数：要绑定的IP地址和端口号
    : _acceptor(ip, port)
    , _loop(_acceptor)//依赖_acceptor,或者说要将监听套接字放入epoll中
    {

    }

    ~TcpServer()
    {

    }

    void start()
    {
        _acceptor.ready();//TCP连接就绪
        _loop.loop();
    }

    void stop()
    {
        _loop.unloop();
    }

    void setAllCallback(TcpConnectionCallback &&onConnection,
                        TcpConnectionCallback &&onMessage,
                        TcpConnectionCallback &&onClose)
    {
        _loop.setConnectionCallback(std::move(onConnection));
        _loop.setMessageCallback(std::move(onMessage));
        _loop.setCloseCallback(std::move(onClose));

    }

    void setTimerCallback(Functor&& cb){_loop.setTimerCallback(std::move(cb));}

private:
    Acceptor _acceptor;//TCP连接
    EventLoop _loop;//消息队列，依赖于_acceptor


};

#endif
