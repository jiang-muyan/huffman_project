#include "decompress.h"
#include "huffman.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 文件信息
typedef struct {
    char name[256];
    long size;
} FileInfo;

//解压文件，参数 src 是压缩包路径，outdir 是输出目录
int decompress_files(const char* src, const char* outdir) {
    FILE* fp = fopen(src, "rb");
    if (!fp) return -1;

    // 1. 读取 flag
    unsigned char flag = fgetc(fp);
    if (flag != 1) {
        fclose(fp);
        return -1;
    }

    // 2. 读取文件个数
    int cnt = 0;
    if (fread(&cnt, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    // 3. 读取文件信息
    FileInfo* files = (FileInfo*)malloc(cnt * sizeof(FileInfo));
    if (!files) { 
        fclose(fp); 
        return -1; 
    }

    //循环读取每个文件的信息
    for (int i = 0; i < cnt; i++) {
        if (fread(files[i].name, 256, 1, fp) != 1) {
            free(files); 
            fclose(fp); 
            return -1;
        }
        files[i].name[255] = '\0';
        if (fread(&files[i].size, sizeof(long), 1, fp) != 1) {
            free(files); 
            fclose(fp); 
            return -1;
        }
    }

    // 4. 读取频率表
    unsigned int freq[256] = {0};
    if (fread(freq, sizeof(unsigned int), 256, fp) != 256) {
        free(files); 
        fclose(fp); 
        return -1;
    }

    // 5. 构建哈夫曼树
    HuffmanNode* root = build_huffman_tree(freq);

    // 如果所有文件都是空的
    if (!root) {
        //循环创建每一个空文件
        for (int i = 0; i < cnt; i++) {
            //定义输出路径缓冲区
            char outpath[512];
            //组合输出目录和文件名形成完整路径
            snprintf(outpath, sizeof(outpath), "%s/%s", outdir, files[i].name);
            FILE* out = fopen(outpath, "wb");
            if (out) fclose(out);
        }
        free(files);
        fclose(fp);
        return 0;
    }

    // 6. 计算所有文件的总原始字节数（用于控制解压结束）
    long long total_size = 0;
    for (int i = 0; i < cnt; i++) {
        total_size += files[i].size;
    }
    long long written_total = 0;   // 已解压出的总字节数

    // 7. 准备输出
    int file_idx = 0;
    long written_current = 0;      // 当前文件已写入字节数
    char outpath[512];
    snprintf(outpath, sizeof(outpath), "%s/%s", outdir, files[0].name);
    FILE* out = fopen(outpath, "wb");
    if (!out) {
        free_tree(root);
        free(files);
        fclose(fp);
        return -1;
    }

    // 8. 简单位读取（由总大小控制停止）
    HuffmanNode* p = root;//设置当前节点为根节点，用于哈夫曼解码遍历
    int ch;// 从压缩文件读取的字节
    int bitpos = -1;// 当前字节已读取的位数，-1 表示需要读新字节
    unsigned char curbyte = 0;//存储当前正在处理的字节

    while (written_total < total_size) {
        // 读取一个位
        if (bitpos < 0) {
            ch = fgetc(fp);
            if (ch == EOF) break;   // 意外结束
            curbyte = (unsigned char)ch;
            bitpos = 7;             // 从最高位开始读
        }
        //从当前字节中取出 bitpos 对应的位（0 或 1）
        int bit = (curbyte >> bitpos) & 1;
        bitpos--;

        // 根据位走向子节点
        p = bit ? p->right : p->left;
        if (!p) break;// 防御：编码错误

        if (!p->left && !p->right) { // 到达叶子
            fputc(p->data, out);
            written_current++;
            written_total++;
            p = root;

            // 当前文件写完
            if (written_current == files[file_idx].size) {
                fclose(out);
                out = NULL;
                file_idx++;
                written_current = 0;
                if (file_idx < cnt) {
                    snprintf(outpath, sizeof(outpath), "%s/%s", outdir, files[file_idx].name);
                    out = fopen(outpath, "wb");
                    if (!out) {
                        free_tree(root);
                        free(files);
                        fclose(fp);
                        return -1;
                    }
                }
            }
        }
    }

    if (out) fclose(out);          // 防止最后文件未关闭
    free_tree(root);
    free(files);
    fclose(fp);
    return 0;
}
