# Makefile for parser directory

# Compiler and flags
CXX = g++
# Add -I. to search for nlohmann/json.hpp in the current directory if placed here
CXXFLAGS = -Wall -std=c++17 -I. -I../common -g # Added -I. and -I../common

# JSON library header (assuming it's placed in the 'nlohmann' subdirectory)
JSON_HPP = nlohmann/json.hpp

# Target executable name
TARGET = parser_executable

# Source files for the parser module
SRCS = main.cpp parser.cpp

# Object files derived from parser source files
OBJS = $(SRCS:.cpp=.o)

# Common directory
COMMON_DIR = ../common

# Common objects needed by parser (e.g., utils for token strings in AST)
# IMPORTANT: This Makefile *assumes* these object files are already built,
# perhaps by a Makefile in ../common or a top-level Makefile.
# It does NOT contain rules to build them.
COMMON_OBJS = $(COMMON_DIR)/utils.o
# Potentially add $(COMMON_DIR)/json_deserializer.o if parser needed to read ASTs

# Default input file for the 'run' target (use ?= for optional override)
# Example: make run INPUT_FILE=../input/my_test.hanami
INPUT_FILE ?= ../input/default_program.hanami # Adjust default path/name as needed

# --- OS Detection and Windows Specific Variables ---
# These WIN_ variables are primarily for the 'clean' rule if using native 'del'
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format for native commands if needed
    # Note: Often unnecessary if using MinGW/MSYS2/WSL's rm
    WIN_OBJS = $(subst /,\,$(OBJS))
    WIN_TARGET = $(subst /,\,$(TARGET))
    # WIN_COMMON_OBJS = $(subst /,\,$(COMMON_OBJS)) # Not needed if parser doesn't clean common
    OUTPUT_DIR_WIN = ..\output # Windows path for cleaning output
else
    RM = rm -f
    OUTPUT_DIR_UNIX = output# Unix path for cleaning output
endif
# --- End OS Detection ---

# Default rule: build the target executable
all: $(TARGET)

# Rule to link the target executable
# Depends on parser object files ($(OBJS)) and pre-built common object files ($(COMMON_OBJS))
$(TARGET): $(OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(COMMON_OBJS) -o $(TARGET)

# Rule to compile parser's .cpp files into .o files
# $< is the first prerequisite (the .cpp file)
# $@ is the target name (the .o file)
# Depends on relevant headers. Changes to these headers trigger recompilation.
%.o: %.cpp parser.h ../common/token.h ../common/ast.h ../common/utils.h $(JSON_HPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Explicit rules (alternative to generic %.o rule for finer dependency control)
# main.o: main.cpp parser.h ../common/token.h ../common/ast.h
#	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o
# parser.o: parser.cpp parser.h ../common/token.h ../common/ast.h ../common/utils.h $(JSON_HPP)
#	$(CXX) $(CXXFLAGS) -c parser.cpp -o parser.o

# Rule to clean up files generated *by this Makefile*
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) $(WIN_OBJS)
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.ast" $(RM) "..\output\*.ast"
	-if exist "$(WIN_COMMON_OBJS)" $(RM) "$(WIN_COMMON_OBJS)"
else
	$(RM) $(OBJS) $(TARGET) $(OUTPUT_DIR_UNIX)/*.ast
endif

# Rule to run the executable with an input file
# Usage: make run                 (uses default INPUT_FILE)
#        make run INPUT_FILE=<path> (uses specified INPUT_FILE)
run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)

# Phony targets avoid conflicts with actual file names and ensure rules run
.PHONY: all clean run