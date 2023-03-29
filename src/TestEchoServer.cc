#include "../include/EchoServer.h"
#include <iostream>
#include <unistd.h>
#include "../include/Module_1.hpp"
#include "../include/Module2.hpp"

using std::cout;
using std::endl;

Module_1* Module_1::_p_module = nullptr;
Module_2* Module_2::_p_module = nullptr;

int main(int argc, char **argv)
{
    if(argc!=2){
        ::perror("argc!=2\n");
        ::exit(1);
    }
    string tmp=argv[1];
    if(tmp=="1")//以守护进程启动
    {
        if(daemon(1,0)<0)
        {
            ::perror("error daemon..\n");
            ::exit(1);
        }
    }
    OfflineConfig::getInstance().load(OFFLINE_JSON);
    Module_1::GetInstance()->LoadDatZh();
    ::printf("load Module 1 zh down\n");
    Module_1::GetInstance()->LoadDatEnd();
    ::printf("load Module 1 end down\n");
    Module_2::GetInstance()->LoadInvertIndex();
    //参数依次是：线程数目，任务队列大小，服务器监听套接字的IP地址，端口号
    EchoServer es(4, 10, "127.0.0.1", 8888);
    es.start();

    return 0;
}

