//
// Created by Lenovo on 2022/11/30.
//

#include "../include/Module2.hpp"
#include "../include/OfflineConfig.hpp"
#include "../include/CacheManager.h"


Module_2::Module_2():_lock() {
}

void Module_2::CreatWebPage() {
    _page_lib.BuildPage();
}

void Module_2::CreateInvertIndex() {
    _page_lib.CutRedundantPages();
    _page_lib.BuildInvertIndexTable();
    _page_lib.Store();
}

void Module_2::LoadInvertIndex() {
    //nothing to do
}

result_type2 Module_2::Query(const string &content) {
    auto& m = Singleton<WebPageCacheManager>::getInstance();
    //获取当前线程的缓存
    auto& cache = m[pthread_self()];
    auto res = cache.Get(content);
    //如果这次查询在缓存里，就直接返回缓存的结果
    if(res.second){
        return res.first;
    }
    //否则从磁盘上获取结果,并加入缓存
    MutexLockGuard lock(_lock);
    auto ret = _search.DoQuery(content);
    if(!ret->webpages.empty()){
        cache.Put(content, ret);
    }
    return ret;
}

