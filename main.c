// 引入标准输入输出库（printf、fopen等函数）
#include <stdio.h>
// 引入字符串处理库（strcmp、strcpy等函数）
#include <string.h>
// 引入标准库（malloc、free等）
#include <stdlib.h>
// 引入文件状态库（判断文件/文件夹是否存在）
#include <sys/stat.h>
// 引入文件类型定义
#include <sys/types.h>
// 引入创建文件夹函数（Windows _mkdir）
#include <direct.h>

#include "compress.h"
#include "decompress.h"
#include "utils.h"

// 创建文件夹（跨平台）
static int make_dir(const char *path) {
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0755);
#endif
}

// 如果文件夹不存在，就创建它
static void mkpath_if_not_exist(const char *dir) {
    struct stat st;
    // stat 查看文件/文件夹状态
    // !=0 表示不存在
    if (stat(dir, &st) != 0) make_dir(dir);
}

// 显示帮助信息（命令提示）
void show_help() {
    printf("Huffman CLI (like zip)\n");
    printf("Compress: huffman out.huf file1 file2 ...\n");
    printf("Decompress: huffman -x out.huf output_dir\n");
    printf("Help: huffman -h\n");
}

// argc = 命令参数个数
// argv = 参数内容数组
int main(int argc, char* argv[]) {
    //命令提示
    if (argc == 2 && !strcmp(argv[1], "-h")) {
        show_help();
        return 0;
    }

    // 解压
    if (argc >= 4 && !strcmp(argv[1], "-x")) {
        const char* arc = argv[2];// 压缩包路径
        const char* out = argv[3];// 输出目录

        mkpath_if_not_exist(out);// 输出目录不存在就创建

        Timer t;
        timer_start(&t);// 开始计时
        int ret = decompress_files(arc, out);// 调用解压函数
        double elapsed = timer_elapsed(&t);// 计算耗时

        if (ret == 0) {
            printf("Decompress success! Time: %.2f s\n", elapsed);
        } else {
            printf("Decompress failed.\n");
        }
        return ret;
    }

    // 压缩
    if (argc >= 3) {
        long total = 0;
        // 遍历所有要压缩的文件，计算总大小
        for (int i = 2; i < argc; i++)
            total += get_file_size(argv[i]);

        Timer t;
        timer_start(&t);// 开始计时
        if (compress_files(&argv[2], argc-2, argv[1]) == 0) {
            double elapsed = timer_elapsed(&t);
            long cmp = get_file_size(argv[1]);// 压缩后大小
            printf("Compress success!\n");
            printf("Original size: %ld bytes\n", total);
            printf("Compressed size: %ld bytes\n", cmp);
            printf("Time: %.2f s\n", elapsed);
            printf("Ratio: %.2f %%\n", calc_ratio(total, cmp));
        } else {
            printf("Compress failed\n");
        }
        return 0;
    }

    show_help();
    return 1;
}
