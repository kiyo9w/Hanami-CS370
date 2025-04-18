# Makefile for lexer directory

# Compiler và flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I../common -g

# Tên file chính
TARGET = lexer_executable

# File objects
# Automatically find source files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format if needed
    WIN_OBJS = $(subst /,\,$(OBJS))
    WIN_TARGET = $(subst /,\,$(TARGET))
else
    RM = rm -f
endif

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
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.tokens" $(RM) "..\output\*.tokens"
else
	$(RM) $(OBJS) $(TARGET) ../output/*.tokens
endif

# Rule để chạy chương trình sau khi biên dịch
# Example: make run INPUT_FILE=input/my_test.hanami
run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

# Phòng khi có file tên "clean" hoặc "all"
.PHONY: clean all run
