#ifndef _CTL_HOTSPOT_HPP_
#define _CTL_HOTSPOT_HPP_

#include "../include/redis.hpp"

#define ALIVE_TIME "60"

class hotspot
{
public:
    hotspot();
    ~hotspot() {}
    bool IsHotspot(string keyword);
    void SetHotspotMessage(string keyword,vector<string> msg);
    vector<string> HotMessage(string keyword);
    void LockRedis(){_redis.LockMutex();}
    void UnlockRedis(){_redis.UnlockMutex();}
private:
    redis _redis;
};

inline
vector<string> hotspot::HotMessage(string keyword)
{
    return _redis.sinter(keyword," ");
}

#endif
