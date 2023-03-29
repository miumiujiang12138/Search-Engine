//
// Created by Lenovo on 2022/11/28.
//

#ifndef TEST_PAGELIBPREPROCESSOR_H
#define TEST_PAGELIBPREPROCESSOR_H

#include "../include/PageLib.h"

using std::unordered_map;
using std::set;
using std::string;
using std::pair;
using std::vector;
using std::map;

class PageLibPreprocessor{
public:
    void BuildPage(){
        auto& cfg=OfflineConfig::getInstance();
        _page_lib.CreatePageLib();
        _page_lib.CreateOffsetLib();
        _page_lib.Store(cfg.PageLibPath(),cfg.PageOffsetPath());
    }
    void CutRedundantPages();//网页去重
    void BuildInvertIndexTable();//建立倒排索引
    void Store();//保存去重后的网页库以及倒排索引
private:
    PageLib _page_lib;
    //set<WebPage> _page_set;
    //unordered_map<int, pair<int,int>> _offset_lib;
    unordered_map<string, vector<pair<int,double>>> _invert_index_table;//word  doc_id  weight
};

#endif //TEST_PAGELIBPREPROCESSOR_H
