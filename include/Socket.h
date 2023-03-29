#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "NonCopyable.h"
//套接字
class Socket
: NonCopyable
{
public:
    Socket();
    explicit Socket(int fd);//不能用于隐式转化和赋值初始化
    ~Socket();
    int fd() const;//返回套接字的文件描述符
    void shutDownWrite();//关闭写端

private:
    int _fd;
};

#endif
