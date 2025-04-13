# Compiler and flags
CXX = g++
# Add include paths for common headers and nlohmann/json
CXXFLAGS = -Wall -std=c++17 -I../common -g 

# Target executable name
TARGET = codegen_executable

# Source files (assuming only main.cpp for now)
SRCS = main.cpp 

# Add common objects to the list
COMMON_OBJS = ../common/json_deserializer.o

# Object files derived from source files
OBJS = $(SRCS:.cpp=.o)

# Default rule: build the target executable
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(COMMON_OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp ../common/ast.h ../common/token.h # Add dependencies
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile common files
../common/json_deserializer.o: ../common/json_deserializer.cpp ../common/json_deserializer.h ../common/ast.h ../common/token.h
	$(CXX) $(CXXFLAGS) -c ../common/json_deserializer.cpp -o ../common/json_deserializer.o

# Rule to clean up generated files
clean:
	rm -f $(OBJS) $(TARGET) output/*.java output/*.py output/*.cpp output/*.js ../common/*.o # Clean common object file

# Rule to run the executable (uses default input/output paths)
run: $(TARGET)
	./$(TARGET)

# Phony targets avoid conflicts with actual file names
.PHONY: all clean run 