//
// Created  on 2022/11/25.
//

#ifndef TEST_SINGLETON_HPP
#define TEST_SINGLETON_HPP

#include <utility>

template<typename T>
class Singleton{
public:
    static T& getInstance(){
        static T unique_instance;
        return unique_instance;
    }
    virtual ~Singleton()=default;
public:
    Singleton(const Singleton&)=delete;
    Singleton(Singleton&&)=delete;
    Singleton& operator=(const Singleton&)=delete;
    Singleton& operator=(Singleton&&)=delete;
protected:
    Singleton()=default;
};


#endif //TEST_SINGLETON_HPP
