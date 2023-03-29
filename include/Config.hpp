//
// Created by Lenovo on 2022/11/25.
//

#ifndef TEST_CONFIG_HPP
#define TEST_CONFIG_HPP

#include "Singleton.hpp"

#include <fstream>

#include "../lib/nlohmann/json.hpp"

using namespace nlohmann;

class AbstractConfig{
public:
    virtual bool load(const std::string& path){
        std::ifstream ifs(path);
        if(!ifs)return false;
        json ret;
        ifs>>ret;
        return parse(ret);
    }
protected:
    virtual bool parse(json&)=0;
};

#endif //TEST_CONFIG_HPP
