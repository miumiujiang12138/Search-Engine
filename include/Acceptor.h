#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Socket.h"
#include "InetAddress.h"
#include <string>

using std::string;

class Acceptor
{
public:
    Acceptor(const string &ip, unsigned short port);
    ~Acceptor();
    void ready();//将服务器的TCP套接字就绪
    void setReuseAddr();//地址复用
    void setReusePort();//端口复用
    void bind();//给套接字绑定地址
    void listen();//开启监听
    int accept();//等待连接，如果有连接，则返回新建连接的文件描述符
    int fd() const;

private:
    Socket _listenSock;//监听套接字
    InetAddress _servAddr;//构造套接字的结构体
};

#endif
