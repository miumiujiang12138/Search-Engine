#ifndef _CTL_DICTIONARIES_HPP_
#define _CTL_DICTIONARIES_HPP_

#include "../include/EditDistance.hpp"
#include "../include/OfflineConfig.hpp"
#include "../include/redis.hpp"
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>

#define OFFLINE_JSON "../conf/offline.json"
#define DICT_ENG_DAT "../data/dict-Eng.dat"
#define DICTINDEX_ENG_DAT "../data/dictIndex-Eng.dat"

using std::map;
using std::set;
using std::unordered_map;
using std::unordered_set;
using std::string;
using std::vector;
using std::cerr;
using std::endl;
using std::pair;

using filename_type =string;
using allfiles_type=vector<string>;

using word_type = string; //单词类型
using frequecy_type = int;//频率类型
using edit_distance_type=int;//最小编辑距离

using index_type = set<int>;

using dictionaries_stop_map = unordered_set < word_type >; 
using dictionaries_map = map< word_type , frequecy_type > ;
using dictionaries_index_map = map <char,index_type>;
using dictionaries_use_map = unordered_map <int,pair<word_type,frequecy_type> >;

using candidate_words = vector< pair<word_type,frequecy_type> >;
using recommend_words = vector< word_type >;


class dictionaries
{
public:
    dictionaries(){}
    dictionaries(allfiles_type,filename_type);
    
    candidate_words CandidateWords(word_type&);
    candidate_words CandidateWordsRedis(word_type&);
    recommend_words RecommendWords(word_type&,size_t num);


    void LoadFiles();//加载文件
    void CreatStopMap();//创建提用词的哈希表
    void CreateMap();//创建去重的词典库
    void CreateUseMap();//下标为主键的词典库
    void CreateIndexMap();//创建索引库
    void WriteMap();//序列化词典库
    void ReleaseMap(){
        _stop_map.clear();
        _map.clear();
        _index_map.clear();
        _use_map.clear();
        _files.clear();
        _stop_file.clear();
    }
    void LoadMap();//加载词典库
    void LoadRedis();//加载redis
    void BuildRedis();//给redis一个一个插入键值对
    ~dictionaries() {}

private:
    allfiles_type _files;
    filename_type _stop_file;
    dictionaries_stop_map _stop_map;
    dictionaries_map _map;
    dictionaries_index_map _index_map;
    dictionaries_use_map _use_map;
    redis _redis;

};

class MyWord
{
private:
    string _keyword;
public:
    string _word;
private:
    frequecy_type _frequecy;
    edit_distance_type _edit_distance;
public:
    MyWord(word_type & keyword,word_type &word,frequecy_type frequecy);
    ~MyWord(){}
    bool operator>(const MyWord &rhs)const{
        if(this->_edit_distance<rhs._edit_distance)//编辑距离小的优先
        {return true;}
        else if(this->_edit_distance==rhs._edit_distance)//编辑距离相同的，词频优先
        {
            if(this->_frequecy>rhs._frequecy)
            {return true;}
            else if(this->_frequecy==rhs._frequecy)//词频相同，首字母优先
            {return this->_word<rhs._word;}
            else{return false;}
        }else{
            return false;
        }
    }
    bool operator==(const MyWord &rhs)const{
        if(this->_edit_distance==rhs._edit_distance &&
           this->_frequecy==rhs._frequecy && 
           this->_word==rhs._word)
        {
            return true;
        }else {return false;}
    }
    bool operator<(const MyWord &rhs)const
    {
        return !((*this)>=rhs);
    }
    bool operator>=(const MyWord &rhs)const{
        return (*this)>rhs||(*this)==rhs;
    }
    bool operator<=(const MyWord &rhs)const{
        return (*this)<rhs||(*this)==rhs;
    }
    bool operator!=(const MyWord &rhs)const{
        return !((*this)==rhs);
    }
};

#endif
