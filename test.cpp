//
// Created by skywa on 2024/4/29.
//
#include <cstdio>
#include <iostream>
#include <cstring>
#include "BPTree.hpp"
using std::cin;
using std::cout;
using std::endl;
BPTree<int> tr;
int main() {
    tr.initialise("happy");
    int T;cin>>T;
    string s,ss,sss;
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
        }else {
            cin>>ss>>v;
            tr.erase(ss,v);
        }
    }
    return 0;
}
/*#include <cstdio>
#include <iostream>
#include <cstring>
#include "BPTree.hpp"
BPTree<int> tr;
int main(){
    tr.initialise("happy");

    for (int i = 1; i<=10; i++){
        std::cout<<i<<':'<<tr.insert(std::to_string(i), 1000+i)<<std::endl;
        std::cout<<i<<':'<<tr.insert(std::to_string(i), 50+i)<<std::endl;
//        tr.output();
    }

    tr.output();
//    int x = 25;
//    std::cout<<x<<':'<<tr.insert(std::to_string(x), 100+x)<<std::endl;
//    tr.output();
//return 0;

//    for (int i = 1; i <= 50; i++){
//        std::cout<<i<<':'<<tr.insert(std::to_string(i), 100+i)<<std::endl;
//        tr.output();
//    }

    for (int i = 1; i <= 10; i++){
        tr.search_values(std::to_string(i));
    }

//    tr.output();



    return 0;
    std::cout<<2<<':'<<tr.insert(std::to_string(2), 2)<<std::endl;
    tr.output();

    std::cout<<3<<':'<<tr.insert(std::to_string(3), 3)<<std::endl;

    tr.output();

    std::cout<<4<<':'<<tr.insert(std::to_string(4), 4)<<std::endl;


    tr.output();
    std::cout<<5<<':'<<tr.insert(std::to_string(5), 5)<<std::endl;

    tr.output();
    return 0;
}*/