# Search-Engine
# 说明
这是一个基于后端的搜索引擎，可以实现关键字检索和网页检索等功能。由于没有做前端，所以该项目只能在Linux下编译运行，先编译sever.o(服务器端)，再编译client.o(客户端)，接下来选择菜单功能即可进行检索。
主要技术有：
1、高并发的服务器与客户端交互
2、网络编程
3、jieba分词
4、最小距离推荐算法
5、redis实现缓存
## 项目开发环境
+ Linux:Ubuntu18.04
+ G++:Version 4.8.4
+ Vim:Version 8.0

## 目录说明
+ **src**/:存放系统的源文件(*.cpp/*.cc)
+ **Include**/:存放系统的头文件(*.hpp/\*.hh/\*.h)
+ **bin**/:存放系统的可执行程序、用于测试程序的文件
+ **conf**/offline.json:记录系统程序中所需的相关配置文件的路径的json文件
+ **data**/dict.dat:存放词典
+ **data**/dictIndex.dat:存放单词所在位置的索引库
+ **data**/newripepage.dat:存放网页库
+ **data**/newoffset.dat:存放网页的偏移库
+ **data**/invertIndex.dat:存放倒排索引库
+ **log**/:存放日志文件
+ **lib**/cppjieba:存放jieba库的头文件，用于中文分词
+ **lib**/hiredis:存放redis接口的头文件
+ **lib**/limonp:lib/simhash需要用到的文件
+ **lib**/nlohmann:存放json开源库的头文件
+ **lib**/simhash:存放Google的simhash库的头文件，用于网页去重
+ **resource**/art-cn:需要解析为中文关键字的中文文章
+ **resource/art-en**:需要解析为英文关键字的英文文章
+ **resource**/stop_words_eng.txt:英文停用词
+ **resource**/stop_words_zh.txt:中文停用词
+ **resource**/xml:本地下载下来建立网页库的xml文件
