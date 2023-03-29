#include "../include/Acceptor.h"
#include "../include/Mylogger.hpp"
#include <stdio.h>

Acceptor::Acceptor(const string &ip, unsigned short port)
: _listenSock()
, _servAddr(ip, port)
{
}

Acceptor::~Acceptor()
{

}

void Acceptor::ready()//开启服务器(地址复用，端口复用，绑定，开启监听)
{
    setReuseAddr();//地址复用
    setReusePort();//端口复用
    bind();//绑定
    listen();//开启监听状态
}

void Acceptor::setReuseAddr()
{
    int on = 1;
    int ret = setsockopt(_listenSock.fd(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    //SO_REUSEADDR用于对TCP套接字处于TIME_WAIT状态下的socket，允许重复绑定使用。server程序总是应该在调用bind()之前设置SO_REUSEADDR套接字选项。
    if(ret)
    {
        perror("setsockopt");
        LogError("setsockopt");
        return;
    }
}

void Acceptor::setReusePort()
{
    int on = 1;
    int ret = setsockopt(_listenSock.fd(), SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
    //SO_REUSEPORT是允许多个socket绑定到同一个ip+port上。SO_REUSEADDR用于对TCP套接字处于TIME_WAIT状态下的socket，才可以重复绑定使用。

    //两者使用场景完全不同。
    //SO_REUSEADDR这个套接字选项通知内核，如果端口忙，但TCP状态位于TIME_WAIT，可以重用端口。
    //这个一般用于当你的程序停止后想立即重启的时候，如果没有设定这个选项，会报错EADDRINUSE，
    //需要等到TIME_WAIT结束才能重新绑定到同一个ip+port上。
    //而SO_REUSEPORT用于多核环境下，允许多个线程或者进程绑定和监听同一个ip+port，无论UDP、TCP（以及TCP是什么状态）。
    if(-1 == ret)
    {
        perror("setsockopt");
        LogError("setsockopt");
        return;
    }
}

void Acceptor::bind()
{
    int ret = ::bind(_listenSock.fd(), 
                     (struct sockaddr *)_servAddr.getInetAddrPtr(),
                     sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
        LogError("bind");
        return;
    }
}

void Acceptor::listen()
{
    int ret = ::listen(_listenSock.fd(), 128);
    if(-1 == ret)
    {
        perror("listen");
        LogError("listen");
        return;
    }
}

int Acceptor::accept()
{
    int connfd = ::accept(_listenSock.fd(), nullptr, nullptr);
    if(-1 == connfd)
    {
        perror("listen");
        LogError("listen");
        return -1;
    }
    return connfd;
    
}
int Acceptor::fd() const
{
    return _listenSock.fd();
}
