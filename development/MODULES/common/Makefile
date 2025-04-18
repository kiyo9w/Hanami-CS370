# Makefile for common directory

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -g

# File objects
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format if needed
    WIN_OBJS = $(subst /,\,$(OBJS))
else
    RM = rm -f
endif

# Rule mặc định
all: $(OBJS)

# Rule biên dịch từng file riêng lẻ
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule để dọn dẹp
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
else
	$(RM) $(OBJS)
endif

.PHONY: clean all
