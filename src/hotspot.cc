#include "../include/hotspot.hpp"

hotspot::hotspot()
:_redis()
{
    if(!_redis.connect(REDIS_IP,REDIS_PORT)){
        ::printf("can't connect redis-server :%s %d 3",REDIS_IP,REDIS_PORT);
        exit(1);
    }
    ::printf("in tb 3\n");
    _redis.select("3");
}


bool hotspot::IsHotspot(string keyword)
{
    // ::printf("is hot?\n");
    return _redis.expire(keyword,ALIVE_TIME);
}

void hotspot::SetHotspotMessage(string keyword,vector<string> msg)
{
    string val=" ";
    val.reserve(msg.size()*10); 
    for(auto&rs:msg)
    {
        val+=rs+" ";
    }
    if(!_redis.expire(keyword,ALIVE_TIME)){
        _redis.sadd(keyword, val);
        _redis.expire(keyword, ALIVE_TIME);
    }
}



