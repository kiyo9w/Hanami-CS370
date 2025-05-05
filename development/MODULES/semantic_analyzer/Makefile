# Makefile for semantic_analyzer directory

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I../common -g

# Object files
OBJS = main.o

# Target executable
TARGET = semantic_analyzer_executable

# Default input file
INPUT_FILE ?= input/input.ast

# Common directory
COMMON_DIR = ../common

# Common objects
COMMON_OBJS = $(COMMON_DIR)/json_deserializer.o $(COMMON_DIR)/utils.o

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    # Convert paths to Windows format
    WIN_OBJS = $(subst /,\,$(OBJS))
    WIN_TARGET = $(subst /,\,$(TARGET))
    WIN_COMMON_OBJS = $(subst /,\,$(COMMON_OBJS))
else
    RM = rm -f
endif

# Rules
all: $(TARGET)

$(TARGET): $(OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Rule to compile object files
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: semananaly.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.ir" $(RM) "..\output\*.ir"
	-if exist "$(WIN_COMMON_OBJS)" $(RM) "$(WIN_COMMON_OBJS)"
else
	$(RM) $(OBJS) $(TARGET) output/*.ir
endif

run: $(TARGET)
	./$(TARGET) $(INPUT_FILE)
	
.PHONY: all clean run
