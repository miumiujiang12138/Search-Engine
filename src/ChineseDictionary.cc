//
// Created by Lenovo on 2022/11/25.
//

#include "../include/ChineseDictionary.h"
#include "../include/SplitTool.hpp"
#include "../include/Mylogger.hpp"

set<string> GetStopList(const std::string& path){
    set<string> ret;
    ifstream ifs(path);
    if(!ifs){
        //todo error handler
        return {};
    }
    string line;
    while(getline(ifs,line)){
        if(line.empty())continue;
        ret.insert(line);
    }
    ifs.close();
    return ret;
}

bool IsUtf8(char ch){
    int n=0;
    for(int i=0;i<6;++i){
        if(ch&(1<<(7-i)))++n;
        else break;
    }
    return n>=3;
}

void ChineseDictionary::BuildCnDict() {
    auto& cfg = OfflineConfig::getInstance();
    //停用词列表
    set<string> stop_list=GetStopList(cfg.StopWordsCn());
    //遍历中文语料文件
    for(auto& file:cfg.GetCnFiles()){
        ifstream ifs(file);
        //读整篇文章到content里
        string content(istreambuf_iterator<char>{ifs},istreambuf_iterator<char>{});
        //jieba 分词
        vector<string> vec = _split_tool->Cut(content);
        for(auto& str:vec){
            if(str.size()<3)continue;
            if(str[0]==-17)continue;
            if(!IsUtf8(str[0]))continue;
            auto it = stop_list.find(str);
            //如果不在停用词列表,词频++
            if(it==stop_list.end())++_dict[str];
        }
        ifs.close();
    }
}

void ChineseDictionary::StoreDict() {
    auto& cfg = OfflineConfig::getInstance();
    ofstream ofs(cfg.DictOutPath(),std::ios::trunc|std::ios::out);
    if(!ofs){
        //todo error handler
        return;
    }
    for(auto& it:_dict){
        ofs<<it.first<<" "<<it.second<<"\n";
    }
}

//实现索引表的建立
void ChineseDictionary::BuildCnIndex(){
    //首先读取词典表里的词语，获取汉字
    int row = 0;
    for(auto& elem : _dict){
        //获取词语
        const string& words = elem.first;
        //获取string的字符数
        int size = words.size();
        //分离单个汉字
        size_t pos = 0;
        int out = 3;
        while(pos < size){
            string word = words.substr(pos,out);
            //若汉字不存在，就创建对应的汉字，加入行号
            //若汉字存在，就直接加入行号
            _index[word].insert(row);
            //依次判断下一个汉字
            pos += out;
        }
        ++row;
    }
}

//磁盘化索引表
void ChineseDictionary::StoreIndex(){
    //首先打开对应的文件
    auto& cfg = OfflineConfig::getInstance();
    auto filename = cfg.IndexOutPath();
    std::ofstream ofs(filename, std::ios::out|std::ios::trunc);
    if(!ofs.is_open()){
        ::perror("open file failed\n");
        LogError("open file failed.\n");
        ::exit(-1);
    }
     //写文件
    string word;
    word.reserve(1024);
    for(auto& elem : _index){
        word = elem.first;
        word += " ";
        for(auto &rs : elem.second){
            word += std::to_string(rs);
            word += " ";
        }
        word += "\n";
        if(ofs.good()){
            ofs << word;
            word.clear();
        }
        else{
            ::perror("ofs");
            ::exit(-1);
        }
    }
    ofs.close();
    LogInfo(filename.append(" down\n"));    
    //::printf("%s down\n", filename.c_str());
}
