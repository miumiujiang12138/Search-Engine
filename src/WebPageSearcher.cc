//
// Created by Lenovo on 2022/11/30.
//
#include "../include/WebPageSearcher.h"
#include "../include/Mylogger.hpp"

#include <regex>


void WebPageSearcher::CalculateCosineSimilarity(){
    //计算余弦相似度
    //首先获取相对应的文章的个数，确定循环次数
    auto ptr = _wordsWeight.begin();
    //用来存放X*Y的结果
    double num1 = 0;
    //用来存放|x| * |y|的结果
    double num2 = 0;
    //有多少篇文章符合查询结果，就循环多少次
    _result.clear();
    for(size_t i = 0; i < _wordsWeight.size(); ++i){
        num1 = 0;
        num2 = 0;
        for(size_t j = 0; j < _keyWords.size(); ++j){
            //分别获取查询语句和文档的关键字的权重系数
            double x = ptr->second[_keyWords[j]];
            double y = _keyWordsWeight[_keyWords[j]].second;
            num1 += x * y;
        }

        //接下来计算|x| * |y| 的结果
        double x = 0;
        double y = 0;
        for(size_t j = 0; j < _keyWords.size(); ++j){
            x += ::powf64(_keyWordsWeight[_keyWords[j]].second, 2);
            y += ::powf64(ptr->second[_keyWords[j]], 2);
        }
        x = ::powf64(x, 0.5);
        y = ::powf64(x, 0.5);
        num2 = x * y;

        //将结果放入结果集
        _result[ptr->first] = num1 / num2;
        ++ptr;
    }
}

//根据倒排索引表生成文档相关的关键字及权重
//传入参数是倒排索引表的相对路径
bool WebPageSearcher::BuildWordsWeight(const string& InvertedIndexLib){
    //加载倒排索引库进入内存
    //打开索引库文件
    ifstream ofs(InvertedIndexLib, std::ios::in);

    if(!ofs.good()){
        cerr << "ofstream1 is not good!" << endl;
        return false;
    }

    string line;
    int id;                         //文档的ID
    string keyWords;                //关键词
    double idf;                     //权重值
    while(getline(ofs, line)){
        istringstream iss(line);
        iss >> keyWords;
        vector<pair<int, double>> wordIndexVector;
        while(iss >> id >> idf)
        {
            wordIndexVector.emplace_back(id, idf);
        }
        _invert_index_table[keyWords] = std::move(wordIndexVector);
    }

    //根据倒排索引库生成对应_wordsWeight
    //首先在倒排索引库里面查找关键词
    //得到所有的文章ID和权重
    //用来存放查询结果
    unordered_map<int, vector<double>> preResult;
    for(auto &elem : _keyWords){
        for(auto &elem1 : _invert_index_table[elem]){
            preResult[elem1.first].push_back(elem1.second);
        }
    }

    //遍历查询结果，删除关键字个数不够的文章
    for(auto it = preResult.begin(); it != preResult.end();){
        if(it->second.size() < _keyWords.size()){
            it = preResult.erase(it);
        }
        else{
            ++it;
        }
    }
    //接下来结果集中就是全部的符合条件的文档ID
    //将结果插入到_result结果集
    _wordsWeight.clear();
    for(auto it = preResult.begin(); it != preResult.end(); ++it){
        for(size_t i = 0; i < _keyWords.size(); ++i){
            _wordsWeight[it->first][_keyWords[i]] = it->second[i];
        }
    }

    //判断返回值
    if(!preResult.empty()){
        return true;
    }
    else{
        return false;
    }
}

shared_ptr<WebPageQueryResult> WebPageSearcher::DoQuery(const string &search) {
    if(search.empty()){
        return make_shared<WebPageQueryResult>();
    }
    auto words=_split->Cut(search);
    auto& cfg=OfflineConfig::getInstance();
    words.erase(std::remove_if(words.begin(), words.end(),[this](const string& word){
        return word==" " || stop_list.find(word) != stop_list.end();
    }),words.end());
    GetQueryWordsWeightVector(words);
    _keyWords=std::move(words);
    bool flag=BuildWordsWeight(cfg.InvertIndex());
    if(!flag)return make_shared<WebPageQueryResult>();
    CalculateCosineSimilarity();
    shared_ptr<WebPageQueryResult> res=make_shared<WebPageQueryResult>();
    res->keywords=_keyWords;
    ExecuteQuery(res);
    using type_cmp = unordered_map<string, string>;
    sort(res->webpages.begin(),res->webpages.end(),[this](type_cmp& lhs,type_cmp& rhs){
        auto l= stoi(lhs["id"]);
        auto r=stoi(rhs["id"]);
        return _result[l]>_result[r];
    });
    return res;
}

void WebPageSearcher::GetQueryWordsWeightVector(vector<string> &words) {
    vector<double> weights;
    auto doc_num=words.size();
    unordered_map<string,int> words_tf;
    unordered_map<string,double> words_idf;
    //计算DF
    for(auto& str:words){
        auto df=_invert_index_table[str].size();
        double idf=log2((double)doc_num/(df+1));
        words_idf[str]=idf;
        ++words_tf[str];
    }
    //去重
    auto cut=unique(words.begin(),words.end());
    words.erase(cut,words.end());
    //建立每个单词的权重
    double total=0.0;
    for(auto& word:words){
        auto w=words_tf[word]*words_idf[word];
        weights.push_back(w);
        total+=w*w;
    }
    int idx=0;
    //归一化处理
    for(auto& w:weights){
        w/=sqrt(total);
        _keyWordsWeight[words[idx]]=make_pair(words_tf[words[idx]],w);
        ++idx;
    }
}

void WebPageSearcher::ExecuteQuery(shared_ptr<WebPageQueryResult>& res) {
    auto &cfg = OfflineConfig::getInstance();
    ifstream ifs(cfg.NewPageOffsetPath());
    if (!ifs) {
        LogWarn("page offset file not found");
        return;
    }
    set<int> lines;
    int id, start, end;
    auto size = _result.size();
    while (ifs >> id >> start >> end) {
        if (size == 0)break;
        if (_result.find(id) == _result.end())continue;
        lines.insert(start);
        --size;
    }
    ifs.close();
    ifs.open(cfg.NewPageLibPath());
    if (!ifs) {
        LogWarn("page lib file not found");
        return;
    }
    string line;
    int num = 1;
    while (getline(ifs, line)) {
        if (line.empty())continue;
        if(lines.empty())break;
        if (lines.find(num)!= lines.end()) {
            unordered_map<string, string> web_map;
            web_map["id"]=Utils::ParseXMLLabel(line);
            getline(ifs, line);
            ++num;
            web_map["title"]=Utils::ParseXMLLabel(line);
            getline(ifs, line);
            ++num;
            web_map["url"]=Utils::ParseXMLLabel(line);
            getline(ifs, line);
            auto tmp=GetAbstract(Utils::ParseXMLLabel(line));
            string ab;
            for(auto& s: tmp){
                ab+=s;
                ab+="\n";
            }
            ab.pop_back();
            web_map["abstract"]=std::move(ab);
            res->webpages.emplace_back(std::move(web_map));
            ++num;
            //lines.erase(num);
        }
        ++num;
    }
}
static const wstring g_pattern1=L"([^.^。^ ^，^！]*(?=[^.^。^，^ ^！]*";
static const wstring g_pattern2=L")[^.^。^，^ ^！]*)";

vector<string> WebPageSearcher::GetAbstract(const string &content) {
    vector<string> res;
    for(auto& key:_keyWords){
        std::wregex pattern(g_pattern1+Utils::to_wide_string(key)+g_pattern2);
        std::wsmatch m;
        std::wstring tmp=Utils::to_wide_string(content);
        auto ret = std::regex_search(tmp,m,pattern);
        if(!ret)continue;
        int i=0;
        for(auto& str:m){
            if(i>=1)break;//限制每个分词最多1条句子
            res.push_back(Utils::to_byte_string(str));
            ++i;
        }
    }
    //去重
    res.erase(std::unique(res.begin(),res.end()),res.end());
    return res;
}
