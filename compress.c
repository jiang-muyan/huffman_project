#include "compress.h"
#include "huffman.h"
#include "utils.h"
#include <string.h>

// files[]：要压缩的文件路径数组
// cnt：文件数量
// out：输出的压缩包名字
int compress_files(char* files[], int cnt, const char* out) {
    if (cnt <= 0) return -1;

    FILE* fp = fopen(out, "wb");// 以二进制写模式打开输出文件
    if (!fp) return -1;

    // 统一使用多文件格式
    unsigned char flag = 1;
    fputc(flag, fp);// 把 flag 写入压缩包第一个字节
    fwrite(&cnt, sizeof(int), 1, fp);// 写入文件数量

    //初始化频率表
    unsigned int freq[256] = {0};

    // 第一遍：写入文件名和大小，同时统计频率
    for (int i = 0; i < cnt; i++) {
        FILE* f = fopen(files[i], "rb");
        if (!f) {
            fclose(fp);
            return -1;
        }

        // 统计该文件的频率
        int ch;
        while ((ch = fgetc(f)) != EOF)
            freq[(unsigned char)ch]++;
        rewind(f);// 文件指针回到开头

        // 提取文件名（不含路径）
        char name[256] = {0};
        const char* p = strrchr(files[i], '\\');// 找 Windows 路径最后一个'\'
        if (!p) p = strrchr(files[i], '/');// 找 Linux/mac 路径最后一个 /
        if (p) p++;// 跳过斜杠，指向文件名
        else p = files[i];// 没有路径，直接用文件名
        strncpy(name, p, 255);
        name[255] = '\0';          // 确保字符串结束
        fwrite(name, 256, 1, fp);  // 固定写入256字节（含'\0'）

        long size = get_file_size(files[i]);
        fwrite(&size, sizeof(long), 1, fp);

        fclose(f);
    }

    // 写入频率表
    fwrite(freq, sizeof(unsigned int), 256, fp);

    // 构建哈夫曼树
    HuffmanNode* root = build_huffman_tree(freq);
    HuffmanCode codes[256] = {0};
    generate_codes(root, codes);

    // 第二遍：压缩数据并写入
    unsigned char bitbuf = 0;// 位缓冲区：暂存 0/1 位
    int bitcnt = 0;// 缓冲区里有多少位
    for (int i = 0; i < cnt; i++) {
        FILE* f = fopen(files[i], "rb");
        if (!f) {
            free_tree(root);
            fclose(fp);
            return -1;
        }
        int ch;
        while ((ch = fgetc(f)) != EOF) {
            HuffmanCode* cd = &codes[(unsigned char)ch];
            for (int j = 0; j < cd->len; j++) {
                // 把每一位 0/1 写入缓冲区
                bitbuf <<= 1;//左移一位腾出位置
                bitbuf |= cd->code[j];// 写入当前位
                bitcnt++;
                if (bitcnt == 8) {// 满8位 = 1字节
                    fputc(bitbuf, fp);
                    bitbuf = 0;
                    bitcnt = 0;
                }
            }
        }
        fclose(f);
    }

    // 写入最后一个字节的填充信息
    int pad = 0;
    if (bitcnt > 0) {
        pad = 8 - bitcnt;
        bitbuf <<= pad;
        fputc(bitbuf, fp);
    }
    fputc(pad, fp);

    free_tree(root);
    fclose(fp);
    return 0;
}
