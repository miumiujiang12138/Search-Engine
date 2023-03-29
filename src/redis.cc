#include "../include/redis.hpp"


vector<string> redis::sinter(std::string key1,std::string key2)
{
    std::string command="SINTER "+key1+key2;
        // ::printf("in sinter\n");
    auto _reply=(redisReply*)redisCommand(this->_connect,command.c_str());
        // ::printf("in sinter\n");
    redisReply** elements=_reply->element;
        // ::printf("in sinter\n");
    size_t element=_reply->elements;
        // ::printf("in sinter\n");
    vector<string> strs;
    if(_reply->type==REDIS_REPLY_ERROR)
    {
        // ::printf("%s IN SINTER ERROR:%s\n",key1.c_str(),_reply->str);
        return strs;
    }
    for(size_t i=0;i<element;++i)
    {
        strs.push_back(elements[i]->str);
        /* freeReplyObject(elements[i]); */
    }
        freeReplyObject(_reply);
        // ::printf("end of sinter :%d %s\n",strs.size(),key1.c_str());
    return strs;
}

void redis::set(std::string key,std::string value)
{
   auto _reply=(redisReply*)redisCommand(this->_connect,"SET %s %s",key.c_str(),value.c_str());
   freeReplyObject(_reply); 
}

std::string redis::get(std::string key)
{
    auto _reply=(redisReply*)redisCommand(this->_connect,"GET %s",key.c_str());
    if(_reply->type==REDIS_REPLY_NIL)
    {
        // ::printf("null\n");
        freeReplyObject(_reply);
        std::string tmp="";
        return tmp;
    }
        // ::printf("not null\n");
    std::string str=_reply->str;
    freeReplyObject(_reply);
    return str;
}

void redis::sadd(std::string key,std::string member)
{
    string commind ="SADD "+key+" "+member;
    auto _reply=(redisReply*)redisCommand(this->_connect,commind.c_str());
    freeReplyObject(_reply);
}

void redis::select(std::string index)
{
    std::string command="SELECT "+index;
    auto _reply=(redisReply*)redisCommand(this->_connect,command.c_str());
    freeReplyObject(_reply);
}

bool redis::expire(std::string msg,std::string times)
{
    std::string command="EXPIRE "+msg+" "+times;
    auto _reply=(redisReply*)redisCommand(this->_connect,command.c_str());
    if(_reply->integer==0)
    {
        freeReplyObject(_reply);
        return false;
    }else{
        freeReplyObject(_reply);
        return true;
    }
}

#if 0
int main()
{
    redis r_use;
    r_use.connect(REDIS_IP,REDIS_PORT);

    r_use.select("4");
    r_use.expire("hello","10");

}
#endif

