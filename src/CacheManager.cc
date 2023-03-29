//
// Created by Lenovo on 2022/12/1.
//

#include "../include/CacheManager.h"
#include "../include/Mylogger.hpp"

void WebPageCacheManager::SynchronizeCache() {
    LogInfo("Synchronizing cache");
    //第一步：把所有缓存的pending放到第一个缓存里
    if(_cache_list.empty()) return;
    else if(_cache_list.size()==1){
        _cache_list[0].second.SaveCache(_cache_path);
        return;
    }
    auto& cache=_cache_list.begin()->second;
    for(auto& it:_cache_list){
        MutexLockGuard lock(*it.second._lock);
        auto& pending = it.second.GetPending();
        for(auto& item:pending){
            cache.Put(item.first,item.second,false);
        }
        pending.clear();
    }
    //第二步：存储缓存
    cache.SaveCache(_cache_path);
    //第三步：把之后的缓存和第一个缓存同步
    for(auto it=_cache_list.begin()+1;it!=_cache_list.end();++it){
        it->second.Update(cache);
    }
}

LRUCache<WebPageQueryResult> &WebPageCacheManager::operator[](pthread_t key) {
    for(auto & it : _cache_list){
        if(it.first==key){return it.second;}
    }
    auto lock = new MutexLock;
    auto cache= LRUCache<WebPageQueryResult>(_max_size,lock);
    _locks.push_back(lock);
    cache.LoadCache(_cache_path);
    _cache_list.emplace_back(key,std::move(cache));
    return _cache_list[_cache_list.size()-1].second;
}
