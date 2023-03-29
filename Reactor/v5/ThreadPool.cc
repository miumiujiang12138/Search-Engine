#include "ThreadPool.h"
#include <unistd.h>

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(queSize)
, _isExit(false)
{
    //预留空间
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool()
{
    if(!_isExit)
    {
        stop();
        _isExit = true;
    }
}

//线程池的启动
void ThreadPool::start()
{
    for(size_t idx = 0; idx != _threadNum; ++idx)
    {
        //function<void()> f = std::bind(&ThreadPool::threadFunc, this);
        unique_ptr<Thread> up(new Thread(std::bind(&ThreadPool::threadFunc, this)));
        _threads.push_back(std::move(up));//unique_ptr不能复制或者赋值
    }

    for(auto &th : _threads)
    {
        th->start();//启动所有的子线程
    }
}

void ThreadPool::stop()
{
    //先要判断任务有没有执行完毕，没有执行完毕就不让工作线程退出
    while(!_taskQue.empty())
    {
        sleep(1);
    }
    _isExit = true;

    _taskQue.wakeup();//唤醒所有休眠的线程
    for(auto &th : _threads)
    {
        th->stop();//回收所有的子线程
    }

}

void ThreadPool::addTask(Task &&task)
{
    if(task)
    {
        _taskQue.push(std::move(task));
    }
}

Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

//线程池交给工作线程做的任务
void ThreadPool::threadFunc()
{
    while(!_isExit)
    {
        //工作线程取获取任务的时候，如果getTask执行的比较慢，任务是
        //可以执行完毕，工作线程也是可以退出来的；但是如果工作线程
        //获取任务getTask的速度非常快的时候，会阻塞在getTask上面，
        //因为没有任务，就会阻塞
        //获取任务
        Task taskcb = getTask();
        if(taskcb)
        {
            taskcb();//执行任务
        }
    }
}
