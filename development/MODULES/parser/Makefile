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
	rm -f $(OBJS) $(TARGET) output/*.ast # Also clean the default output

# Rule to run the executable (uses default input/output paths)
run: $(TARGET)
	./$(TARGET)

# Phony targets avoid conflicts with actual file names
.PHONY: all clean run 