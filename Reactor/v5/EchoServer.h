#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include "EventLoop.h"
#include "ThreadPool.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }
    void process()
    {
        //处理业务逻辑
        //deconde
        //compute
        //encode
        //要让线程池告诉EventLoop线程去进行数据的发送
        //如何让线程池告诉EventLoop线程去进行收发数据，什么时候告诉呢？
        //就涉及到线程与线程之间去进行通信
        //线程池是进行业务逻辑处理的，是进行计算的，将这种线程称为计算线程
        //将数据进行收发，进行读写操作的EventLoop/Reactor线程，称为IO线程
        //计算线程（线程池）如何与IO线程（EventLoop）进行通信？
        //eventfd可以在进程或者线程之前进行通信.
        _con->sendInLoop(_msg);
        //TcpConnection要发送数据，此时TcpConnection要知道发送的对象EventLoop
        //的存在,在TcpConnection中设计一个EventLoop的指针或者引用
    }
private:
    string _msg;
    TcpConnectionPtr _con;
};


class EchoServer
{
public:
    EchoServer(size_t threadNum, size_t queSize
               , const string &ip,
               unsigned short port)
    : _pool(threadNum, queSize)
    , _server(ip, port)
    {

    }

    ~EchoServer()
    {

    }

    void start()
    {
        _pool.start();
        using namespace std::placeholders;
        _server.setAllCallback(std::bind(&EchoServer::onConnection, this, _1),
                               std::bind(&EchoServer::onMessage, this, _1),
                               std::bind(&EchoServer::onClose, this, _1));
        _server.start();
    }

    void stop()
    {
        _pool.stop();
        _server.stop();
    }

    void onConnection(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has connected!" << endl;
    }
    
    void onMessage(const TcpConnectionPtr &con)
    {
        string msg = con->receive();
        cout << "msg = " << msg << endl;
        //业务逻辑的处理
        //...
        //...
        /* string s1 = msg; */
        MyTask task(msg, con);
        _pool.addTask(std::bind(&MyTask::process, task));//只能采用值传递
    }
    
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    
    }
private:
    ThreadPool _pool;
    TcpServer _server;
};

#endif
