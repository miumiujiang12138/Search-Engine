#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

class EventLoop;//前向声明（此时EventLoop就不是一个完整类型）

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
public:
    TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const Message &msg);
    void sendInLoop(const Message &msg);
    Message receive();
    string toString();
    bool isClosed() const;

    void setConnectionCallback(const TcpConnectionCallback &cb);
    void setMessageCallback(const TcpConnectionCallback &cb);
    void setCloseCallback(const TcpConnectionCallback &cb);

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();
private:
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();
private:
    EventLoop *_loop;
    Socket _sock;
    SocketIO _sockIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;

    TcpConnectionCallback _onConnection;
    TcpConnectionCallback _onMessage;
    TcpConnectionCallback _onClose;
};

#endif
