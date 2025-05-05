# Makefile for lexer directory

# Compiler và flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I../common -g

# Tên file chính
TARGET = lexer_executable

# Lexer source files
LEXER_SRCS = $(wildcard *.cpp) # lexer.cpp, main.cpp
LEXER_OBJS = $(LEXER_SRCS:.cpp=.o) # lexer.o, main.o

# Common objects needed
COMMON_OBJS = ../common/utils.o

# All object files
OBJS = $(LEXER_OBJS) $(COMMON_OBJS)

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format if needed
    WIN_LEXER_OBJS = $(subst /,\,$(LEXER_OBJS))
    WIN_COMMON_OBJS = $(subst /,\,$(COMMON_OBJS))
    WIN_TARGET = $(subst /,\,$(TARGET))
else
    RM = rm -f
endif

# Rule mặc định
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Rule to compile lexer sources
%.o: %.cpp lexer.h ../common/token.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile common utils
$(COMMON_OBJS): ../common/utils.cpp ../common/utils.h ../common/token.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule để dọn dẹp
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) $(WIN_LEXER_OBJS)
	-if exist ..\common\*.o $(RM) $(WIN_COMMON_OBJS)
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "output\output.tokens" $(RM) "output\output.tokens" # Relative path
else
	$(RM) $(LEXER_OBJS) $(COMMON_OBJS) $(TARGET) output/output.tokens # Use relative output path
endif

# Rule để chạy chương trình sau khi biên dịch
# Example: make run INPUT_FILE=input/my_test.hanami
run: $(TARGET)
	./$(TARGET) $(INPUT_FILE) # Default output is output/output.tokens

# Phòng khi có file tên "clean" hoặc "all"
.PHONY: clean all run
