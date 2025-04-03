# Compiler và flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Tên file chính
TARGET = lexer_test

# File objects
OBJS = main.o lexer.o

# Rule mặc định
all: $(TARGET)

# Biên dịch và liên kết chương trình
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule biên dịch từng file riêng lẻ
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

lexer.o: lexer.cpp
	$(CXX) $(CXXFLAGS) -c lexer.cpp -o lexer.o

# Rule để dọn dẹp
clean:
	rm -f $(OBJS) $(TARGET)

# Rule để chạy chương trình sau khi biên dịch
run: $(TARGET)
	./$(TARGET)

# Phòng khi có file tên "clean" hoặc "all"
.PHONY: clean all run
