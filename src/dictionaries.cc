#include "../include/dictionaries.hpp"
#include "../include/Mylogger.hpp"

dictionaries::dictionaries(allfiles_type files,filename_type stopfile)
:_files(files)
,_stop_file(stopfile)
,_stop_map()
,_map()
,_index_map()
,_use_map()
{
}


void dictionaries::CreatStopMap(){
    std::fstream fd;
    string buf;
    std::stringstream line;
    string words;
    words.reserve(10);
    
    fd.open(this->_stop_file,std::ios::in);
    if(!fd.is_open())
    {
        cerr<<this->_stop_file.c_str()<<": in fd.open fail "<<_stop_file<<endl;
        LogError(_stop_file.append(": in fd.open fail"));
        exit(1);
    }
    while(!fd.fail())
    {
        std::getline(fd,buf,fd.widen('\n'));
        for(auto &ch:buf)
        {
            line.put(ch);
        }
        //逐行读取单词
        while(line>>words)
        {
            //停用词没有重复
           auto ret= _stop_map.emplace(words);
           if(!ret.second)
           {
               cerr<<"stopfile words:"<<words<<" emplace fail"<<endl;
               LogError(string("stopfile words: ").append(words).append(" emplace fail"));
           }
            words.clear();
        }
        line.clear();
    }
    fd.close();
    //::printf("%s,file ok\n",this->_stop_file.c_str());
    LogInfo(_stop_file.append(", file ok"));
}


void dictionaries::CreateMap()
{
    std::fstream fd;
    string buf;
    std::stringstream line;
    string words;
    words.reserve(10);
    for(auto &rf:_files)
    {

        fd.open(rf,std::ios::in);
        if(!fd.is_open())
        {
            //printf("%s: in fd.open fail \n",rf.c_str());
            LogWarn(rf.append(": in fd.open fail"));
            continue;
        }
        while(!fd.fail())
        {
            std::getline(fd,buf,fd.widen('\n'));
            for(auto &ch:buf)
            {
                //只将数字、字母、空白加到line流
                if(std::isalnum(ch)||std::isblank(ch))
                {
                    //如果是字母，转为小写
                    if(std::isalpha(ch))
                    {
                        ch=std::tolower(ch);
                    }
                    line.put(ch);
                }
            }
            //逐行读取单词
            while(line>>words)
            {
                auto ret_stop=(this->_stop_map.find(words));
                if(ret_stop!=this->_stop_map.end())
                {
                    words.clear();
                    continue;
                }
                auto ret=(this->_map.find(words));
                if(ret==this->_map.end())
                {
                    this->_map.insert({words,1});
                }else{
                    ++(ret->second);
                }
                words.clear();
            }
            line.clear();
        }
        fd.close();
        //::printf("%s,file ok\n",rf.c_str());
        LogInfo(rf.append(", file ok\n"));
    }
    //::printf("all down\nstart create use map\n");
    LogInfo("all down\tstart create use map");

    CreateUseMap();
    CreateIndexMap();
}

void dictionaries::CreateUseMap()
{
    int i=0;
    for(auto &rm:_map)
    {
        _use_map.insert({i,{rm.first.c_str(),rm.second}});
        ++i;
    }
    //::printf("bulid %d word in use map\n",i);
    char log[30];
    sprintf(log, "bulid %d word in use map", i);
    LogInfo(log);
}

void dictionaries::CreateIndexMap()
{
    int i=0;
    for(auto &rm:_use_map)
    {
        string tmp=rm.second.first;
        for(auto &rs:tmp)
        {
            if(isalpha(rs))
            {
                auto ret=_index_map[rs].insert(rm.first);
                if(!ret.second)
                {
                }else{++i;}
            }
        }
    }
    //::printf("bulid %d index in index map\n",i);
    char log[30];
    sprintf(log, "bulid %d index in index map", i);
    LogInfo(log);
}

void dictionaries::WriteMap()
{
    std::fstream fd;  
    //写词典库
    fd.open(DICT_ENG_DAT,std::ios::out);
    if(!fd.is_open())
    {
        cerr<<DICT_ENG_DAT<<" open fail"<<endl;
        LogError(string(DICT_ENG_DAT).append(" open fail"));
        exit(1);
    }
    string buf;
    buf.reserve(20);
    for(auto &rm:_map)
    {
        if(fd.fail())
        {
            cerr<<"write to "<<DICT_ENG_DAT<<" fail"<<endl;
            LogError(string("write to ").append(DICT_ENG_DAT).append(" fail"));
        }
        buf=rm.first;
        buf+=" ";
        buf+=std::to_string(rm.second);
        buf+="\n";
        //写入格式："string int"
        //例如:"braving 1"
        fd.write(buf.c_str(),buf.size());
        buf.clear();
    }
    fd.close();
    //::printf("write %s down\n",DICT_ENG_DAT);
    LogInfo(string("write ").append(DICT_ENG_DAT).append(" down"));
    //写索引文件
    fd.open(DICTINDEX_ENG_DAT,std::ios::out);
    if(!fd.is_open())
    {
        cerr<<DICTINDEX_ENG_DAT<<" open fail"<<endl;
        LogError(string(DICTINDEX_ENG_DAT).append(" open fail"));
        exit(1);
    }
    for(auto &rm:_index_map)
    {
        if(fd.fail())
        {
            cerr<<"write to "<<DICTINDEX_ENG_DAT<<" fail"<<endl;
            LogError(string("write to ").append(DICTINDEX_ENG_DAT).append(" fail"));
        }
        buf=rm.first;
        buf+=" ";
        for(auto &rs:rm.second)
        {
            buf+=std::to_string(rs);
            buf+=" ";
        }
        buf+="\n";
        //写入格式："string int"
        //例如:"braving 1"
        fd.write(buf.c_str(),buf.size());
        buf.clear();
    }
    fd.close();
    //::printf("write %s down\n",DICTINDEX_ENG_DAT);
    LogInfo(string("write ").append(DICTINDEX_ENG_DAT).append(" down"));
}

void dictionaries::LoadMap()
{
    std::fstream fd;
    fd.open(DICT_ENG_DAT,std::ios::in);
    if(!fd.is_open())
    {
        fd.close();
        //::printf("not find file :%s\n",DICT_ENG_DAT);
        LogWarn(string("not find file :").append(DICT_ENG_DAT));
        this->LoadFiles();
        this->CreatStopMap();
        this->CreateMap();
        //::printf("start write\n");
        this->WriteMap();
        return;
    }
    std::stringstream line;
    string buf;
    buf.reserve(1024);
    word_type word;
    word.reserve(20);
    frequecy_type frequecy;
    int index=0;
    int counter_word=0;
    int counter_index=0;
    string tmp;
    while(!fd.fail())
    {
        std::getline(fd,buf,fd.widen('\n'));
        for(auto &ch:buf)
        {
            line.put(ch);
        }
        line>>word;
        line>>tmp;
        frequecy=std::stoi(tmp);
        /* ::printf("buf:%s |word:%s| tmp:%s |fre:%d\n",buf.c_str(),word.c_str(),tmp.c_str(),frequecy); */
        _use_map[index]={word,frequecy};
        ++counter_word;
        for(auto &rw:word)
        {
            if(isalpha(rw))
            {
               auto ret= _index_map[rw].insert(index);
               if(ret.first!=_index_map[rw].end())
               {++counter_index;}
            }
        }
        word.clear();
        buf.clear();
        line.clear();
        ++index;
    }
    fd.close();
    //printf("load %d word in use map\nload %d index in index map\n",counter_word,counter_index);
    char log[100];
    sprintf(log, "load %d word in use map\nload %d index in index map",counter_word,counter_index);
    LogInfo(log);
}

candidate_words dictionaries::CandidateWords(word_type& key_word)
{
    //::printf("start Recommend word :%s\n",key_word.c_str());
    LogInfo(string("start Recommend word :").append(key_word));
    candidate_words tmpWord;
    vector<index_type> _index_sets;
    _index_sets.reserve(key_word.size());
    for(auto & rs:key_word)
    {
        if(isalpha(rs)){
            /* ::printf("%c\n",rs); */
            rs=std::tolower(rs);
            index_type ret=_index_map.find(rs)->second;
            if(!ret.empty())
            {
                /* ::printf("not empty\n"); */
                _index_sets.push_back(ret);
            }
        }
    }
    // ::printf("get all set\n");
    index_type tmp_1;
    index_type tmp_2;
    for(size_t k=0;k<_index_sets.size();)
    {
        int right=(k+1)%2;
        
        if(tmp_1.empty()&&tmp_2.empty()){

            std::set_intersection(_index_sets[k].begin(),_index_sets[k].end(),
                              _index_sets[right].begin(),_index_sets[right].end(),
                              inserter(tmp_1,tmp_1.begin()));
            k+=2;
        }else if(tmp_2.empty()){
            std::set_intersection(tmp_1.begin(),tmp_1.end(),
                              _index_sets[k].begin(),_index_sets[k].end(),
                              inserter(tmp_2,tmp_2.begin()));
            ++k;
            tmp_1.clear();
        }else if(tmp_1.empty())
        {
            std::set_intersection(tmp_2.begin(),tmp_2.end(),
                              _index_sets[k].begin(),_index_sets[k].end(),
                              inserter(tmp_1,tmp_1.begin()));
            ++k;
            tmp_2.clear();
        }
    }
    if(!tmp_1.empty()){
        tmp_2=tmp_1;
    }
    tmpWord.reserve(tmp_2.size());
    for(auto& rs:tmp_2){
        tmpWord.push_back({_use_map[rs].first,_use_map[rs].second});
    }
    return tmpWord;

}

recommend_words dictionaries::RecommendWords(word_type& key_word,size_t num)
{
    //获取候选词
    // candidate_words tmpWord=CandidateWords(key_word);
    candidate_words tmpWord=CandidateWordsRedis(key_word);
    // ::printf("get all candidate words :%lu\n",tmpWord.size());
    vector<MyWord> count_words;
    recommend_words ret_words;
    if(tmpWord.size()==0)
    {
        return ret_words; 
    }
    count_words.reserve(tmpWord.size());
    for(auto &rc:tmpWord)
    {
        // std::cout<<rc.first<<" "<<rc.second<<endl;
        count_words.push_back(MyWord(key_word, rc.first, rc.second));
    }
    std::sort(count_words.begin(),count_words.end());
    //避免为空的情况
    if(count_words.size()==0){return ret_words;}
    
    for(int i =count_words.size()-1;i>=0;--i)
    {
        // std::cout<<count_words[i]._word<<endl;
        ret_words.push_back(count_words[i]._word);
        if(count_words.size()-i==num){break;}
    }
    return ret_words;
}

MyWord::MyWord(word_type &key_word,word_type &word,frequecy_type frequecy)
:_keyword(key_word)
,_word(word)
,_frequecy(frequecy)
,_edit_distance(EditDistance::editDistance(_keyword,word))
{}

void dictionaries::LoadFiles()
{
    OfflineConfig::getInstance().load(OFFLINE_JSON);
    auto &cfg=OfflineConfig::getInstance();
    //加载停用词文件名
    _stop_file=cfg.StopWordsEn();
    //加载英文语料库
    _files=cfg.getEnFiles();
}

void dictionaries::BuildRedis(){
    string  key;     
    string  member; 
    //插入索引表
    for(auto &rc:_index_map)
    {
        key.clear();
        key=rc.first;
        for(auto &rs:rc.second)
        {
            member.clear();
            member=std::to_string(rs);
            _redis.sadd(key,member);
        }
    }
    //插入词典表
    for(auto &rc:_use_map)
    {
        key.clear();
        key=std::to_string(rc.first);
        member.clear();
        member+=rc.second.first+" "+std::to_string(rc.second.second);
        _redis.set(key,member);
    }
    //::printf("BuildRedis down\n");
}

candidate_words dictionaries::CandidateWordsRedis(word_type & key_word)
{
    //::printf("start CandidateWordsRedis :%s\n",key_word.c_str());
    vector<string> keys;
    keys.reserve(key_word.size());
    string tmp;
    for(auto& rs:key_word)
    {
        if(::isalpha(static_cast<int>(rs)))
        {
            unsigned char l=::tolower(rs);
            tmp.clear();
            tmp+=l;
            keys.push_back(tmp);
        }
    }
    //利用redis 取交集
    string key;
    key.reserve(keys.size()*4);
    key.clear();
    for(auto & rs:keys)
    {
        key+=rs+" ";
    }
    // std::cout<<"all alpha:"<<key<<endl;
    vector<string> words_index=_redis.sinter(key,"");
    candidate_words tmpWord;
    word_type word_tmp="";
    frequecy_type frequecy;
    char buf[1024]={'\0'};
    // std::cout<<"_index.size()"<<words_index.size()<<endl;
    for(auto & rs:words_index)
    {
        tmp.clear();
        tmp=_redis.get(rs);
        // std::cout<<rs<<":"<<tmp<<endl;
        //tmp 里面是单词和词频连在一起的字符串
        //例如: "hello 13"
        word_tmp.clear();
        word_tmp.reserve(tmp.size());
        ::memset(buf,'\0',sizeof(buf));
        ::sscanf(tmp.c_str(),"%s %d",buf,&frequecy);
        word_tmp+=buf;
        // std::cout<<word_tmp<<endl;
        tmpWord.push_back({word_tmp,frequecy});
    }
    // ::printf("end of candi:%s \n",key_word.c_str());
    return tmpWord;
}


void dictionaries::LoadRedis()
{
    if(!_redis.connect(REDIS_IP,REDIS_PORT))
    {
        ::printf("connect redis server:%s %d faile\n",REDIS_IP,REDIS_PORT);
        ::exit(1);
    }
    // ::printf("start load redis\n");
    string tmp;
    tmp=_redis.get("IS_empty");
    if(tmp=="NO")
    {
        return;
    }
    else{
        //::printf("start load map\n");
        LoadMap();
        //::printf("start bulid redis\n");
        BuildRedis();
        //_redis.set("IS_empty","NO");
        ReleaseMap();
        _redis.set("IS_empty","NO");
        return;
    }
}


#if 0

int main(int argc,char const * argv[])
{

    if(argc!=3){
        ::perror("argc!=3\n");
        ::exit(1);
    }
    dictionaries dic;
    dic.LoadFiles();
    dic.LoadMap();
    string tmp(argv[1]);
    size_t num=std::stoi(argv[2]);
    recommend_words use_tmp=dic.RecommendWords(tmp,num);
        std::cout<<">>推荐: "<<tmp<<" "<<use_tmp.size()<<endl;
        int i=0;
        for(auto &rr:use_tmp)
        {
            std::cout<<">> "<<++i<<"  "<<rr<<endl;
        }
}

#endif



