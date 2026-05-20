CC=gcc//编译器使用 gcc
CFLAGS=-Wall -O2//编译选项：开启所有警告 + 优化代码
TARGET=huffman//最终生成的可执行程序叫 huffman

//所有源代码文件路径
SRC=src/main.c src/huffman.c src/compress.c src/decompress.c src/utils.c

//默认目标：编译生成 huffman
all: $(TARGET)

//编译命令：把所有 .c 编译成 huffman
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

//清理命令：删除生成的可执行文件
clean:
	rm -f $(TARGET)
