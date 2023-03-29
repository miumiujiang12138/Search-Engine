//
// Created by Lenovo on 2022/12/1.
//

#ifndef TEST_CACHEMANAGER_H
#define TEST_CACHEMANAGER_H

#include "LRUCache.h"
#include "WebPageSearcher.h"
#include "Singleton.hpp"
#include "MutexLock.h"

class WebPageCacheManager{
public:
    explicit WebPageCacheManager():_max_size(50),_cache_path("../data/cache"){}
    ~WebPageCacheManager(){
        for(auto it:_locks){
            delete it;
        }
    }
    void SynchronizeCache();
    LRUCache<WebPageQueryResult>& operator[](pthread_t key);
    void SetMaxSize(int max){_max_size = max;}
public:
    std::string _cache_path;
private:
    int _max_size;
    vector<MutexLock*> _locks;
    vector<pair<pthread_t,LRUCache<WebPageQueryResult>>> _cache_list;
};

#endif //TEST_CACHEMANAGER_H
