# Compiler và flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I../common -g

# Tên file chính
TARGET = lexer_executable

# File objects
# Automatically find source files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Rule mặc định
all: $(TARGET)

# Biên dịch và liên kết chương trình
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Rule biên dịch từng file riêng lẻ
%.o: %.cpp lexer.h ../common/token.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule để dọn dẹp
clean:
	rm -f $(OBJS) $(TARGET) output/*.tokens

# Rule để chạy chương trình sau khi biên dịch
# Example: make run INPUT_FILE=input/my_test.hanami
run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

# Phòng khi có file tên "clean" hoặc "all"
.PHONY: clean all run
