#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

#define BYTE_SIZE 256

// 定义霍夫曼树节点
typedef struct HuffmanNode {
    unsigned char data;// 存储字节（0~255）
    unsigned int freq;// 该字节出现的频率
    struct HuffmanNode *left, *right;// 左孩子（0），右孩子（1）
} HuffmanNode;

// 定义霍夫曼编码结构
typedef struct {
    unsigned char code[256];// 编码位序列
    int len;// 编码长度
} HuffmanCode;

// 创建节点
HuffmanNode* create_node(unsigned char data, unsigned int freq);
// 构建霍夫曼树
HuffmanNode* build_huffman_tree(unsigned int freq[]);
//生成霍夫曼编码表
void generate_codes(HuffmanNode* root, HuffmanCode codes[]);
// 释放霍夫曼树内存
void free_tree(HuffmanNode* root);

#endif
