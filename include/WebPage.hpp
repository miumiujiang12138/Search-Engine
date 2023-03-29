//
// Created by Lenovo on 2022/11/28.
//

#ifndef TEST_WEBPAGE_HPP
#define TEST_WEBPAGE_HPP

#include "../lib/simhash/Simhasher.hpp"
#include "../include/OfflineConfig.hpp"

#include <string>

using std::string;

class SimHash{
public:
    SimHash():_top_n(20){
        auto& cfg = OfflineConfig::getInstance();
        cfg.load("../conf/offline.json");
        _hash=new simhash::Simhasher(
                cfg.JiebaDictPath(),
                cfg.JiebaHmmPath(),
                cfg.JiebaIdfPath(),
                cfg.JiebaStopWordsPath());
    }
    ~SimHash(){delete _hash;}

public:
    uint64_t Hash(const string& content)const{
        uint64_t ret=0;
        _hash->make(content,_top_n,ret);
        return ret;
    }

private:
    simhash::Simhasher* _hash;
    int _top_n;
};

struct WebPage{
    int _id;
    string _title;
    string _link;
    string _content;
    string _url;
    static SimHash _hash;
    bool operator==(const WebPage& other) const{
        return _hash.Hash(_content)==_hash.Hash(other._content);
    }
    bool operator < (const WebPage& other) const{
        return _hash.Hash(_content)<_hash.Hash(other._content);
    }
};


#endif //TEST_WEBPAGE_HPP
