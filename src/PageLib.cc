//
// Created by Lenovo on 2022/11/28.
//
#include "../include/PageLib.h"
#include "../include/tinyxml2.h"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include <regex>
#include <fstream>

using namespace tinyxml2;
using std::regex;
using std::endl;
using std::cout;
using std::cerr;
using std::ofstream;
using std::make_shared;


void PageLib::CreatePageLib(){
    GetFiles();
    _pageLib.reserve(4315);
    int m = 1;
    char* buf = new char[MAXPATHLEN + 1]();
    //cout << "Enter" << endl;
    XMLDocument xml; //1.声明xml类，用来读取xml文件
    for(int i = 0; i < _files.size(); ++i){
        xml.LoadFile(_files[i].c_str());//2.循环读取xml文件
        XMLElement *category = xml.RootElement();//3.拿到xml文件的根节点Rss
        XMLElement *channel = category->FirstChildElement("channel");//4.拿到channel结点指针
        XMLElement *title = channel->FirstChildElement("title");
        XMLElement *item = channel->FirstChildElement("item");//5.拿到item结点
        while(item){//6.设置循环，遍历channel下的每个item结点
            //cout << "Enter" << endl;
            auto rss_item=make_shared<WebPage>();//7.把数据保存到WebPage自定义类

            regex reg("<[^>]*>"); //8.正则表达式去除网页html标签

            //9.获取title结点指针，把内容保存到结构体相应的变量
            XMLElement *item_title = item->FirstChildElement("title");//title结点
            rss_item->_title = item_title->GetText(); //保存结点的内容

            //10.获取link结点指针，把内容保存到结构体相应的变量
            XMLElement *item_link = item->FirstChildElement("link");
            rss_item->_link = item_link->GetText();
            //cout<< "Enter" <<endl;

            //11.获取desc结点指针，数据用正则去除html标签后，把内容保存到结构体相应的变量
            XMLElement *item_description = item->FirstChildElement("description");
            if(item_description==nullptr){
                item = item->NextSiblingElement();
                continue;
            }
            rss_item->_content = item_description->GetText();

            rss_item->_content = regex_replace(rss_item->_content, reg, "");//去除html标签i
            //cout << "Enter" << endl;
            rss_item->_content=rss_item->_content.substr(0,rss_item->_content.find('\n'));
            //cout << "Enter" << endl;
            //12.设置每篇文章的id
            rss_item->_id = m++;
            //13.得到每个文档的绝对路径
            ::realpath(_files[i].c_str(), buf);
            string cwd2(buf);
            rss_item->_url = cwd2;
            //14.若标题为空，选取文章的第一行内容为标题
            if(rss_item->_title.empty()){
                string search("。");
                auto ret = rss_item->_content.find_first_of(search);
                if(ret == string::npos){
                    perror("title of text");
                    exit(-1);
                }
                string str_title = rss_item->_content.substr(0, ret+1);
                //去除标题里面的空格
                while(true){
                    size_t site = str_title.find(' ');
                    if(site == string::npos)
                    {
                        break;
                    }
                    str_title.erase(site, site + 1);
                }
                rss_item->_title = str_title;
            }
            //15.把每个结构体保存进 vector,将vector里面的每个结构体保存为相应的文章
            _pageLib.push_back(rss_item);

            item = item->NextSiblingElement();//找下一个item结点
        }
    }
    delete[] buf;
}

void PageLib::GetFiles() {
    auto& cfg=OfflineConfig::getInstance();
    _files=cfg.GetXMLFiles();
}

void PageLib::CreateOffsetLib(){
    for(int i = 0; i < _pageLib.size(); ++i){
        int num = _pageLib[i]->_id;
        _offsetLib[num] = std::make_pair(5*(i+1)-4, 5*(i+1)-1);
    }
}

void PageLib::Store(const string& ripepage, const string& offset){
    //磁盘换网页库
    //ofstream ofs(ripepage);    //这里是网页库page.dat的相对路径
    int fd=open(ripepage.c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fd==-1){
        perror("open");
        return;
    }
    string large_str;

    for (int i = 0; i < _pageLib.size(); i++){
        large_str.append("<docid>").append(std::to_string(_pageLib[i]->_id)).append("</docid>\n\t");
        large_str.append("<title>").append(_pageLib[i]->_title).append("</title>\n\t");
        large_str.append("<url>").append(_pageLib[i]->_link).append("</url>\n\t");
        large_str.append("<content>").append(_pageLib[i]->_content).append("</content>\n\t\n");
        /*ofs << "<docid>" << _pageLib[i]->_id<< "</docid>"
            << "\n"
            << "\t"
            << "<title>" << _pageLib[i]->_title << "</title>"
            << "\n"
            << "\t"
            << "<url>" << _pageLib[i]->_link << "</url>"
            << "\n" //注意治理读取的是link,不过无所谓，最后都会变成文件的绝对路径
            << "\t"
            << "<content>" << _pageLib[i]->_content << "</content>"
            << "\n"
            << "\t"
            << endl;*/
    }
    //使用mmap 零拷贝技术 减少大文件io耗时
    auto file_size=large_str.size();
    ftruncate(fd,file_size);
    size_t length=1024*4*256*10;//10兆
    off_t f_size=0;
    int cp_size=0;
    while(f_size<file_size){
        if(file_size-f_size>=length){
            cp_size=length;
        }
        else cp_size=file_size-f_size;
        char *dest=(char*)mmap(nullptr, cp_size, PROT_WRITE, MAP_SHARED, fd, f_size);
        if(dest==MAP_FAILED){
            perror("mmap");
            return;
        }
        memcpy(dest,large_str.c_str()+f_size,cp_size);
        munmap(dest,cp_size);
        f_size+=cp_size;
    }
    close(fd);
    ofstream ofs;
    //磁盘换索引库
    ofs.open(offset);      //这里是offset.dat的相对路径

    if(!ofs.good()){
        cerr << "ofstream2 is not good!" << endl;
        return;
    }

    for(auto it=_offsetLib.begin();it!=_offsetLib.end();++it){
        ofs << it->first << "  " << it->second.first << "  " << it->second.second
            << "\n"
            << "\t"
            << endl;
    }
    ofs.close();
}
