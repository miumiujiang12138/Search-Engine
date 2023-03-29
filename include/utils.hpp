//
// Created by Lenovo on 2022/11/27.
//

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP


#include <vector>
#include <string>
#include <fstream>
#include <set>

using std::vector;
using std::string;
using std::min;
using std::ifstream;
using std::set;

namespace Utils{
    int Utf8Bytes(char ch);

    int Utf8Length(const string& str);

    int Utf8MED(const string& str1,const string& str2);

    set<string> GetStopList(const std::string& path);

    string ParseXMLLabel(const std::string& str);

    std::wstring to_wide_string(const std::string& input);

    std::string to_byte_string(const std::wstring& input);
}

#endif //TEST_UTILS_HPP
