//
// Created by Lenovo on 2022/11/30.
//

#ifndef TEST_MODULE2_HPP
#define TEST_MODULE2_HPP

#include "../include/WebPageSearcher.h"
#include "../include/PageLibPreprocessor.h"
#include "../include/MutexLock.h"

using result_type2 = shared_ptr<WebPageQueryResult>;


class Module_2{
public:
    static Module_2* GetInstance(){
        if(_p_module==nullptr){
            atexit(Destroy);
            _p_module=new Module_2();
        }
        return _p_module;
    }
    static void Destroy(){
        if(_p_module){
            delete _p_module;
            _p_module=nullptr;
        }
    }

    //根据xml创建 网页库
    void CreatWebPage();//创建网页库、偏移库

    //创建倒排索引
    void CreateInvertIndex();

    //加载.dat 文件
    //在内存或者缓冲区上加载完整表，保证查询
    void LoadInvertIndex();

    result_type2 Query(const string& content);

    //删除赋值语句，拷贝构造，赋值运算符
    Module_2(const Module_2 &rhs)=delete;
    Module_2& operator =(const Module_2 &ths)=delete;
private:
    //将构造,析构函数私有化
    Module_2();
    ~Module_2() {}
    static Module_2* _p_module;
    WebPageSearcher _search;
    PageLibPreprocessor _page_lib;
    MutexLock _lock;
};


#endif //TEST_MODULE2_HPP
