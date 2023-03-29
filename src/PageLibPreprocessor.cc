//
// Created by Lenovo on 2022/11/28.
//

#include "../include/PageLibPreprocessor.h"
#include "../include/SplitTool.hpp"
#include "../include/utils.hpp"

#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <future>

inline bool operator==(const shared_ptr<WebPage>& lhs,const shared_ptr<WebPage>& rhs){
    return *lhs==*rhs;
}

inline bool operator<(const shared_ptr<WebPage>& lhs,const shared_ptr<WebPage>& rhs){
    return *lhs<*rhs;
}

void PageLibPreprocessor::CutRedundantPages() {
    //sort(_page_lib._pageLib.begin(),_page_lib._pageLib.end());
    //auto tmp=unique(_page_lib._pageLib.begin(),_page_lib._pageLib.end());
    //网页去重
    //_page_lib._pageLib.erase(tmp,_page_lib._pageLib.end());

    set<shared_ptr<WebPage>> tmp(_page_lib._pageLib.begin(),_page_lib._pageLib.end());
    _page_lib._pageLib=std::move(vector<shared_ptr<WebPage>>(tmp.begin(),tmp.end()));

    //重建索引
    _page_lib.CreateOffsetLib();
}

unordered_map<string,int> BuildDict(const string& content,SplitTool& split){
    unordered_map<string,int> dict;
    auto& cfg=OfflineConfig::getInstance();
    auto stop_list=Utils::GetStopList(cfg.StopWordsCn());
    vector<string> vec = split.Cut(content);
    for(auto& str:vec){
        if(str.size()<3)continue;
        if(str[0]==-17)continue;
        if(Utils::Utf8Bytes(str[0])<3)continue;
        auto it = stop_list.find(str);
        //如果不在停用词列表,词频++
        if(it==stop_list.end())++dict[str];
    }
    return dict;
}

unordered_map<int,unordered_map<string,int>> BuildAllDict(vector<shared_ptr<WebPage>>& page_set,SplitTool& split){
    unordered_map<int,unordered_map<string,int>> ret;
    for(auto& page:page_set){
        ret[page->_id]=BuildDict(page->_content,split);
    }
    return ret;
}

void PageLibPreprocessor::BuildInvertIndexTable() {
    unordered_map<string,int> doc_freq;
    unordered_map<int,vector<pair<string,double>>> weight_map;
    SplitTool split;
    auto doc_num=_page_lib._pageLib.size();
    auto dict_map= BuildAllDict(_page_lib._pageLib,split);
    //计算DF
    for(auto& page:_page_lib._pageLib){
        unordered_map<string,int> freq=dict_map[page->_id];
        for(auto& it:freq){
            ++doc_freq[it.first];
        }
    }
    //建立每个词的权重map
    int id=1;
    for(auto& page:_page_lib._pageLib){
        int doc_id=page->_id;
        for(auto& it:dict_map[doc_id]){
            string word=it.first;
            double tf=it.second;
            double idf=log2((double)doc_num/(doc_freq.at(word)+1));
            double weight=tf*idf;
            weight_map[doc_id].emplace_back(std::make_pair(word,weight));
        }
    }
    //归一化处理
    //unordered_map<int,vector<pair<string,double>>>  doc_id  word  weight
    for(auto& it:weight_map){
        double weight=0.0;
        for(auto& freq:it.second){
            weight+= freq.second * freq.second;
        }
        weight=sqrt(weight);
        for(auto& freq:it.second){
            //freq.second=freq.second/weight;
            //word  doc_id  weight
            double real_weight=freq.second/weight;
            _invert_index_table[freq.first].emplace_back(std::make_pair(it.first, real_weight));
        }
    }
}

void PageLibPreprocessor::Store() {
    auto& cfg=OfflineConfig::getInstance();
    //异步调用网页库保存
    auto page_store=std::async(std::launch::async,[&cfg, this](){
        _page_lib.Store(cfg.NewPageLibPath(),cfg.NewPageOffsetPath());
    });
    int fd=open(cfg.InvertIndex().c_str(),O_RDWR|O_CREAT|O_TRUNC,0666);
    if(fd==-1){
        std::cout<<"open failed"<<std::endl;
        return;
    }
#if 0
    ofstream ofs(cfg.InvertIndex(),std::ios::out|std::ios::trunc);
    if(!ofs){
        //todo error handler
        return;
    }
#endif
    string large_str;
    for(auto & iter : _invert_index_table){
        large_str.append(iter.first).append(" ");
        for(auto& ele:iter.second){
            large_str.append(std::to_string(ele.first)).append(" ");
            large_str.append(std::to_string(ele.second)).append(" ");
        }
        large_str.append("\n");
    }
#if 0
    for(auto& ele:_invert_index_table){
        ofs<<ele.first<<" ";
        for(auto& it:ele.second){
            ofs<<it.first<<" "<<it.second<<" ";
        }
        ofs<<endl;
    }
    ofs.close();
#endif
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
    page_store.get();
}

#if 0
int main(){
    PageLibPreprocessor pp;
    pp.BuildPage();
    pp.CutRedundantPages();
    pp.BuildInvertIndexTable();
    pp.Store();
    return 0;
}
#endif
