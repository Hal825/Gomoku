# ============================================================
#  Project: wuzhiqi (五子棋) Server
#  Compiler: g++
# ============================================================

CXX      := g++
TARGET   := server
SRC      := src/main.cpp
OBJ      := $(SRC:.cpp=.o)
DEP      := $(SRC:.cpp=.d)

# ---------- 包含路径 ----------
INCLUDES := -Iinclude -I/usr/include/mysql

# ---------- 编译选项 ----------
CXXFLAGS := -std=c++17 -Wall -Wextra -g -O2 $(INCLUDES)

# ---------- 链接库 ----------
LDFLAGS  := -L/usr/lib/x86_64-linux-gnu
LDLIBS   := -lmysqlclient -lzstd -lssl -lcrypto -lresolv -lm -ljsoncpp -lpthread

# ============================================================
# 构建规则
# ============================================================

.PHONY: all clean rebuild

all: $(TARGET)

# 链接
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@echo "===== 编译完成: $(TARGET) ====="

# 编译（自动生成依赖文件）
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# 引入自动生成的依赖
-include $(DEP)

# 清理
clean:
	rm -f $(TARGET) $(OBJ) $(DEP)
	@echo "===== 清理完成 ====="

# 重新构建
rebuild: clean all
