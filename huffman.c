#include "huffman.h"

// 创建一个节点，申请内存 → 赋值 → 左右孩子为空 → 返回节点
HuffmanNode* create_node(unsigned char data, unsigned int freq) {
    HuffmanNode* node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    node->data = data;
    node->freq = freq;
    node->left = node->right = NULL;
    return node;
}

// 对霍夫曼节点指针数组按频率进行升序排序
static void sort_nodes(HuffmanNode** nodes, int cnt) {
    for(int i = 0; i < cnt - 1; i++){
        for(int j = 0; j < cnt - i - 1; j++){
            if(nodes[j]->freq > nodes[j+1]->freq){
                HuffmanNode* t = nodes[j];
                nodes[j] = nodes[j+1];
                nodes[j+1] = t;
            }
        }
    }
}

// 构建霍夫曼树
HuffmanNode* build_huffman_tree(unsigned int freq[]) {
    //创建一个指针数组，用来存放所有叶子节点
    HuffmanNode** nodes = (HuffmanNode**)malloc(BYTE_SIZE * sizeof(HuffmanNode*));
    int cnt = 0;

    //把所有频率>0的字节做成叶子节点
    for(int i = 0; i < BYTE_SIZE; i++){
        if(freq[i] > 0){
            nodes[cnt++] = create_node((unsigned char)i, freq[i]);
        }
    }

    // 如果没有任何字节出现（所有文件大小为0），返回NULL
    if (cnt == 0) {
        free(nodes);
        return NULL;
    }
    
    //处理只有一个字符的情况
    if (cnt == 1) {
        // 创建虚拟父节点，频度为 0
        HuffmanNode* parent = create_node(0, nodes[0]->freq);
        parent->left = nodes[0];
        parent->right = NULL;   // 右孩子为空
        free(nodes);
        return parent;
    }

    //循环选出最小两个节点合并，生成父节点
    while(cnt > 1){
        sort_nodes(nodes, cnt);//先排序，让最小的两个节点排在最前面
        HuffmanNode *l = nodes[0], *r = nodes[1];//左，右孩子
        HuffmanNode* p = create_node(0, l->freq + r->freq);//父节点
        p->left = l;
        p->right = r;

        nodes[0] = p;//把新的父节点放回数组
        for(int i = 1; i < cnt - 1; i++) nodes[i] = nodes[i+1];//删除原来的两个子节点
        cnt--;//节点总数 cnt 减 1
    }

    // 最后只剩一个根节点
    HuffmanNode* root = nodes[0];
    free(nodes);
    return root;
}

//递归遍历霍夫曼树，生成每个字节的编码
static void traverse(HuffmanNode* root, unsigned char buf[], int dep, HuffmanCode codes[]) {
    if(!root) return;
    if(!root->left && !root->right){//如果是叶子节点（没有孩子），说明走到了一个字节
        codes[root->data].len = dep;//记录编码长度
        //把临时存储的 0/1 编码，复制到最终编码表中
        for(int i = 0; i < dep; i++) codes[root->data].code[i] = buf[i];
        return;
    }
    //向左走 → 记 0 → 递归
    buf[dep] = 0;
    traverse(root->left, buf, dep+1, codes);
    //向右走 → 记 1 → 递归
    buf[dep] = 1;
    traverse(root->right, buf, dep+1, codes);
}

//生成完整霍夫曼编码表
void generate_codes(HuffmanNode* root, HuffmanCode codes[]) {
    //创建临时缓冲区，调用递归函数，开始生成编码
    unsigned char buf[256];
    traverse(root, buf, 0, codes);
}

//释放整棵树的内存，避免内存泄漏
void free_tree(HuffmanNode* root) {
    if(!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
