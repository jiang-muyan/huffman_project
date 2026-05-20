#ifndef UTILS_H
#define UTILS_H
//防止头文件被重复包含

#include <time.h>// 引入时间库（用于 clock() 计时）
#include <stdio.h>

//定义一个计时器结构体，记录开始时间，计算耗时
typedef struct {
    clock_t start;
} Timer;

//函数声明：开始计时
void timer_start(Timer* t);
//函数声明：计算从开始到现在过了多少秒
double timer_elapsed(Timer* t);
//函数声明：计算压缩比 = 压缩后大小 / 原大小 * 100%
double calc_ratio(long ori, long cmp);
//函数声明：获取文件大小（字节）
long get_file_size(const char* path);

#endif
