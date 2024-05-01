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

//for dubug only
template<typename T>
void debug(T arg){
    std::cout << arg << std::endl;
}
template<typename T, typename... Args>
void debug(T arg, Args... args){
    std::cout << arg << " ";
    debug(args...);
}

//each internal node with M keys and M+1 sons
template<class T, int Max_Nodes = 100, int M = 4>
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
        int root_node_id;
        int empty_node_id[Max_Nodes];
        //if 0 then used
    };

    //Node's id is 1-based
    struct Node{
        int is_leaf, size, id;
        int pre_node, nxt_node;
        long long index[M];
        int sons[M + 1];
        //if is_leaf == 1: sons point to Value, else point to another node's id
    };

    struct Node_Value{
        T values[M + 1];
    };

    Node root;
    Basic_Information basic_info;

    Basic_Information read_Basic_Information(){
        //file is open before
        file.seekg(std::ios::beg);
        Basic_Information info;
        file.read((char*)&info, sizeofBasicInformation);
        return info;
    }
    void write_Basic_Information(Basic_Information info_){
        //file is open before
        file.seekp(std::ios::beg);
        file.write(reinterpret_cast<char*>(&info_), sizeofBasicInformation);
    }
    Node read_Node(int pos_){
        //file is open before
        file.seekg(sizeofBasicInformation + (pos_ - 1) * sizeofNode, std::ios::beg);
        Node node_;
        file.read((char*)&node_, sizeofNode);
        return node_;
    }

    void write_Node(int pos_, Node node_){
        //file is open before
        file.seekp(sizeofBasicInformation + (pos_ - 1) * sizeofNode, std::ios::beg);
        file.write(reinterpret_cast<char*>(&node_), sizeofNode);
    }

    //todo
    Node_Value read_Node_Value(int pos_){
        file_value.seekg((pos_ - 1) * sizeofNodeValue, std::ios::beg);
        Node_Value node_;
        file_value.read((char*)&node_, sizeofNodeValue);
        return std::move(node_);
    }
    void write_Node_Value(int pos_, Node_Value node_){
        file_value.seekp((pos_ - 1) * sizeofNodeValue, std::ios::beg);
        file_value.write(reinterpret_cast<char*>(&node_), sizeofNode);
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
        Basic_Information info1;
        info1.root_node_id = 0; //0 means nullptr
        for (int i = 1; i <= Max_Nodes; i++)
            info1.empty_node_id[i-1] = i;
        write_Basic_Information(info1);
        file.close();

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
        assert(flag != 0); //todo: for debug only
        return flag;
    }
    int deallocate_node(int id_){
        basic_info.empty_node_id[id_ - 1] = id_;
    }
    void update_Node(int id_, Node node_){
        write_Node(id_, node_);
    }

    void update_Node_and_Values(int id_, Node node_, Node_Value val_){
        //file be opened before
        write_Node(id_, node_);
        write_Node_Value(id_, val_);
    }

    /*void insert_value(int node_id, int pos_, T value_){
        //file_value is opened before
        //insert value_ at position pos_ in the node_id_th node
        Node_Value node_ = read_Node_Value(node_id);
        node_.values[pos_ - 1] = value_;
        write_Node_Value(node_id, node_);
    }*/

    /*
    Node find_Node(long long index_hash){
        //be called in insert only
        //todo
        Node cur_node = read_Node(basic_info.root_node_id);
        while (!cur_node.is_leaf){
            int pos = cur_node.size + 1;
            for (int i = 0; i < cur_node.size; i++){
                if (index_hash <= cur_node[i]){
                    pos = i;
                    break;
                }
                cur_node = read_Node(cur_node.sons[pos]);
            }
        }
        //then cur_node is a leaf_node
        int flag = 1;
        while (flag){
            Node_Value values = read_Node_Value(cur_node.id);
            if (index_hash == cur_node.index[cur_node.size - 1] && values[cur_node.size - 1] < value_ && cur_node.nxt_node != 0){
                //should be in the nxt_node
                cur_node = read_Node(cur_node.nxt_node);
                continue;
            }
            //then just insert in cur_node
            flag = 0;
        }
        return cur_node;
    }
    */


    std::pair<std::vector<int>, Node> find_Node(long long index_hash, T value_){
        //return false when the key&values is same
        //be called in insert only
        //todo
        std::vector<int> trace = {}, empty_trace = {};

        Node cur_node = read_Node(basic_info.root_node_id);
        trace.push_back(cur_node.id);

        while (!cur_node.is_leaf){
            int pos = cur_node.size + 1;

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
                    if (node_values.values[i] == value_){
                        return std::make_pair(empty_trace, cur_node);
                        //same
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
            if (index_hash == cur_node.index[i] && value_ == values.values[i]) return std::make_pair(empty_trace, cur_node);
        }
        //a tiny optimize here
        return std::make_pair(trace, cur_node);
    }

    void insert_node(Node cur_node, long long index_, T val_, int ptr_){
        //insert index_ and val_ inside cur_node
        Node_Value cur_values = read_Node_Value(cur_node.id);
        std::cout<<cur_node.id<<','<<basic_info.root_node_id<<';'<<cur_node.index[0]<<','<<index_<<std::endl;
        std::cout<<(cur_node.index[0] <= index_ || cur_node.id == basic_info.root_node_id)<<std::endl;
        assert(cur_node.index[0] <= index_ || cur_node.id == basic_info.root_node_id); //非root时 不可能index_ < 叶子节点的第一个index0.025
        int pos = cur_node.size + 1;
        for (int i = 0; i < cur_node.size; i++)
            if (cur_node.index[i] > index_ || cur_node.index[i] == index_ && cur_values.values[i] >val_){
                pos = i;
                break;
            }
        //then insert at position pos
        for (int i = cur_node.size; i > pos; i--){
            cur_node.index[i] = cur_node.index[i - 1];
            cur_node.sons[i] =cur_node.sons[i - 1];
            cur_values.values[i] = cur_values.values[i - 1];
        }

        cur_node.index[pos] = index_;
        cur_node.sons[pos] = ptr_;
        cur_values.values[pos] = val_;
        cur_node.size++;

        update_Node_and_Values(cur_node.id, cur_node, cur_values);
    }

    int insert(const string& str1, T value_){
        //return 0 is key&value is same
        long long index_hash = get_Hash(str1);
        file.open(index_filename, std::ios::in | std::ios::out | std::ios::binary);
        file_value.open(value_filename, std::ios::in | std::ios::out | std::ios::binary);

        basic_info = read_Basic_Information();

        if (basic_info.root_node_id == 0){ //BPTree is empty
            basic_info.root_node_id = allocate_node();
            Node new_node;
            Node_Value new_node_value;
            new_node.id = basic_info.root_node_id;
            new_node.size = 1;
            new_node.is_leaf = 1;
            new_node.index[0] = index_hash;
            new_node.sons[0] = 1; //1-based
            new_node_value.values[0] = value_;
            update_Node_and_Values(new_node.id, new_node, new_node_value);
        } else {
            std::pair<std::vector<int>, Node> ret_ = find_Node(index_hash, value_);
            if (ret_.first.empty()) return 0;
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
                    //insert on leaf then split on leaf
                    //cur_node.size = M
                    int exist_nxt_node = cur_node.nxt_node;
                    Node new_node, nxt_node;
                    Node_Value new_values, cur_values = read_Node_Value(cur_node.id);
                    new_node.id = allocate_node();
                    new_node.size = M - M / 2;
                    for (int i = 0; i < new_node.size; i++){
                        new_node.index[i] = cur_node.index[i + M / 2];
                        new_node.sons[i] = cur_node.sons[i + M / 2];
                        new_values.values[i] = cur_values.values[i + M / 2];
                    }
                    if (exist_nxt_node){
                        nxt_node = read_Node(cur_node.nxt_node);
                        nxt_node.pre_node = new_node.id;
                    }
                    new_node.is_leaf = cur_node.is_leaf;
                    new_node.nxt_node = cur_node.nxt_node; new_node.pre_node = cur_node.id;
                    cur_node.nxt_node = new_node.id;

                    new_node.index[new_node.size] = inserted_index;
                    new_node.sons[new_node.size] = inserted_ptr;
                    new_values.values[new_node.size] = inserted_value;
                    new_node.size++;

                    cur_node.size = M / 2;

                    if (cur_node.id == basic_info.root_node_id){
                        Node new_root;
                        Node_Value new_root_values;
                        new_root.id = allocate_node();
                        basic_info.root_node_id = new_root.id;
                        new_root.size = 1;
                        new_root.is_leaf = 0;
                        new_root.index[0] = new_node.index[0];
                        new_root_values.values[0] = new_values.values[0];
                        new_root.sons[0] = cur_node.id; new_root.sons[1] = new_node.id;

                        if (exist_nxt_node) update_Node(nxt_node.id, nxt_node);
                        update_Node_and_Values(new_root.id, new_root, new_root_values);
                        update_Node_and_Values(cur_node.id, cur_node, cur_values);
                        update_Node_and_Values(new_node.id, new_node, new_values);
                        break;
                    }
                    if (exist_nxt_node) update_Node(nxt_node.id, nxt_node);
                    update_Node_and_Values(cur_node.id, cur_node, cur_values);
                    update_Node_and_Values(new_node.id, new_node, new_values);

                    inserted_index = new_node.index[0];
                    inserted_value = new_values.values[0];
                    inserted_ptr = cur_node.id;

                    trace_cnt--;
                    cur_node = read_Node(trace[trace_cnt]);
                }
            }

        }

        write_Basic_Information(basic_info);

        file.close();
        file_value.close();
        return 1;
    }


    BPTree() = default;
    explicit BPTree(const string& FN) : filename(FN) {}

    ~BPTree() = default;

};
#endif //BPLUSTREE_BPTREE_HPP
