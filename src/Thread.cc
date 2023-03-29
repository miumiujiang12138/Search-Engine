#include "../include/Thread.h"
#include <stdio.h>

Thread::Thread(ThreadCallback &&cb)
: _thid(0)
, _isRunning(false)
, _cb(std::move(cb))//注册
{

}

Thread::~Thread()
{
    if(_isRunning)
    {
        pthread_detach(_thid);
    }
}

//start参数列表中隐含着指向对象本身的this
void Thread::start()
{
    //threadFunc函数必须要写成一个静态成员函数，消除this的影响
    int ret = pthread_create(&_thid, nullptr, threadFunc, this);
    if(ret)
    {
        perror("pthread_create");
        return;
    }

    _isRunning = true;
}

void Thread::stop()
{
    if(_isRunning)
    {
        int ret = pthread_join(_thid, nullptr);
        if(ret)
        {
            perror("pthread_join");
            return;
        }
        _isRunning = false;
    }
}

void *Thread::threadFunc(void *arg)
{
    Thread *ph = static_cast<Thread *>(arg);
    if(ph)
    {
        //执行回调函数
        ph->_cb();//线程所做的任务
    }

    /* return nullptr; */
    pthread_exit(nullptr);
}
