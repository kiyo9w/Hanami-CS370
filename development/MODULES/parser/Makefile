# Makefile for parser directory

# Compiler and flags
CXX = g++
# Add -I. to search for nlohmann/json.hpp in the current directory if placed here
# Add -I../common potentially if more common headers are needed
CXXFLAGS = -Wall -std=c++17 -I. -g # Using C++17 for convenience, -g for debugging

# JSON library header (assuming it's placed in the same directory or accessible via include paths)
JSON_HPP = nlohmann/json.hpp

# Target executable name
TARGET = parser_executable

# Source files
SRCS = main.cpp parser.cpp

# Object files derived from source files
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

# Default rule: build the target executable
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
# $< is the first prerequisite (the .cpp file)
# $@ is the target name (the .o file)
%.o: %.cpp parser.h ../common/token.h $(JSON_HPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up generated files
clean:
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe"
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.ast" $(RM) "..\output\*.ast"
else
	$(RM) $(OBJS) $(TARGET) ../output/*.ast
endif

# Rule to run the executable (uses default input/output paths)
run: $(TARGET)
	./$(TARGET)

# Phony targets avoid conflicts with actual file names
.PHONY: all clean run
