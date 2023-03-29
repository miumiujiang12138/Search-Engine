//
// Created by Lenovo on 2022/12/2.
//

#include "../../include/PageLibPreprocessor.h"

#include <chrono>
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::duration;

int main(){
    PageLibPreprocessor pp;
    auto start=system_clock::now();
    pp.BuildPage();
    auto end=system_clock::now();
    std::cout<<"建立以及持久化网页库和偏移库耗时: "<< duration_cast<seconds>(end-start).count()<<"s"<<std::endl;
    start=end;
    pp.CutRedundantPages();
    end=system_clock::now();
    std::cout<<"去重耗时: "<< duration_cast<seconds>(end-start).count()<<"s"<<std::endl;
    start=end;
    pp.BuildInvertIndexTable();
    end=system_clock::now();
    std::cout<<"建立倒排索引耗时: "<< duration_cast<seconds>(end-start).count()<<"s"<<std::endl;
    start=end;
    pp.Store();
    end=system_clock::now();
    std::cout<<"持久化去重后的网页库和偏移库以及倒排索引表耗时: "<< duration_cast<seconds>(end-start).count()<<"s"<<std::endl;
    std::cout<<"success"<<std::endl;
    return 0;
}