#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "Thread.h"
#include "TaskQueue.h"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

using Task = function<void()>;

class ThreadPool
{
public:
    ThreadPool(size_t threadNum, size_t queSize);
    ~ThreadPool();

    //线程池开始与退出
    void start();
    void stop();

    //任务的获取与执行
    void addTask(Task &&task);
private:
    Task getTask();

    //线程池交给工作线程所做的任务
    void threadFunc();

private:
    size_t _threadNum;//子线程的数目
    size_t _queSize;//任务队列的大小
    vector<unique_ptr<Thread>> _threads;//存储子线程的容器
    TaskQueue _taskQue;//任务队列
    bool _isExit;//线程池退出的标志

};

#endif
