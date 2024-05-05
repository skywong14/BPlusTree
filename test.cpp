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

    for (int i = 30; i >= 1; i--){
        std::cout<<i<<':'<<tr.insert(std::to_string(i), 100+i)<<std::endl;
//        tr.output();
    }
//    tr.output();
//    int x = 25;
//    std::cout<<x<<':'<<tr.insert(std::to_string(x), 100+x)<<std::endl;
//    tr.output();
//return 0;

    for (int i = 1; i <= 50; i++){
        std::cout<<i<<':'<<tr.insert(std::to_string(i), 100+i)<<std::endl;
//        tr.output();
    }
    tr.output();



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
}