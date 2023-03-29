#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "ThreadPool.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include "../include/Module_1.hpp"
#include "../include/Module2.hpp"



class MyTask
{
public:
    MyTask(Message msg, const TcpConnectionPtr &con);

    void process();

private:
    Message _msg;
    TcpConnectionPtr _con;
};


class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip,
               unsigned short port);

    ~EchoServer();

    void start();

    void stop();

    void onConnection(const TcpConnectionPtr &con);
    
    void onMessage(const TcpConnectionPtr &con);

    void onTimer();
    
    void onClose(const TcpConnectionPtr &con);
private:
    ThreadPool _pool;
    TcpServer _server;//服务器套接字创建
};

#endif
