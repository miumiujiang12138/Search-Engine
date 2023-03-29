//
// Created  on 2022/11/27.
//

#ifndef TEST_KEYWORDRECOMMENDER_H
#define TEST_KEYWORDRECOMMENDER_H

#include <string>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>
#include "../include/redis.hpp"

using std::istringstream;
using std::string;
using std::unordered_map;
using std::set;
using std::vector;
using std::pair;

struct Result{
    Result(string  str,int f,int m):word(std::move(str)),freq(f),med(m){}
    string word;
    int freq;
    int med;
    bool operator<(const Result& other) const{
        if(med==other.med)return freq>other.freq;
        else return med>other.med;
    }
};

class KeywordRecommender{
public:
    vector<string> Recommend(const string& str,int max=5);
    vector<string> RedisRecommend(const string& str, int max = 5);
    void InitIndex(const string& path);
    void InitDict(const string& path);
    //将词典和索引加载到Redis数据库中
    void IndexRedis();
    void DictRedis();

    //Redsi数据库连接
    void RedsiConnect();
private:
    vector<pair<string,int>> Query(const string& str);
    vector<pair<string,int>> RedisQuery(const string& str);
private:
    unordered_map<int,pair<string,int>> _dict;
    unordered_map<string,set<int>> _index;
    redis _redis;
};

#endif //TEST_KEYWORDRECOMMENDER_H
