# Makefile for codegen directory

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
ifeq ($(OS),Windows_NT)
	-if exist "*.o" $(RM) *.o
	-if exist "$(WIN_TARGET).exe" $(RM) "$(WIN_TARGET).exe" 
	-if exist "$(WIN_TARGET)" $(RM) "$(WIN_TARGET)"
	-if exist "..\output\*.java" $(RM) "..\output\*.java"
	-if exist "..\output\*.py" $(RM) "..\output\*.py"
	-if exist "..\output\*.cpp" $(RM) "..\output\*.cpp"
	-if exist "..\output\*.js" $(RM) "..\output\*.js"
	-if exist "$(WIN_COMMON_OBJS)" $(RM) "$(WIN_COMMON_OBJS)"
else
	$(RM) $(OBJS) $(TARGET) ../output/*.java ../output/*.py ../output/*.cpp ../output/*.js $(COMMON_OBJS)
endif

# Rule to run the executable (uses default input/output paths)
run: $(TARGET)
	./$(TARGET)

# Phony targets avoid conflicts with actual file names
.PHONY: all clean run
