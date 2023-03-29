#ifndef _SE_EDITDISTANCE_HPP_
#define _SE_EDITDISTANCE_HPP_


#include <string>

using std::string;

namespace EditDistance
{
    //求一个字符占据的字节数
    size_t nBytesCode(const char ch);
    //求一个字符串的字符长度
    std::size_t length(const std::string &str);
    //中英文通用的最小编辑距离算法
    int editDistance(const std::string & lhs,const std::string &rhs);
}//end of EditDistance


#endif

