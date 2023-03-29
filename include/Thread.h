#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>

using std::function;


class Thread
{
    //重定义
    using ThreadCallback = function<void()>;
public:
    Thread(ThreadCallback &&cb);
    virtual ~Thread();

    void start();
    pthread_t getId() const{return _thid;}
    void stop();
private:
    //线程入口函数
    static void *threadFunc(void *arg);

private:
    pthread_t _thid;
    bool _isRunning;
    ThreadCallback _cb;

};

#endif
