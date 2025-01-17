# 编译器
CC = gcc
# 编译选项
#CFLAGS = -pg

# 目标文件列表
OBJS = hapIBD.o read_haphash.o arguments.o sliding_window.o binarySearch.o process_threads.o utils.o get_s_e_pos.o get_base_in_target.o max_block.o phasing_reads.o readbam.o analyze_hap.o

# 最终可执行文件
TARGET = hapIBD


# 默认目标
all: $(TARGET)


# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lpthread


# 编译.c 文件生成.o 文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)
