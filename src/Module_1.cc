#include "../include/Module_1.hpp"


Module_1::Module_1()
:_hotspot()
{
    OfflineConfig::getInstance().load(OFFLINE_JSON);
}


void Module_1::CreatDatEnd()
{
    _dic_end.LoadFiles();
    _dic_end.CreatStopMap();
    _dic_end.CreateMap();
    _dic_end.WriteMap();
    _dic_end.ReleaseMap();
}

void Module_1::LoadDatEnd()
{
    _dic_end.LoadRedis();
}

result_type Module_1::RecommendEnd(string keyword,size_t num)
{
    _hotspot.LockRedis();
    if(_hotspot.IsHotspot(keyword))
    {
        auto the_re1=_hotspot.HotMessage(keyword);
        _hotspot.UnlockRedis();
        return the_re1;
    }else
    {
        auto the_result=_dic_end.RecommendWords(keyword,num);
        _hotspot.SetHotspotMessage(keyword,the_result);
        _hotspot.UnlockRedis();
        return the_result;
    }
}

void Module_1::CreatDatZh(){
    /* OfflineConfig::getInstance().load("../conf/offline.json"); */
    auto split =new SplitTool;
    ChineseDictionary dictionay(split);
    dictionay.BuildCnDict();
    dictionay.BuildCnIndex();
    dictionay.StoreDict();
    dictionay.StoreIndex();
}

void Module_1::LoadDatZh(){
    /* OfflineConfig::getInstance().load("../conf/offline.json"); */
    auto &cof=OfflineConfig::getInstance();

    //第三处改动
    _dic_zn.RedsiConnect();
    _dic_zn.InitDict(cof.DictOutPath());
    _dic_zn.InitIndex(cof.IndexOutPath());

    //第一处改动
    _dic_zn.DictRedis();
    _dic_zn.IndexRedis();
}


result_type Module_1::RecommendZh(string keyword,size_t num){
    _hotspot.LockRedis();
    if (_hotspot.IsHotspot(keyword))
    {
        auto rets=_hotspot.HotMessage(keyword);
        _hotspot.UnlockRedis();
        return rets;
    }else
    {
        //第二处改动
        auto rets=_dic_zn.RedisRecommend(keyword,num);
        _hotspot.SetHotspotMessage(keyword,rets);
        _hotspot.UnlockRedis();
        return rets;
    }
}




#if 0
Module_1* Module_1::_p_module=nullptr; 
int main()
{
    string tmp1="中国";
    string tmp2="hello";
    Module_1::GetInstance()->LoadDatZh();
    Module_1::GetInstance()->LoadDatEnd();

    for(auto &rc:Module_1::GetInstance()->RecommendZh(tmp1,5))
    {
        cout<<rc<<endl;
    }
    cout<<endl;
    for(auto &rc:Module_1::GetInstance()->RecommendEnd(tmp2,5))
    {
        cout<<rc<<endl;
    }
    return 0;
}
#endif 
