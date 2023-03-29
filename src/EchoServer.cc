#include "../include/EchoServer.h"
#include "../include/EventLoop.h"
#include "../include/Mylogger.hpp"
#include "../include/CacheManager.h"
#include "../lib/nlohmann/json.hpp"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using nlohmann::json;

MyTask::MyTask(Message msg, const TcpConnectionPtr &con)
    : _msg(msg)
    , _con(con)
    {

    }

void MyTask::process()
{
    //处理业务逻辑
    //deconde
    //compute
    //encode
    //要让线程池告诉EventLoop线程去进行数据的发送
    //如何让线程池告诉EventLoop线程去进行收发数据，什么时候告诉呢？
    //就涉及到线程与线程之间去进行通信
    //线程池是进行业务逻辑处理的，是进行计算的，将这种线程称为计算线程
    //将数据进行收发，进行读写操作的EventLoop/Reactor线程，称为IO线程
    //计算线程（线程池）如何与IO线程（EventLoop）进行通信？
    //eventfd可以在进程或者线程之前进行通信.
     if(_msg.id == 1){
        //关键字查询
        LogInfo(_con->toString().append(": ").append(_msg.mess));
        string tmp(_msg.mess);
        ::memset(&_msg, 0, sizeof(_msg));
        _msg.id = 100;
        json j_keyword;
        j_keyword["keyword"] = tmp;
        vector<string> ret1 = Module_1::GetInstance()->RecommendEnd(tmp, 5);
        if(ret1.size() != 0){
            for(int i = ret1.size(); i < 5; ++i){
                ret1.push_back("null");
            }
            for(int i = 0; i < 5; ++i){
                j_keyword["result"][i] = ret1[i];
            }
            LogInfo(_con->toString().append(": result of ").append(tmp).append(" ").append(_msg.mess));
        }else{
            vector<string> ret2 = Module_1::GetInstance()->RecommendZh(tmp, 5);
            if(ret2.size()){
                for(int i = ret2.size(); i < 5; ++i){
                    ret2.push_back("null");
                }
                for(int i = 0; i < 5; ++i){
                    j_keyword["result"][i] = ret2[i];
                }
                LogInfo(_con->toString().append(": result of ").append(tmp).append(" ").append(_msg.mess));
            }else{
                _msg.id = 0;
                string value("No relevant keywords found.");
                j_keyword["result"] = value;
                LogInfo(_con->toString().append(": result of ").append(tmp).append(" ").append(value));
            }
        }
        sprintf(_msg.mess, "%s", j_keyword.dump().c_str());
    }else if(_msg.id == 2){
        //网页查询
        string contnet(_msg.mess);
        LogInfo(_con->toString().append(": web search of ").append(_msg.mess));
        json j_web;
        j_web["web search"] = contnet;

        result_type2 re = Module_2::GetInstance()->Query(contnet);
        ::memset(&_msg, 0, sizeof(_msg));
        _msg.id = 200;
        if(re->webpages.size()){
            int number = 1;
            for(auto & page : re->webpages){
                j_web["web search result"][number]["abstract"] = page["abstract"];
                j_web["web search result"][number]["title"] = page["title"];
                j_web["web search result"][number]["url"] = page["url"];
                if(number++ > 11) break;
            }
            LogInfo(_con->toString().append(": ").append(contnet).append(": search success."));
        }else{
            _msg.id = 0;
            j_web["web search result"] = "No relerant page found";
            LogInfo(_con->toString().append(": ").append(contnet).append(": No relevant page found."));
        }
        ::sprintf(_msg.mess, "%s", j_web.dump().c_str());
        _msg.len = 2 * sizeof(int) + j_web.dump().size();
    }
        
    _con->sendInLoop(_msg);
    //TcpConnection要发送数据，此时TcpConnection要知道发送的对象EventLoop
    //的存在,在TcpConnection中设计一个EventLoop的指针或者引用
}


EchoServer::EchoServer(size_t threadNum, size_t queSize
            , const string &ip,
            unsigned short port)
: _pool(threadNum, queSize)//
, _server(ip, port)//
{

}

EchoServer::~EchoServer()
{

}


void EchoServer::start()
{
    _pool.start();//启动工作线程池
    //遍历每条线程，初始化所有子线程的缓存
    auto& manager=Singleton<WebPageCacheManager>::getInstance();
    for(auto& ptr:_pool.getThreads()){
        manager[ptr->getId()].Get("init");
    }
    using namespace std::placeholders;
    _server.setAllCallback(std::bind(&EchoServer::onConnection, this, _1),//设置所有的回调函数
                               std::bind(&EchoServer::onMessage, this, _1),
                               std::bind(&EchoServer::onClose, this, _1));
    _server.setTimerCallback(std::bind(&EchoServer::onTimer, this));//设置定时器回调
    _server.start();
}

void EchoServer::stop()
{
    _pool.stop();
    _server.stop();
}

void EchoServer::onConnection(const TcpConnectionPtr &con)
{
    //cout << con->toString() << " has connected!" << endl;
    LogInfo(con->toString().append(" has connected!"));
}
    
void EchoServer::onMessage(const TcpConnectionPtr &con)
{
    Message msg = con->receive();
    //业务逻辑的处理
    /* string s1 = msg; */
    MyTask task(msg, con);
    //处理完去进行发送
    _pool.addTask(std::bind(&MyTask::process, task));//只能采用值传递
}

void EchoServer::onTimer(){
    //cout << "onTimer" << endl;
    //同步线程缓存
    Singleton<WebPageCacheManager>::getInstance().SynchronizeCache();
}
    
void EchoServer::onClose(const TcpConnectionPtr &con)
{
    //cout << con->toString() << " has closed!" << endl;
    LogInfo(con->toString().append(" has closed!"));
    
}

