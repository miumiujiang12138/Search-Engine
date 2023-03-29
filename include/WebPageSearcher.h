//
// Created by Lenovo on 2022/11/30.
//

#ifndef TEST_WEBPAGESEARCHER_H
#define TEST_WEBPAGESEARCHER_H

#include "../include/SplitTool.hpp"
#include "../include/utils.hpp"

#include <math.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

using std::shared_ptr;
using std::istringstream;
using std::ifstream;
using std::vector;
using std::unordered_map;
using std::cout;
using std::string;
using std::endl;
using std::cin;
using std::pair;
using std::cerr;


struct WebPageQueryResult{
    vector<string> keywords;
    vector<unordered_map<string, string>> webpages;
};

class WebPageSearcher{
public:
    WebPageSearcher():_split(new SplitTool){
        auto& cfg=OfflineConfig::getInstance();
        stop_list=Utils::GetStopList(cfg.StopWordsCn());
    }
    ~WebPageSearcher(){delete _split;}
    shared_ptr<WebPageQueryResult> DoQuery(const string& search);                    //做查询
    void CalculateCosineSimilarity();                    //计算余弦相似度
    //这里我改了一下返回值
    //返回false代表没有网页符合查询结果
    bool BuildWordsWeight(const string& InvertedIndexLib);       //根据倒排索引库，找出拥有关键字的文档，以及关键字及权重
private:

    set<string>stop_list;

    //分词工具
    SplitTool* _split;

    //用来存放网页查询的关键字
    vector<string> _keyWords;

    //用来存放网页查询语句的关键字的权重
    //里面的数据分别对应：关键字，出现的次数，权重系数
    unordered_map<string, pair<int, double>> _keyWordsWeight;

    //用来存放文档里面的对应关键的权重
    //里面的数据分别对应：文档ID，关键字，关键字的权重系数
    unordered_map<int, unordered_map<string, double>> _wordsWeight;

    //用来存放计算的余弦相似度的结果
    //里面的数据对应：文档ID，余弦相似度
    unordered_map<int, double> _result;

    //倒排索引表
    unordered_map <string, vector<pair<int, double>>> _invert_index_table;
private:
    void GetQueryWordsWeightVector(vector<string> &words);//获取查询词的权重
    void ExecuteQuery(shared_ptr<WebPageQueryResult>& res);//执行网页查询
    vector<string> GetAbstract(const std::string& content);//获取摘要信息
};



#endif //TEST_WEBPAGESEARCHER_H
