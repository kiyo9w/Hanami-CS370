# Makefile for parser directory

# Compiler and flags
CXX = g++
# Add -I. to search for nlohmann/json.hpp in the current directory if placed here
# Add -I../common potentially if more common headers are needed
CXXFLAGS = -Wall -std=c++17 -I. -I../common -g # Added -I../common

# JSON library header (assuming it's placed in the same directory or accessible via include paths)
JSON_HPP = nlohmann/json.hpp

# Target executable name
TARGET = parser_executable

# Source files
SRCS = main.cpp parser.cpp

# Object files derived from source files
OBJS = $(SRCS:.cpp=.o)

# Common directory
COMMON_DIR = ../common

# Common objects needed by parser (utils for token strings in AST)
COMMON_OBJS = $(COMMON_DIR)/utils.o 
# Potentially add $(COMMON_DIR)/json_deserializer.o if parser needed to read ASTs

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format if needed
    WIN_OBJS = $(subst /,\,$(OBJS))
    WIN_TARGET = $(subst /,\,$(TARGET))
    WIN_COMMON_OBJS = $(subst /,\,$(COMMON_OBJS))
else
    RM = rm -f
endif

# Default rule: build the target executable
all: $(TARGET)

# Rule to link the target executable (Added COMMON_OBJS)
$(TARGET): $(OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(COMMON_OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
# $< is the first prerequisite (the .cpp file)
# $@ is the target name (the .o file)
# Updated dependencies for common headers
%.o: %.cpp parser.h ../common/token.h ../common/ast.h ../common/utils.h $(JSON_HPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rules might be needed if headers aren't directly in %.cpp
# main.o: main.cpp parser.h ../common/token.h ../common/ast.h
# 	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o
# parser.o: parser.cpp parser.h ../common/token.h ../common/ast.h
# 	$(CXX) $(CXXFLAGS) -c parser.cpp -o parser.o

# Rule to clean up generated files (Added COMMON_OBJS)
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.ast" $(RM) "..\output\*.ast"
	-if exist "$(WIN_COMMON_OBJS)" $(RM) "$(WIN_COMMON_OBJS)"
else
	$(RM) $(OBJS) $(TARGET) ../output/*.ast $(COMMON_OBJS)
endif

# Rule to run the executable (uses default input/output paths)
run: $(TARGET)
	./$(TARGET)

# Phony targets avoid conflicts with actual file names
.PHONY: all clean run
