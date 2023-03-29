#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#define MSG_SIZE 10240

//传输协议
struct Message
{
    int len;
    int id;
    char mess[MSG_SIZE];
};

class SocketIO
{
public:
    explicit SocketIO(int fd);
    ~SocketIO();
    int readn(Message *buf, int len);
    /*int readLine(char *buf, int len);*/
    int writen(const Message *buf, int len);

private:
    int _fd;

};

#endif
