#pragma once
#ifndef __AI_H__
#define __AI_H__

#include <iostream>
#include <vector>
#include <string>
#include <math.h>       // floor()

using std::vector;
using std::string;
using std::cout;
using std::endl;

struct Node {
    int key;                    // number 
    string value;               // genre
    Node* next;                 // pointer to remember memory address of next node

    Node() :key(0), value(""), next(0) {};
    Node(int Key, string Value) :key(Key), value(Value), next(0) {};
    Node(Node const& data) :key(data.key), value(data.value), next(data.next) {};
};

class HashChainNode {
private:
    int size,                 // size: size of table, count: number of data
        count;                // count/size = load factor
    Node** table;             // pointer to pointer, hash table  

    int HashFunction(int key);      // Multiplication method
    void TableDoubling();
    void TableShrinking();
    void Rehashing(int size_orig);

public:
    HashChainNode() {};
    HashChainNode(int m) :size(m), count(0) {
        table = new Node * [size];           // allocate the first demension of table
        for (int i = 0; i < size; i++) {    // initialization
            table[i] = 0;                   // ensure every slot points to NULL
        }
    }
    ~HashChainNode();

    void Insert(Node data);         // consider TableDoubling()
    void Delete(int key);           // consider TableShrinking()
    string Search(int key);
    void DisplayTable();
};

#endif