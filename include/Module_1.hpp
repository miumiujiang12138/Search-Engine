#ifndef _CTL_MODULE_1_HPP_
#define _CTL_MODULE_1_HPP_

#include "../include/dictionaries.hpp"
#include "../include/KeywordRecommender.h"
#include "../include/ChineseDictionary.h"
#include "../include/SplitTool.hpp"
#include "../include/hotspot.hpp"


using result_type = vector<string>;

//设置成单例
class Module_1
{
public:
    static Module_1* GetInstance()
    {
        if(_p_module==nullptr)
        {
            atexit(Destroy);
            _p_module=new Module_1();
        }
        return _p_module;
    }
    static void Destroy(){
        if(_p_module)
        {
            delete _p_module;
            _p_module=nullptr;
        }
    }

    //根据语料创建 .dat 文件
    void CreatDatEnd();//英文关键字
    void CreatDatZh();//中文关键字

    //加载.dat 文件
    //在内存或者缓冲区上加载完整表，保证查询
    void LoadDatEnd();
    void LoadDatZh();

    //关键字推荐
    result_type RecommendEnd(string keyword,size_t num);//英文关键字推荐
    result_type RecommendZh(string keyword,size_t num);//中文关键字推荐
    
    //删除赋值语句，拷贝构造，赋值运算符
    Module_1(const Module_1 &rhs)=delete;
    Module_1& operator =(const Module_1 &ths)=delete;
private:
    //将构造,析构函数私有化
    Module_1();
    ~Module_1() {}
    static Module_1* _p_module;
    dictionaries _dic_end;//英文关键字推荐需要的对象实体
    KeywordRecommender _dic_zn;//中文关键字
    hotspot _hotspot;//热点关键字
};



#endif
