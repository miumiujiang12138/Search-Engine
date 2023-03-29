//
// Created by Lenovo on 2022/11/25.
//

#ifndef TEST_SPLITTOOL_HPP
#define TEST_SPLITTOOL_HPP

#include "OfflineConfig.hpp"

#include "../lib/cppjieba/Jieba.hpp"

using namespace cppjieba;

class SplitTool{
public:
    SplitTool(){
        auto& cfg = OfflineConfig::getInstance();
        _jieba=new Jieba(
                cfg.JiebaDictPath(),
                cfg.JiebaHmmPath(),
                cfg.JiebaUserDictPath());
        /*
         *                 cfg.JiebaIdfPath(),
                cfg.JiebaStopWordsPath()
         */
    }
    ~SplitTool(){delete _jieba;}
    vector<string> Cut(const string& content){
        vector<string> ret;
        _jieba->CutAll(content,ret);
        return std::move(ret);
    }
private:
    Jieba* _jieba;
};

#endif //TEST_SPLITTOOL_HPP
