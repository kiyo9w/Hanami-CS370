# Comprehensive Makefile for the compilation pipeline
# Process: lexer -> parser -> semantic analyzer -> codegen

# Common variables and configurations
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./common -g

# Module directories
LEXER_DIR = ./lexer
PARSER_DIR = ./parser
SEMANTIC_DIR = ./semantic_analyzer
CODEGEN_DIR = ./codegen
COMMON_DIR = ./common

# Executables
LEXER_EXEC = $(LEXER_DIR)/lexer_executable
PARSER_EXEC = $(PARSER_DIR)/parser_executable
SEMANTIC_EXEC = $(SEMANTIC_DIR)/semantic_analyzer_executable
CODEGEN_EXEC = $(CODEGEN_DIR)/codegen_executable

# Input/output directories
INPUT_DIR = ./input
OUTPUT_DIR = ./output

# Default input file from command line or use default value
INPUT_FILE ?= $(INPUT_DIR)/test.hanami
OUTPUT_TOKEN_FILE = $(OUTPUT_DIR)/output.tokens
OUTPUT_AST_FILE = $(OUTPUT_DIR)/output.ast
OUTPUT_IR_FILE = $(OUTPUT_DIR)/output.ir

# Detect OS and set appropriate delete command
ifeq ($(OS),Windows_NT)
	RM = del /Q /F
	RMDIR = rmdir /S /Q
	RM_FILES = $(RM)
	# Convert output paths to Windows format
	OUTPUT_WIN = $(subst /,\,$(OUTPUT_DIR))
else
	RM = rm -f
	RMDIR = rm -rf
	RM_FILES = $(RM)
endif

# Default rule
all: build

# Rule to build all modules
build: build_common build_lexer build_parser build_semantic build_codegen

# Build common library first
build_common:
	@echo "Building common modules..."
	$(MAKE) -C $(COMMON_DIR)

# Build lexer
build_lexer: build_common
	@echo "Building lexer..."
	$(MAKE) -C $(LEXER_DIR)

# Build parser
build_parser: build_common
	@echo "Building parser..."
	$(MAKE) -C $(PARSER_DIR)

# Build semantic analyzer
build_semantic: build_common
	@echo "Building semantic analyzer..."
	$(MAKE) -C $(SEMANTIC_DIR)

# Build codegen
build_codegen: build_common
	@echo "Building code generator..."
	$(MAKE) -C $(CODEGEN_DIR)

# Rule to run the entire pipeline
run: build
	@echo "Running full compilation pipeline..."
	@echo "Input file: $(INPUT_FILE)"
	
	@echo "Step 1: Lexical Analysis"
	$(LEXER_EXEC) $(INPUT_FILE) $(OUTPUT_TOKEN_FILE)
	
	@echo "Step 2: Parsing"
	$(PARSER_EXEC) $(OUTPUT_TOKEN_FILE) $(OUTPUT_AST_FILE)
	
	@echo "Step 3: Semantic Analysis" 
	$(SEMANTIC_EXEC) $(OUTPUT_AST_FILE) $(OUTPUT_IR_FILE)
	
	@echo "Step 4: Code Generation"
	$(CODEGEN_EXEC) $(OUTPUT_IR_FILE) $(OUTPUT_DIR)
	
	@echo "Compilation completed successfully!"

# Rule to clean all modules
clean:
	@echo "Cleaning all modules..."
	-$(MAKE) -C $(COMMON_DIR) clean
	-$(MAKE) -C $(LEXER_DIR) clean
	-$(MAKE) -C $(PARSER_DIR) clean
	-$(MAKE) -C $(SEMANTIC_DIR) clean
	-$(MAKE) -C $(CODEGEN_DIR) clean
ifeq ($(OS),Windows_NT)
	-if exist "$(OUTPUT_WIN)\*.tokens" $(RM) "$(OUTPUT_WIN)\*.tokens"
	-if exist "$(OUTPUT_WIN)\*.ast" $(RM) "$(OUTPUT_WIN)\*.ast"  
	-if exist "$(OUTPUT_WIN)\*.ir" $(RM) "$(OUTPUT_WIN)\*.ir"
	-if exist "$(OUTPUT_WIN)\*.java" $(RM) "$(OUTPUT_WIN)\*.java"
	-if exist "$(OUTPUT_WIN)\*.py" $(RM) "$(OUTPUT_WIN)\*.py"
	-if exist "$(OUTPUT_WIN)\*.cpp" $(RM) "$(OUTPUT_WIN)\*.cpp"
	-if exist "$(OUTPUT_WIN)\*.js" $(RM) "$(OUTPUT_WIN)\*.js"
else
	-$(RM_FILES) $(OUTPUT_DIR)/*.tokens $(OUTPUT_DIR)/*.ast $(OUTPUT_DIR)/*.ir
	-$(RM_FILES) $(OUTPUT_DIR)/*.java $(OUTPUT_DIR)/*.py $(OUTPUT_DIR)/*.cpp $(OUTPUT_DIR)/*.js
endif

# Rules to run individual modules
run_lexer: build_lexer
	@echo "Running lexer only..."
	$(LEXER_EXEC) $(INPUT_FILE) $(OUTPUT_TOKEN_FILE)

run_parser: build_parser
	@echo "Running parser only..."
	$(PARSER_EXEC) $(OUTPUT_TOKEN_FILE) $(OUTPUT_AST_FILE)

run_semantic: build_semantic
	@echo "Running semantic analyzer only..."
	$(SEMANTIC_EXEC) $(OUTPUT_AST_FILE) $(OUTPUT_IR_FILE)

run_codegen: build_codegen
	@echo "Running code generator only..."
	$(CODEGEN_EXEC) $(OUTPUT_IR_FILE) $(OUTPUT_DIR)

# To prevent conflicts with files of the same name
.PHONY: all build clean run run_lexer run_parser run_semantic run_codegen
.PHONY: build_common build_lexer build_parser build_semantic build_codegen
