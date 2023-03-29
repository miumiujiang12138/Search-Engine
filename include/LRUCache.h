//
// Created by Lenovo on 2022/12/1.
//

#ifndef TEST_LRUCACHE_H
#define TEST_LRUCACHE_H

#include "WebPageSearcher.h"
#include "../include/Mylogger.hpp"

#include <string>
#include <list>
#include <memory>
#include <unordered_map>
#include <fstream>
#include <iostream>

using std::ifstream;
using std::ofstream;

#include "../lib/nlohmann/json.hpp"

#include "../include/MutexLock.h"

using std::string;
using std::pair;
using std::list;
using std::unordered_map;
using std::make_shared;
using std::shared_ptr;

template<typename T>
class LRUCache{
public:
    using value_type = shared_ptr<T>;
    using return_type = pair<value_type,bool>;
    using key_type = string;
    using lru_type = list<pair<key_type,value_type>>;
    using table_type = typename lru_type::iterator;
public:
    LRUCache(size_t max_size,MutexLock* lock):_max_size(max_size),_lock(lock){}//LRU 缓存大小
public:
    void LoadCache(const string& filename);//从磁盘上加载缓存
    void SaveCache(const string& filename);//保存缓存到磁盘
public:
    return_type Get(const key_type& key);
    void Put(const key_type& key,value_type value,bool update_pending=true);
    void Update(const LRUCache& cache);
    lru_type& GetLru(){return _lru;}
    lru_type& GetPending(){return _pending;}
private:
    void Set(const key_type& key,const value_type& value);
public:
    MutexLock* _lock;
private:
    size_t _max_size;
    unordered_map<key_type,table_type> _table;
    lru_type _pending;
    lru_type _lru;
};

template<typename T>
void LRUCache<T>::LoadCache(const string &filename) {
    ifstream ifs(filename);
    if(!ifs){
        LogWarn("cache file not found");
        return;
    }
    nlohmann::json json;
    try{
        ifs>>json;
    }
    catch(const nlohmann::json::exception& e){
        LogWarn("cache file is corrupted");
        ifs.close();
        //缓存格式损坏,清空缓存
        ofstream ofs(filename,std::ios::trunc|std::ios::out);
        if(ofs)ofs.close();
        return;
    }
    for(auto& j:json.items()){
        auto result=make_shared<WebPageQueryResult>();
        auto ele=j.value();
        result->keywords=ele["keywords"].get<vector<string>>();
        result->webpages=ele["webpages"].get<vector<unordered_map<string,string>>>();
        Set(j.key(),result);
    }
    ifs.close();
}

template<typename T>
void LRUCache<T>::SaveCache(const string &filename) {
    nlohmann::json out;
    for(auto& it:_lru){
        out[it.first]["keywords"]=it.second->keywords;
        out[it.first]["webpages"]=it.second->webpages;
    }
    if(out.empty())return;
    ofstream ofs(filename,std::ios::trunc|std::ios::out);
    if(!ofs){
        LogWarn("cache file not found");
        return;
    }
    ofs<<out<<std::endl;
    ofs.close();
}

template<typename T>
typename LRUCache<T>::return_type LRUCache<T>::Get(const LRUCache::key_type &key) {
    if(_table.count(key)){
        auto tmp = _table[key]->second;
        _lru.splice(_lru.begin(),_lru,_table[key]);
        return std::make_pair(tmp,true);
    }
    return std::make_pair(value_type{},false);
}

template<typename T>
void LRUCache<T>::Set(const LRUCache::key_type &key, const LRUCache::value_type &value) {
    if(_table.count(key)){
        _table[key]->second = value;
        _lru.splice(_lru.begin(),_lru,_table[key]);
    }
    else{
        if(_table.size()==_max_size){
            _table.erase(_lru.back().first);
            _lru.pop_back();
        }
        _lru.emplace_front(key,value);
        _table[key] = _lru.begin();
    }
}

template<typename T>
void LRUCache<T>::Put(const LRUCache::key_type &key, value_type value, bool update_pending) {
    if(update_pending){
        MutexLockGuard lock(*_lock);
        _pending.emplace_front(key,value);
    }
    Set(key,value);
}

template<typename T>
void LRUCache<T>::Update(const LRUCache &cache) {
    for(auto it = cache._lru.rbegin(); it != cache._lru.rend();++it){
        Set(it->first,it->second);
    }
}


#endif //TEST_LRUCACHE_H
