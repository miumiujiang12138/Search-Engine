#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "TcpServer.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using std::endl;

void onConnection(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has connected!" << endl;
}

void onMessage(const TcpConnectionPtr &con)
{
    string msg = con->receive();
    cout << "msg = " << msg << endl;
    con->send(msg);
}

void onClose(const TcpConnectionPtr &con)
{
    cout << con->toString() << " has closed!" << endl;

}

void test()
{
    TcpServer server("127.0.0.1", 8888);
    server.setAllCallback(std::move(onConnection), 
                          std::move(onMessage), 
                          std::move(onClose));
    server.start();
}
int main(int argc, char **argv)
{
    test();
    return 0;
}

