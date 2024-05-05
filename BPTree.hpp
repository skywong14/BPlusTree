// Created by skywa on 2024/4/29.
#ifndef BPLUSTREE_BPTREE_HPP
#define BPLUSTREE_BPTREE_HPP
#pragma once
#include <cstdio>
#include <cstring>
#include <vector> //use only for debug
#include <iostream>
#include <fstream>
#include <cassert>
using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

//each internal node with M keys and M+1 sons
template<class T, int Max_Nodes = 300, int M = 4>
class BPTree{
private:
    fstream file, file_value;
    string index_filename, value_filename, filename;
    const int sizeofint = sizeof(int);
    const long long BASE = 197, MOD = 1e9+7;
    int sizeofBasicInformation = sizeof(Basic_Information);
    int sizeofNode = sizeof(Node);
    int sizeofNodeValue = sizeof(Node_Value);

    struct Basic_Information{
        int root_node_id = 0;
        int empty_node_id[Max_Nodes]{};
        //if 0 then used
    };

    //Node's id is 1-based
    struct Node{
        int is_leaf = 0, size = 0, id = 0;
        int pre_node = 0, nxt_node = 0;
        long long index[M + 1]{};
        int sons[M + 2]{};
        Node() = default;
        //if is_leaf == 1: sons point to Value, else point to another node's id
    };

    struct Node_Value{
        T values[M + 2]{};
    };

    Node root;
    Basic_Information basic_info;

    Basic_Information read_Basic_Information(){
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekg(std::ios::beg);
        Basic_Information info;
        file.read((char*)&info, sizeofBasicInformation);
        file.close();
        return info;
    }
    void write_Basic_Information(Basic_Information info_){
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(std::ios::beg);
        file.write(reinterpret_cast<char*>(&info_), sizeofBasicInformation);
        file.close();
    }
    Node read_Node(int pos_){
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekg(sizeofBasicInformation + (pos_ - 1) * sizeofNode, std::ios::beg);
        Node node_;
        file.read((char*)&node_, sizeofNode);
        file.close();
        return node_;
    }

    void write_Node(int pos_, Node node_){
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(sizeofBasicInformation + (pos_ - 1) * sizeofNode, std::ios::beg);
        file.write(reinterpret_cast<char*>(&node_), sizeofNode);
        file.close();
    }

    //todo
    Node_Value read_Node_Value(int pos_){
        file_value.open(value_filename, std::ios::in | std::ios::out | std::ios::binary);
        file_value.seekg((pos_ - 1) * sizeofNodeValue, std::ios::beg);
        Node_Value node_;
        file_value.read((char*)&node_, sizeofNodeValue);
        file_value.close();
        return std::move(node_);
    }
    void write_Node_Value(int pos_, Node_Value node_){
        file_value.open(value_filename, std::ios::in | std::ios::out | std::ios::binary);
        file_value.seekp((pos_ - 1) * sizeofNodeValue, std::ios::beg);
        file_value.write(reinterpret_cast<char*>(&node_), sizeofNodeValue);
        file_value.close();
    }

    int check_file_exists(const string& FN){
        fstream file_tmp(FN);
        if (file_tmp.is_open()){
            file_tmp.close();
            return 1;
        } else
            return 0;
    }

public:
    long long get_Hash(const string& str1){
        long long ha = 0;
        for (int i = 0; i < str1.length(); i++)
            ha = (ha * BASE + (long long)(str1[i]) ) % MOD;
        return ha;
    }

    void initialise_file(){
        int tmp = 0;
        file.open(index_filename, std::ios::out | std::ios::binary);
        file.write(reinterpret_cast<char *>(&tmp), sizeofint);
        file.close();

        file_value.open(value_filename, std::ios::out | std::ios::binary);
        file_value.write(reinterpret_cast<char *>(&tmp), sizeofint);
        file_value.close();
    }
    int initialise(string FN = "", int clear_file = 0){
        if (!FN.empty()) filename = FN;
        index_filename = filename + "_index.txt";
        value_filename = filename + "_value.txt";

        if (check_file_exists(index_filename) && check_file_exists(value_filename) && (!clear_file)) return 0; //文件已经存在就无需初始化

        initialise_file();
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        assert(file.is_open());
        file.close();
        Basic_Information info1;
        info1.root_node_id = 0; //0 means nullptr
        for (int i = 1; i <= Max_Nodes; i++)
            info1.empty_node_id[i-1] = i;
        write_Basic_Information(info1);

        file_value.open(value_filename, std::ios::in | std::ios::out | std::ios::binary);
        assert(file_value.is_open());
        file_value.close();
        return 1;
    }

    int allocate_node(){
        //使用后记得更新basic_info到文件，返回申请得到的node's id
        //如果返回0，说明出错
        int flag = 0;
        for (int i = 0; i < Max_Nodes; i++)
            if (basic_info.empty_node_id[i] > 0) {
                flag = basic_info.empty_node_id[i];
                basic_info.empty_node_id[i] = 0;
                break;
            }
        assert(flag != 0); //for debug only
        return flag;
    }
    int deallocate_node(int id_){
        basic_info.empty_node_id[id_ - 1] = id_;
    }
    void update_Node(int id_, Node node_){
        write_Node(id_, node_);
    }
    void update_Node_and_Values(int id_, Node node_, Node_Value val_){
        write_Node(id_, node_);
        write_Node_Value(id_, val_);
    }

    std::pair<std::vector<int>, Node> find_Node(long long index_hash, T value_){
        //vector end with -1 when the key&values is same
        std::vector<int> trace = {};
        int same_flag = 0;
        Node cur_node = read_Node(basic_info.root_node_id);
        trace.push_back(cur_node.id);

        while (!cur_node.is_leaf){
            int pos = cur_node.size;
            Node_Value node_values;
            int values_flag = 1;

            for (int i = 0; i < cur_node.size; i++){
                if (index_hash < cur_node.index[i]){
                    pos = i;
                    break;
                }
                if (index_hash == cur_node.index[i]){
                    if (values_flag){
                        node_values = read_Node_Value(cur_node.id);
                        values_flag = 0;
                    }
                    if (value_ < node_values.values[i]){
                        pos = i;
                        break;
                    }
                }
            }
            cur_node = read_Node(cur_node.sons[pos]);
            trace.push_back(cur_node.id);
        }
        //then cur_node is a leaf_node
        Node_Value values = read_Node_Value(cur_node.id);
        for (int i = 0; i < cur_node.size; i++){
            if (index_hash == cur_node.index[i] && value_ == values.values[i]) {
                same_flag = 1;
                break;
            }
        }
        if (same_flag) trace.push_back(-1);
        return std::make_pair(trace, cur_node);
    }

    void insert_node(Node cur_node, long long index_, T val_, int ptr_){
        //insert index_ and val_ inside cur_node
        Node_Value cur_values = read_Node_Value(cur_node.id);
        int pos = cur_node.size;
        for (int i = 0; i < cur_node.size; i++)
            if (cur_node.index[i] > index_ || cur_node.index[i] == index_ && cur_values.values[i] >val_){
                pos = i;
                break;
            }
        //then insert at position pos
        for (int i = cur_node.size; i > pos; i--){
            cur_node.index[i] = cur_node.index[i - 1];
            cur_node.sons[i + 1] =cur_node.sons[i - 1 + 1];
            cur_values.values[i] = cur_values.values[i - 1];
        }

        cur_node.index[pos] = index_;
        cur_node.sons[pos + 1] = ptr_;
        cur_values.values[pos] = val_;
        cur_node.size++;

        update_Node_and_Values(cur_node.id, cur_node, cur_values);
    }

    int insert(const string& str1, T value_){
        //return 0 is key&value is same
        long long index_hash = get_Hash(str1);

        basic_info = read_Basic_Information();

        if (basic_info.root_node_id == 0){ //BPTree is empty
            basic_info.root_node_id = allocate_node();
            Node new_node;
            Node_Value new_node_value;
            new_node.id = basic_info.root_node_id;
            new_node.size = 1;
            new_node.is_leaf = 1;
            new_node.index[0] = index_hash;
            new_node.sons[0] = 0;
            new_node_value.values[0] = value_;
            update_Node_and_Values(new_node.id, new_node, new_node_value);
        } else {
            std::pair<std::vector<int>, Node> ret_ = find_Node(index_hash, value_);
            if (ret_.first.back() == -1) return 0;
            Node cur_node = ret_.second;
            std::vector<int> trace = ret_.first;
            int trace_cnt = int(trace.size()) - 1;
            //then insert in cur_node and not the head of cur_node is guaranteed
            //we need to insert index_hash&inserted_value to cur_node
            long long inserted_index = index_hash;
            int inserted_ptr = 0;
            T inserted_value = value_;

            while (true){
                if (cur_node.size < M){
                    insert_node(cur_node, inserted_index, inserted_value, inserted_ptr);
                    break;
                } else {
                    //split
                    //insert on node then split the node
                    //cur_node.size = M (attention: but has M + 1 sons)
                    int exist_nxt_node = cur_node.nxt_node;
                    Node new_node, nxt_node;
                    Node_Value new_values, cur_values = read_Node_Value(cur_node.id);

                    //insert first
                    int pos = cur_node.size;
                    for (int i = 0; i < cur_node.size; i++)
                        if (cur_node.index[i] > inserted_index || cur_node.index[i] == inserted_index && cur_values.values[i] >inserted_value){
                            pos = i;
                            break;
                        }
                    for (int i = cur_node.size; i > pos; i--){
                        cur_node.index[i] = cur_node.index[i - 1];
                        cur_node.sons[i + 1] =cur_node.sons[i - 1 + 1];
                        cur_values.values[i] = cur_values.values[i - 1];
                    }
                    cur_node.index[pos] = inserted_index;
                    cur_node.sons[pos + 1] = inserted_ptr;
                    cur_values.values[pos] = inserted_value;
                    cur_node.size++;

                    //then split, now cur_node.size = M + 1
                    new_node.id = allocate_node();
                    if (cur_node.is_leaf){
                        cur_node.size = (M + 1) / 2;
                        new_node.size = M + 1 - (M + 1) / 2;
                        for (int i = 0; i < new_node.size; i++){
                            new_node.index[i] = cur_node.index[i + (M + 1) / 2];
                            new_node.sons[i] = cur_node.sons[i + (M + 1) / 2 + 1];
                            new_values.values[i] = cur_values.values[i + (M + 1) / 2];
                        }
                        if (exist_nxt_node){
                            nxt_node = read_Node(cur_node.nxt_node);
                            nxt_node.pre_node = new_node.id;
                        }
                        new_node.is_leaf = cur_node.is_leaf;
                        new_node.nxt_node = cur_node.nxt_node; new_node.pre_node = cur_node.id;
                        cur_node.nxt_node = new_node.id;

                        inserted_index = new_node.index[0];
                        inserted_value = new_values.values[0];
                        inserted_ptr = new_node.id;
                    } else {
                        new_node.size = M - M / 2;
                        for (int i = 0; i < new_node.size; i++){
                            new_node.index[i] = cur_node.index[i + M / 2 + 1];
                            new_node.sons[i] = cur_node.sons[i + M / 2 + 1];
                            new_values.values[i] = cur_values.values[i + M / 2 + 1];
                        }
                        new_node.sons[new_node.size] = cur_node.sons[cur_node.size];
                        cur_node.size = M / 2;

                        if (exist_nxt_node){
                            nxt_node = read_Node(cur_node.nxt_node);
                            nxt_node.pre_node = new_node.id;
                        }
                        new_node.is_leaf = cur_node.is_leaf;
                        new_node.nxt_node = cur_node.nxt_node; new_node.pre_node = cur_node.id;
                        cur_node.nxt_node = new_node.id;

                        inserted_index = cur_node.index[M / 2];
                        inserted_value = cur_values.values[M / 2];
                        inserted_ptr = new_node.id;
                    }

                    if (cur_node.id == basic_info.root_node_id){
                        Node new_root;
                        Node_Value new_root_values;
                        new_root.id = allocate_node();
                        basic_info.root_node_id = new_root.id;
                        new_root.size = 1;
                        new_root.is_leaf = 0;
                        new_root.index[0] = inserted_index;
                        new_root_values.values[0] = inserted_value;
                        new_root.sons[0] = cur_node.id; new_root.sons[1] = inserted_ptr;

                        if (exist_nxt_node) update_Node(nxt_node.id, nxt_node);
                        update_Node_and_Values(new_root.id, new_root, new_root_values);
                        update_Node_and_Values(cur_node.id, cur_node, cur_values);
                        update_Node_and_Values(new_node.id, new_node, new_values);
                        break;
                    }
                    if (exist_nxt_node) update_Node(nxt_node.id, nxt_node);
                    update_Node_and_Values(cur_node.id, cur_node, cur_values);
                    update_Node_and_Values(new_node.id, new_node, new_values);

                    trace_cnt--;
                    cur_node = read_Node(trace[trace_cnt]);
                }
            }

        }

        write_Basic_Information(basic_info);
        return 1;
    }

    void search_values(const string& str_index){
        long long index_hash = get_Hash(str_index);
        int same_flag = 0;
        Node cur_node = read_Node(basic_info.root_node_id);

        while (!cur_node.is_leaf){
            int pos = cur_node.size;
            Node_Value node_values;
            int values_flag = 1;

            for (int i = 0; i < cur_node.size; i++)
                if (index_hash <= cur_node.index[i]){
                    pos = i;
                    break;
                }
            cur_node = read_Node(cur_node.sons[pos]);
        }
        //then cur_node is a leaf_node
        while (cur_node.index[cur_node.size - 1] < index_hash && cur_node.nxt_node > 0) cur_node = read_Node(cur_node.nxt_node);

        Node_Value values = read_Node_Value(cur_node.id);
        int flag = 1;
        std::vector<T> val = {};
        while (flag){
            for (int i = 0; i < cur_node.size; i++)
                if (cur_node.index[i] == index_hash){
                    val.push_back(values.values[i]);
                } else if (cur_node.index[i] > index_hash) {flag = 0; break;}
            if (cur_node.nxt_node > 0) {
                cur_node = read_Node(cur_node.nxt_node);
                values = read_Node_Value(cur_node.id);
            }
            else break;
        }
        if (val.empty()) std::cout<<"null"<<std::endl;
        else {
            for (int i = 0; i < val.size() - 1; i++) std::cout<<val[i]<<' ';
            std::cout<<val[val.size() - 1]<<std::endl;
        }
    }

    void output_dfs(int id, int space){
        Node n = read_Node(id);
        Node_Value v = read_Node_Value(id);
        for (int ii= 0; ii < space; ii++) std::cout<<' ';
        std::cout<<"Node_"<<id<<':'<<" size="<<n.size<<','<<" index0="<<n.index[0]<<','<<" is_leaf="<<n.is_leaf<<std::endl;
        if (n.is_leaf){
            for (int i = 0; i < n.size; i++){
                for (int ii= 0; ii < space; ii++) std::cout<<' ';
                std::cout<<"|son_"<<i<<':'<<n.index[i]<<','<<v.values[i]<<std::endl;
            }
        }else{
            for (int ii= 0; ii < space; ii++) std::cout<<' ';
            std::cout<<"|son_0:"<<std::endl;
            output_dfs(n.sons[0], space + 3);
            for (int i = 1; i <= n.size; i++){
                for (int ii= 0; ii < space; ii++) std::cout<<' ';
                std::cout<<"|son_"<<i<<": "<<n.index[i - 1]<<", "<<v.values[i - 1]<<std::endl;
                output_dfs(n.sons[i], space + 3);
            }
        }
    }
    void output(){
        std::cout<<">>>>>>>>>>>>>>>"<<std::endl;
        Basic_Information info_ = read_Basic_Information();
        output_dfs(info_.root_node_id,0);
        std::cout<<"<<<<<<<<<<<<"<<std::endl;
    }
    void debug(){
        Basic_Information info = read_Basic_Information();
        std::cout<<info.root_node_id<<std::endl;
        Node n = read_Node(1);
        std::cout<<n.size<<':';
    }

    BPTree() = default;
    explicit BPTree(const string& FN) : filename(FN) {}

    ~BPTree() = default;

};
#endif //BPLUSTREE_BPTREE_HPP
