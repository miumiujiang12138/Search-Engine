//
// Created by Lenovo on 2022/11/27.
//
#include "../include/KeywordRecommender.h"
#include "../include/utils.hpp"

#include <fstream>
#include <algorithm>
#include <queue>
#include <iostream>

using std::ifstream;
using std::set_intersection;
using std::inserter;
using std::priority_queue;

void KeywordRecommender::InitIndex(const string& path) {
    ifstream ifs(path);
    if(!ifs)return;
    string word;
    int line;
    while(true){
        ifs>>word;
        while(ifs>>line)_index[word].insert(line);
        if(ifs.eof())break;
        ifs.clear();
    }
    ifs.close();
}

void KeywordRecommender::InitDict(const string& path) {
    ifstream ifs(path);
    if(!ifs)return;
    string word;
    int line;
    int tmp=1;
    while(ifs>>word>>line){
        _dict[tmp++]=std::make_pair(word,line);
    }
}

vector<string> KeywordRecommender::Recommend(const string &str,int max) {
    priority_queue<Result,vector<Result>> res;
    for(auto& tmp:Query(str)){
        res.emplace(tmp.first,tmp.second,Utils::Utf8MED(str,tmp.first));
    }
    vector<string> ans;
    for(int i=0;i<max;++i){
        if(res.empty())break;
        ans.push_back(res.top().word);
        res.pop();
    }
    return ans;
}

vector<pair<string,int>> KeywordRecommender::Query(const string &str) {
    int idx=0;
    set<int> tmp1;
    set<int> tmp2;
    while(idx<str.size()){
        int n = Utils::Utf8Bytes(str[0]);
        string sub=str.substr(idx,n);
        if(tmp1.empty())tmp1=_index[sub];
        else {
            set_intersection(_index[sub].begin(),_index[sub].end(),tmp1.begin(),tmp1.end(), inserter(tmp2,tmp2.begin()));
            tmp1=std::move(tmp2);
        }
        idx+=n;
    }
    vector<pair<string,int>> res;
    for(auto& it:tmp1){
        res.push_back(_dict[it+1]);
    }
    return res;
}
vector<string> KeywordRecommender::RedisRecommend(const string& str, int max)
{
    priority_queue<Result,vector<Result>> res;
    for(auto& tmp:RedisQuery(str)){
        res.emplace(tmp.first,tmp.second,Utils::Utf8MED(str,tmp.first));
    }
    vector<string> ans;
    for(int i=0;i<max;++i){
        if(res.empty())break;
        ans.push_back(res.top().word);
        res.pop();
    }
    return ans;
}

vector<pair<string,int>> KeywordRecommender::RedisQuery(const string& str)
{
    //切换到数据库1
    _redis.select(string("1"));

    int idx = 0;
    set<int> tmp1;
    set<int> tmp2;
    string words;
    while(idx < str.size()){
        //拿到字符串里面的第一个字符
        int n = Utils::Utf8Bytes(str[0]);
        string sub=str.substr(idx,n);

        //判断该字符在不在索引库中
        if(!_redis.get(sub).empty())
        {
            words += sub;
            words += " ";
        }
        idx+=n;
    }
    //对找到的所有键值取交集
    vector<string> result = _redis.sinter(words, "");

    string keyword;
    vector<pair<string, int>> res;
    string dict;
    int frequency;
    for(auto& elem : result)
    {
        dict.clear();
        frequency = 0;
        int l= std::stoi(elem)+1;
        keyword = _redis.get(std::to_string(l));
        size_t in = keyword.find('_');
        dict = keyword.substr(0, in);
        frequency = std::stoi(keyword.substr(in+1));
        res.push_back(std::make_pair(dict, frequency));
    }
    return res;
}

//将索引加载到Redis数据库中去
void KeywordRecommender::IndexRedis()
{
    //切换到一号数据库
    _redis.select(string("1"));

    //首先看数据库里面是不是有数据，如果有就不用加载
    string test("IS_OK_1");
    string ret2 = _redis.get(test);
    if(ret2.empty())
    {
        //先把标志位加进去
        //加载索引到Redis数据库
        for(auto& elem : _index)
        {
            for(auto it = elem.second.begin(); it != elem.second.end(); ++it)
            {   
                _redis.sadd(elem.first, std::to_string(*it));
            }
        }
        _redis.set(test, string("1"));
        _index.clear();
    }
}

//将词典加载到Redis数据库中
void KeywordRecommender::DictRedis()
{
    //切换到一号数据库
    _redis.select(string("1"));

    //首先看数据库里面是不是有数据，如果有就不用加载
    string test("IS_OK_0");
    string ret2 = _redis.get(test);
    if(ret2.empty())
    {
        //先把标志位加进去
        //加载词典到redis数据库
        string value;
        for(auto& elem : _dict)
        {
            value.clear();
            value = elem.second.first;
            value += '_';
            value += std::to_string(elem.second.second);

            _redis.set(std::to_string(elem.first), value);
        }
        _redis.set(test, string("1"));
        _dict.clear();
    }
}

void KeywordRecommender::RedsiConnect()
{
    //先连接数据库
    if(!_redis.connect(REDIS_IP, REDIS_PORT))
    {
        perror("redis connect error");
        exit(-1);
    }
}

#if 0
int main(){
    KeywordRecommender rec;
    rec.InitIndex("../data/index-cn.dat");
    rec.InitDict("../data/dict-cn.dat");
    while(true){
        string str1;
        std::getline(std::cin,str1);
        auto res=rec.Recommend(str1);
        for(auto& str:res){
            std::cout<<str<<std::endl;
        }
    }
    return 0;
}
#endif 
