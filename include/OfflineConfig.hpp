//
// Created by Lenovo on 2022/11/25.
//

#ifndef TEST_OFFLINECONFIG_HPP
#define TEST_OFFLINECONFIG_HPP

#include <dirent.h>
#include "Config.hpp"

static bool GetFiles(const std::string& path,std::vector<std::string>& vec){
    DIR* dir=opendir(path.c_str());
    if(dir==nullptr)return false;
    dirent* ptr=nullptr;
    while((ptr=readdir(dir))!=nullptr){
        if(strcmp(ptr->d_name,".")==0||strcmp(ptr->d_name,"..")==0)continue;
        else if(ptr->d_type==8)vec.emplace_back(path+ptr->d_name);
    }
    closedir(dir);
    return true;
}

class OfflineConfig final : public Singleton<OfflineConfig>,public AbstractConfig{
public:

    std::vector<std::string> getEnFiles(){
        std::vector<std::string> ret;
        GetFiles(_map["art-path-en"],ret);
        return ret;
    }

    std::vector<std::string> GetCnFiles(){
        std::vector<std::string> ret;
        GetFiles(_map["art-path-cn"],ret);
        return ret;
    }

    std::vector<std::string> GetXMLFiles(){
        std::vector<std::string> ret;
        GetFiles(_map["webpage"]["xml-path"],ret);
        return ret;
    }

    std::string StopWordsEn() const{return _map["stop-word-en"];}

    std::string StopWordsCn()const{return _map["stop-word-cn"];}

    std::string JiebaStopWordsPath()const{return _map["jieba"]["stop-words"];}

    std::string JiebaIdfPath()const{return _map["jieba"]["idf"];}

    std::string JiebaHmmPath()const{return _map["jieba"]["hmm-model"];}

    std::string JiebaDictPath()const{return _map["jieba"]["jieba-dict"];}

    std::string JiebaUserDictPath()const{return _map["jieba"]["user-dict"];}

    std::string DictOutPath()const{return _map["output"]["dict"];}

    std::string IndexOutPath()const{return _map["output"]["index"];}

    std::string PageLibPath()const{return _map["output"]["page-lib"];}

    std::string PageOffsetPath()const{return _map["output"]["page-offset"];}

    std::string NewPageLibPath()const{return _map["output"]["new-page-lib"];}

    std::string NewPageOffsetPath()const{return _map["output"]["new-page-offset"];}

    std::string InvertIndex()const{return _map["output"]["invert-index"];}
protected:
    bool parse(nlohmann::json & j) override{
        if(j.empty()) return false;
#if 0
        auto json=j["Offline"];
        if(json.empty())return false;
        if(json.contains("art-path-en")){_en_path=json["art-path-en"];}
        if(json.contains("art-path-cn")){_cn_path=json["art-path-cn"];}
        if(json.contains("stop-word-cn")){_cn_stop_words_path=json["stop-word-cn"];}
        if(json.contains("stop-word-en")){_en_stop_words_path=json["stop-word-en"];}
        auto json1=json["jieba"];
        if(json1.empty())return false;
        if(json1.contains("jieba-dict")){_jieba_dict_path=json1["jieba-dict"];}
        if(json1.contains("stop-words")){_jieba_stop_words_path=json1["stop-words"];}
        if(json1.contains("idf")){_jieba_idf_path=json1["idf"];}
        if(json1.contains("hmm-model")){_jieba_hmm_path=json1["hmm-model"];}
        if(json1.contains("user-dict")){_jieba_user_dict_path=json1["user-dict"];}
        auto json2=json["output"];
        if(json2.empty())return false;
        if(json2.contains("dict")){_output_dict_path=json2["dict"];}
        if(json2.contains("index")){_output_index_path=json2["index"];}
#endif
        _map=std::move(j["Offline"]);
        return true;
    }

private:
    nlohmann::json _map;
#if 0
    std::string _en_path;
    std::string _cn_path;
    std::string _cn_stop_words_path;
    std::string _en_stop_words_path;
    std::string _jieba_stop_words_path;
    std::string _jieba_hmm_path;
    std::string _jieba_idf_path;
    std::string _jieba_dict_path;
    std::string _jieba_user_dict_path;
    std::string _output_dict_path;
    std::string _output_index_path;
#endif
};


#endif //TEST_OFFLINECONFIG_HPP
