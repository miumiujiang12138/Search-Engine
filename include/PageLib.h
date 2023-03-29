//
// Created by Lenovo on 2022/11/28.
//

#ifndef TEST_PAGELIB_H
#define TEST_PAGELIB_H

#include "../include/WebPage.hpp"

#include <vector>
#include <string>
#include <unordered_map>

using std::vector;
using std::unordered_map;
using std::string;
using std::pair;
using std::shared_ptr;


class PageLib
{
public:
    vector<string> _files;     //语料库文件的集合
    vector<shared_ptr<WebPage>> _pageLib;   //文档的集合，这里我想把下标加一的值当作文档的标号，方便读取
    unordered_map<int, pair<int, int>> _offsetLib;   //预处理前的偏移库, 分别对应文档ID，文档的开始行号和结束行号
public:
    void CreatePageLib();      //用来创造网页库
    void CreateOffsetLib();    //用来创造偏移表
    void Store(const string& page_path,const string& offset_path);              //用来磁盘化
    void GetFiles();  //用来得到语料库文件

};

#endif //TEST_PAGELIB_H
