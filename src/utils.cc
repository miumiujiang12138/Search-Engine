//
// Created by Lenovo on 2022/11/30.
//
#include "../include/utils.hpp"

#include <locale>
#include <codecvt>

namespace Utils{
    int Utf8Bytes(char ch){
        int ret=0;
        for(int i=0;i<6;i++){
            if(ch&(1<<(7-i)))ret++;
            else break;
        }
        return ret==0?1:ret;
    }

    int Utf8Length(const string& str){
        int idx=0;
        int len=0;
        for(;idx<str.size();){
            int n= Utf8Bytes(str[idx]);
            idx+=n;
            len++;
        }
        return len;
    }

    int Utf8MED(const string& str1,const string& str2){
        int len1=Utf8Length(str1);
        int len2=Utf8Length(str2);
        vector<vector<int>> dp(len1+1,vector<int>(len2+1));
        int temp=1;
        for(int i=1;i<=len1;++i)dp[i][0]=temp++;
        temp=1;
        for(int i=1;i<=len2;++i)dp[0][i]=temp++;
        int l_idx=0;
        int r_idx=0;
        for(int i=1;i<=len1;++i){
            int n1=Utf8Bytes(str1[l_idx]);
            string l=str1.substr(l_idx,n1);
            for(int j=1;j<=len2;++j){
                int n2=Utf8Bytes(str2[r_idx]);
                string r=str2.substr(r_idx,n2);
                if(l==r){
                    dp[i][j]=dp[i-1][j-1];
                }
                else{
                    dp[i][j]=min(min(dp[i-1][j-1],dp[i-1][j]),dp[i][j-1])+1;
                }
                r_idx+=n2;
            }
            l_idx+=n1;
            r_idx=0;
        }
        return dp[len1][len2];
    }

    set<string> GetStopList(const std::string& path){
        set<string> ret;
        ifstream ifs(path);
        if(!ifs){
            return {};
        }
        string line;
        while(getline(ifs,line)){
            if(line.empty())continue;
            ret.insert(line);
        }
        ifs.close();
        return ret;
    }

    string ParseXMLLabel(const std::string& str){
        auto first=str.find_first_of('>');
        auto end=str.find("</");
        return str.substr(first+1,end-first-1);
    }

    std::wstring to_wide_string(const std::string& input){
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(input);
    }
    // convert wstring to string
    std::string to_byte_string(const std::wstring& input){
        //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(input);
    }

}