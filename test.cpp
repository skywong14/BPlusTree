// Created by skywa on 2024/4/29.
#include <cstdio>
#include <iostream>
#include <cstring>
#include "vector.hpp"
#include "BPTree.hpp"
using std::string;
using std::cin;
using std::cout;
using std::endl;
BPTree<int> tr;
int main() {
//    freopen("data","r",stdin);
    tr.initialise("happy");
    int T;
    cin>>T;
    string s,ss;
    int v;
    while (T--){
        cin>>s;
        if (s == "insert"){
            cin>>ss>>v;
            tr.insert(ss,v);
        }else
        if (s=="find"){
            cin>>ss;
            auto val = tr.search_values(ss);
            if (val.empty()) std::cout<<"null"<<std::endl;
            else {
                for (int i = 0; i < val.size() - 1; i++) std::cout<<val[i]<<' ';
                std::cout<<val[val.size() - 1]<<std::endl;
            }
        }else if (s == "delete") {
            cin>>ss>>v;
            tr.erase(ss,v);
        }
    }
    return 0;
}