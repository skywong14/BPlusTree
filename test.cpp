//
// Created by skywa on 2024/4/29.
//
#include <cstdio>
#include <iostream>
#include <cstring>
#include "BPTree.hpp"
BPTree<int> tr;
int main(){
    tr.initialise("happy");
    std::cout<<tr.insert("1",1)<<std::endl;
    std::cout<<tr.insert("2",1)<<std::endl;
    std::cout<<tr.insert("3",1)<<std::endl;
    std::cout<<tr.insert("4",1)<<std::endl;
    return 0;
}