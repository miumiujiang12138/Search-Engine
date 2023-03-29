#ifndef __INETADDRESS_H__
#define __INETADDRESS_H__

#include <arpa/inet.h>
#include <string>

using std::string;

class InetAddress
{
public:
    InetAddress(const string &ip, unsigned short port);//传入绑定的IP,端口号
    InetAddress(const struct sockaddr_in &addr);//根据套接字结构体的构造函数
    ~InetAddress();
    string ip() const;//返回IP
    unsigned short port() const;//返回端口号
    const struct sockaddr_in *getInetAddrPtr() const;//获取套接字结构体

private:
    struct sockaddr_in _addr;//套接字结构体，存储信息

};

#endif
