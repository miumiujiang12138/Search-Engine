//
// Created by Lenovo on 2022/11/25.
//

#ifndef TEST_CHINESEDICTIONARY_H
#define TEST_CHINESEDICTIONARY_H

#include <vector>
#include <string>
#include <unordered_map>
#include <set>

using std::vector;
using std::unordered_map;
using std::string;
using std::pair;
using std::set;

class SplitTool;

class ChineseDictionary{
public:
    explicit ChineseDictionary(SplitTool* split):_split_tool(split){}
    void BuildCnIndex();
    void BuildCnDict();
    void StoreIndex();
    void StoreDict();
private:
    vector<string> _files;
    SplitTool* _split_tool;
    unordered_map<string,int> _dict;
    unordered_map<string,set<int>> _index;
};

#endif //TEST_CHINESEDICTIONARY_H
