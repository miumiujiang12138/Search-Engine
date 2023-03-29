#ifndef _CTL_REDIS_HPP_
#define _CTL_REDIS_HPP_


#include <cstdio>
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include "../lib/hiredis/hiredis.h"
#include "../include/MutexLock.h"

#define REDIS_IP "127.0.0.1"
#define REDIS_PORT 6379


using std::vector;
using std::string;

class redis
{
public:
    redis() {}
    ~redis() {
        this->_connect=nullptr;
        // this->_reply=nullptr;
    }
    //创建连接
    bool connect(std::string host,int port)
    {
        this->_connect=redisConnect(host.c_str(),port);
        if(this->_connect != nullptr && this->_connect->err)
        {
            ::printf("connect error : %s \n",this->_connect->errstr);
            return false;
        }
        ::printf("connect redis server:%s %d\n",host.c_str(),port);
        return true;
    }
    //get请求
    std::string get(std::string key);
    //set请求
    void set(std::string key,std::string value);
    //sadd请求
    void sadd(std::string key,std::string member);
    //sinter请求，取交集
    vector<string> sinter(std::string key1,std::string key2);
    //切换数据库select index
    void select(std::string index);
    //设置生存时间，单位秒
    bool expire(std::string msg,std::string times);
    void LockMutex(){_mutelock.lock();}
    void UnlockMutex(){_mutelock.unlock();}
private:
    redisContext* _connect;
    MutexLock _mutelock;
    // redisReply* _reply;
};

#endif
