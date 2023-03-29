#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
: _queSize(queSize)
, _que()
, _mutex()
, _notEmpty(_mutex)
, _notFull(_mutex)
, _flag(true)
{

}

TaskQueue::~TaskQueue()
{

}

bool TaskQueue::empty() const
{
    return 0 == _que.size();
}

bool TaskQueue::full() const
{
    return _que.size() == _queSize;
}

void TaskQueue::push(ElemType &&value)
{
    MutexLockGuard autoLock(_mutex);

    while(full())
    {
        _notFull.wait();//让生产者睡觉
    }

    _que.push(std::move(value));

    _notEmpty.notify();
}

ElemType TaskQueue::pop()
{
    MutexLockGuard autoLock(_mutex);

    while(_flag && empty())
    {
        //上半部：排队、释放锁、睡眠
        //下半部：被唤醒、上锁、返回
        _notEmpty.wait();//让消费者睡觉
    }

    if(_flag)
    {
        ElemType tmp = _que.front();
        _que.pop();

        _notFull.notify();
        return tmp;
    }
    else
    {
        return nullptr;
    }
}

void TaskQueue::wakeup()
{
    _flag = false;
    _notEmpty.notifyAll();
}
