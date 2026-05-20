#include "utils.h"

void timer_start(Timer* t){
    // 把当前系统时间存入结构体的 start 变量
    t->start = clock();
}

double timer_elapsed(Timer* t){
    return (double)(clock() - t->start) / CLOCKS_PER_SEC;
    // 除以 CLOCKS_PER_SEC 变成秒
}

double calc_ratio(long ori, long cmp){
    if(ori <= 0) return 0;
    return (double)cmp / ori * 100.0;
}

long get_file_size(const char* path){
    // 以二进制只读方式打开文件
    FILE* f = fopen(path, "rb");
    // 如果打开失败，返回 0
    if(!f) return 0;
    // 把文件指针移到最后
    fseek(f,0,SEEK_END);
    // 获取当前位置 = 文件大小
    long s = ftell(f);
    // 关闭文件
    fclose(f);
    return s;
}
