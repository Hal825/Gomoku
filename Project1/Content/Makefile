# 编译器设置
CXX = g++
CXXFLAGS = -Wall -std=c++11

# 目标文件
TARGET = main
OBJS = main.o

# 默认目标
all: $(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# 编译 main.cc 生成 main.o (注意这里改为 .cc)
main.o: main.cc logger.hpp
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o

# 清理编译生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 声明伪目标
.PHONY: all clean run