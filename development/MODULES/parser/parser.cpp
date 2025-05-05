#include "parser.h"
#include <vector>
#include <stdexcept>

// --- Error Handling --- 

void Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[Line " << token.line << ", Col " << token.column << "] Error";

    if (token.type == TokenType::EOF_TOKEN) {
        std::cerr << " at end";
    } else {
        // Optionally include the problematic lexeme for context
         std::cerr << " at '" << token.lexeme << "'";
    }

    std::cerr << ": " << message << std::endl;
    // Throw an exception to unwind the parsing stack. 
    // This allows the caller (like main.cpp) to catch it.
    throw ParseError(message); 
}

// Basic error recovery: advance until a likely statement boundary
// void Parser::synchronize() { ... } // Can optionally remove or comment out this entire function if not used

// --- Helper Methods --- 

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    // Ensure current > 0 before accessing previous
    if (current == 0) {
         // This case should ideally not happen in normal flow after an advance()
         // Handle appropriately, maybe return the first token or throw an error
         throw std::out_of_range("Cannot get previous token at the beginning.");
    }
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    // Check if the current token type is EOF
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

// Checks if the current token is of the given type without consuming it.
bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

// Checks if the current token matches any of the given types.
// If it matches, consumes the token and returns true.
bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

// Consumes the current token if it matches the expected type.
// Throws an error if it doesn't match.
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    error(peek(), message);
    // The error function now throws, so this return is technically unreachable
    // but needed to satisfy the compiler's return type requirement.
    return peek(); // Return dummy token after error
}

// --- Main Parsing Logic --- 

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        try {
             std::cerr << "DEBUG: Parser::parse() loop, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\n";
             auto declaration = parseDeclaration();
             if (declaration) {
                 std::cerr << "DEBUG: Adding statement of type '" << declaration->toJson()["node_type"].get<std::string>() << "' to ProgramNode.\n";
                 program->statements.push_back(std::move(declaration));
             } else {
                 std::cerr << "WARN: parseDeclaration() returned nullptr! Skipping token: " << tokenTypeToString(peek().type) << "\n";
                 if (!isAtEnd()) advance(); // Avoid infinite loop if parseDeclaration fails
             }
        } catch (const ParseError& e) {
             std::cerr << "DEBUG: Caught ParseError in main loop, re-throwing...\\n";
             // synchronize(); // Remove synchronization
             throw; // Re-throw the caught exception
        }
    }
    return program;
}

// --- Grammar Rule Parsers --- 

// declaration -> styleInclude | gardenDeclaration | speciesDeclaration | functionDefinition | variableDeclaration | statement ;
std::unique_ptr<Statement> Parser::parseDeclaration() {
    // Check for STYLE_INCLUDE directly as the lexer now provides it
    if (check(TokenType::STYLE_INCLUDE)) {
        Token pathToken = consume(TokenType::STYLE_INCLUDE, "Internal error: checked STYLE_INCLUDE but failed to consume.");
        match({TokenType::SEMICOLON}); // Optional semicolon
        return std::make_unique<StyleIncludeStmt>(pathToken.lexeme);
    }
    if (match({TokenType::GARDEN})) return parseGardenDeclaration();
    if (match({TokenType::SPECIES})) return parseSpeciesDeclaration();
    if (match({TokenType::GROW})) return parseFunctionDefinition();
    // Variable declarations start with a type (IDENTIFIER)
    // This requires lookahead or careful handling in parseStatement
    // For simplicity, let's parse potential var decls within parseStatement

    return parseStatement();
}

// statement -> exprStmt | branchStmt | ioStmt | returnStmt | blockStmt ;
std::unique_ptr<Statement> Parser::parseStatement() {
    if (match({TokenType::LEFT_BRACE})) return parseBlock();
    if (match({TokenType::BRANCH})) return parseBranchStatement();
    if (match({TokenType::BLOOM, TokenType::WATER})) {
        return parseIOStatement(previous().type); // Pass BLOOM or WATER
    }
    if (match({TokenType::BLOSSOM})) return parseReturnStatement();
    if (match({TokenType::WHILE})) {
        return parseWhileStatement();
    }
    if (match({TokenType::FOR})) {
        return parseForStatement();
    }

    // This handles variable declarations, assignments, and function calls
    return parseVariableDeclarationOrExprStmt(); 
}

// gardenDeclaration -> GARDEN IDENTIFIER SEMICOLON? ;
std::unique_ptr<Statement> Parser::parseGardenDeclaration() {
    // 'garden' token was already consumed
    Token name = consume(TokenType::IDENTIFIER, "Expect garden name.");
    // Semicolon is optional
    match({TokenType::SEMICOLON}); 
    return std::make_unique<GardenDeclStmt>(name.lexeme);
}

// speciesDeclaration -> SPECIES IDENTIFIER LEFT_BRACE (visibilityBlock)* RIGHT_BRACE SEMICOLON? ;
std::unique_ptr<Statement> Parser::parseSpeciesDeclaration() {
    // 'species' token consumed
    Token name = consume(TokenType::IDENTIFIER, "Expect species name.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before species body.");

    auto speciesDecl = std::make_unique<SpeciesDeclStmt>(name.lexeme);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        speciesDecl->sections.push_back(parseVisibilityBlock());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after species body.");
    match({TokenType::SEMICOLON}); // Optional semicolon

    std::cerr << "DEBUG: Returning SpeciesDeclStmt for '" << speciesDecl->name << "' from parseSpeciesDeclaration.\n";
    return speciesDecl;
}

// visibilityBlock -> (OPEN | HIDDEN | GUARDED) SCOPE_RESOLUTION declaration* ;
// Implicitly ends when next visibility keyword or RIGHT_BRACE is encountered.
std::unique_ptr<VisibilityBlockStmt> Parser::parseVisibilityBlock() {
     TokenType visibilityType;
     if (match({TokenType::OPEN})) {
         visibilityType = TokenType::OPEN;
     } else if (match({TokenType::HIDDEN})) {
         visibilityType = TokenType::HIDDEN;
     } else if (match({TokenType::GUARDED})) {
         visibilityType = TokenType::GUARDED;
     } else {
         // This part might be reached if a declaration starts without visibility keyword,
         // handle this based on language grammar (e.g., default visibility or error)
         // For Hanami, assume visibility keyword is mandatory here.
         error(peek(), "Expect 'open', 'hidden', or 'guarded' to start a visibility block.");
         // Return dummy value after throwing
         return std::make_unique<VisibilityBlockStmt>(TokenType::ERROR, nullptr);
     }

     consume(TokenType::COLON, "Expect ':' after visibility keyword.");
     
     auto block = std::make_unique<BlockStmt>();
     std::cerr << "DEBUG: Entering visibilityBlock loop, checking: " << tokenTypeToString(peek().type) << "\n";
     // Loop until }, EOF, or next visibility keyword
     while (!isAtEnd()) {
         // *** Check for terminators BEFORE parsing declaration ***
         if (check(TokenType::RIGHT_BRACE) || 
             check(TokenType::OPEN) || 
             check(TokenType::HIDDEN) || 
             check(TokenType::GUARDED)) {
             std::cerr << "DEBUG: visibilityBlock loop - Found block end/visibility token: " << tokenTypeToString(peek().type) << ". Breaking loop.\n";
             break; // Exit the loop
         }
         
         // If not a terminator, parse the declaration
         std::cerr << "DEBUG: visibilityBlock loop iteration, parsing declaration for token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
         block->statements.push_back(parseDeclaration());
         std::cerr << "DEBUG: visibilityBlock loop after parseDeclaration, next token: " << tokenTypeToString(peek().type) << "\n";
     }
     std::cerr << "DEBUG: Exiting visibilityBlock loop naturally, stopped at token: " << tokenTypeToString(peek().type) << "\n";

     return std::make_unique<VisibilityBlockStmt>(visibilityType, std::move(block));
}


// blockStmt -> LEFT_BRACE declaration* RIGHT_BRACE ;
std::unique_ptr<BlockStmt> Parser::parseBlock() {
    std::cerr << "DEBUG: Entering parseBlock(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\\n";
    auto block = std::make_unique<BlockStmt>();
    std::cerr << "DEBUG: Entering blockStmt loop, checking: " << tokenTypeToString(peek().type) << "\\n";
    // Loop until } or EOF
    while (!isAtEnd()) {
        // *** Check for terminator BEFORE parsing declaration ***
        if (check(TokenType::RIGHT_BRACE)) {
            std::cerr << "DEBUG: blockStmt loop - Found RIGHT_BRACE. Breaking loop.\\n";
            break; // Exit the loop
        }

        // If not }, parse the statement directly
        std::cerr << "DEBUG: blockStmt loop iteration, parsing statement for token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
        block->statements.push_back(parseStatement()); // Call parseStatement directly
        std::cerr << "DEBUG: blockStmt loop after parseStatement, next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
    }
    std::cerr << "DEBUG: Exiting blockStmt loop naturally, stopped at token: " << tokenTypeToString(peek().type) << "\\n";
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    std::cerr << "DEBUG: Exiting parseBlock() after consuming brace\\n";
    return block;
}

// functionDefinition -> GROW IDENTIFIER LEFT_PAREN parameters? RIGHT_PAREN ARROW IDENTIFIER blockStmt ;
// parameters -> IDENTIFIER IDENTIFIER ( COMMA IDENTIFIER IDENTIFIER )*
std::unique_ptr<Statement> Parser::parseFunctionDefinition() {
    // 'grow' token consumed
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");

    std::vector<Parameter> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            // Basic parameter parsing: assumes TYPE NAME
            Token paramType = consume(TokenType::IDENTIFIER, "Expect parameter type.");
            Token paramName = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            parameters.emplace_back(paramType.lexeme, paramName.lexeme);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

    // consume(TokenType::ARROW, "Expect '->' for return type."); // Lexer sends MINUS, STREAM_IN
    consume(TokenType::ARROW, "Expect '->' for function return type arrow.");

    Token returnType = consume(TokenType::IDENTIFIER, "Expect return type identifier (e.g., void, int).");

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    auto body = parseBlock(); // Parse the function body as a block

    auto funcDef = std::make_unique<FunctionDefStmt>(name.lexeme, returnType.lexeme, std::move(body));
    funcDef->parameters = std::move(parameters);
    return funcDef;
}

// Variable declaration or expression statement
// variableDeclaration -> IDENTIFIER IDENTIFIER (ASSIGN expression)? SEMICOLON ;
// expressionStatement -> expression SEMICOLON ;
std::unique_ptr<Statement> Parser::parseVariableDeclarationOrExprStmt() {
    // Lookahead to differentiate variable declaration from expression statement
    // Declaration: TYPE_IDENTIFIER NAME_IDENTIFIER (ASSIGN | SEMICOLON)
    // Need to handle std::string potential: std :: string name ...

    // Check for potential std::string var decl first
    if (check(TokenType::IDENTIFIER) && peek().lexeme == "std" &&
            current + 3 < tokens.size() &&
        tokens[current+1].type == TokenType::SCOPE_RESOLUTION &&
        tokens[current+2].type == TokenType::IDENTIFIER && tokens[current+2].lexeme == "string" &&
        tokens[current+3].type == TokenType::IDENTIFIER) // name
    {
        // Check if ASSIGN or SEMICOLON follows the name
        if (current + 4 < tokens.size() &&
            (tokens[current+4].type == TokenType::ASSIGN || tokens[current+4].type == TokenType::SEMICOLON))
        {
             advance(); // std
             advance(); // ::
             Token typeName = advance(); // string
             Token varName = advance(); // name
             std::string actualTypeName = "string"; // Use simplified type
            
            std::unique_ptr<Expression> initializer = nullptr;
            if (match({TokenType::ASSIGN})) {
                initializer = parseExpression();
            }
             consume(TokenType::SEMICOLON, "Expect ';' after std::string variable declaration.");
            return std::make_unique<VariableDeclStmt>(actualTypeName, varName.lexeme, std::move(initializer));
        }
        // If not followed by ASSIGN/SEMICOLON, let it be parsed as expression (e.g., std::string() call)
    }

    // Check for regular variable declaration: IDENTIFIER IDENTIFIER (ASSIGN | SEMICOLON)
    bool potentialVarDecl = check(TokenType::IDENTIFIER) &&
                            current + 1 < tokens.size() && tokens[current + 1].type == TokenType::IDENTIFIER &&
                            current + 2 < tokens.size() &&
                            (tokens[current + 2].type == TokenType::ASSIGN || tokens[current + 2].type == TokenType::SEMICOLON);

    if (potentialVarDecl)
    {
        Token typeName = advance(); // Consume TYPE
        Token varName = advance();  // Consume NAME
        std::cerr << "DEBUG: Potential VarDecl identified: " << typeName.lexeme << " " << varName.lexeme << ", next: " << tokenTypeToString(peek().type) << "\n";
                    
                    std::unique_ptr<Expression> initializer = nullptr;
                    if (match({TokenType::ASSIGN})) {
             std::cerr << "DEBUG: Parsing initializer for " << varName.lexeme << "...\n";
                        initializer = parseExpression();
             std::cerr << "DEBUG: Finished initializer for " << varName.lexeme << ", next: " << tokenTypeToString(peek().type) << "\n";
                    }
                    
        // Check for semicolon AFTER attempting to parse initializer
        if (check(TokenType::SEMICOLON)) {
            advance(); // Consume SEMICOLON
            std::cerr << "DEBUG: Successfully parsed VarDecl: " << typeName.lexeme << " " << varName.lexeme << "\n";
                    return std::make_unique<VariableDeclStmt>(typeName.lexeme, varName.lexeme, std::move(initializer));
        } else {
            // This case should ideally not happen if ASSIGN was matched
            // If no ASSIGN was matched, SEMICOLON is mandatory
            error(peek(), "Expect ';' after variable declaration.");
            // Need to backtrack if error occurs? For now, error is thrown.
            return nullptr; // Unreachable
        }
    }
    
    // If it doesn't match the variable declaration patterns, parse it as an expression statement.
    std::cerr << "DEBUG: Parsing as expression statement, token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    return parseExpressionStatement();
}



// branchStmt -> BRANCH LEFT_PAREN expression RIGHT_PAREN blockStmt (ELSE BRANCH LEFT_PAREN expression RIGHT_PAREN blockStmt)* (ELSE blockStmt)? ;
std::unique_ptr<Statement> Parser::parseBranchStatement() {
    std::cerr << "DEBUG: ENTERING parseBranchStatement(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
    // 'branch' consumed
    auto branchStmt = std::make_unique<BranchStmt>();

    // First branch (if)
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'branch'.");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after branch condition.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before branch body.");
    std::cerr << "DEBUG: parseBranchStatement() - BEFORE parsing first block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
    auto body = parseBlock();
    std::cerr << "DEBUG: parseBranchStatement() - AFTER parsing first block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
    branchStmt->branches.emplace_back(std::move(condition), std::move(body));

    // Else branch (else if)
    while (check(TokenType::ELSE) && current + 1 < tokens.size() && tokens[current + 1].type == TokenType::BRANCH) {
        advance(); // Consume ELSE
        advance(); // Consume BRANCH
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'else branch'.");
        auto elseIfCondition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after branch condition.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before else branch body.");
        std::cerr << "DEBUG: parseBranchStatement() - BEFORE parsing else-if block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
        auto elseIfBody = parseBlock();
        std::cerr << "DEBUG: parseBranchStatement() - AFTER parsing else-if block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
        branchStmt->branches.emplace_back(std::move(elseIfCondition), std::move(elseIfBody));
    }

    // Else block
    if (match({TokenType::ELSE})) { // This should now correctly find the ELSE if it wasn't consumed above
         consume(TokenType::LEFT_BRACE, "Expect '{' before else body.");
         std::cerr << "DEBUG: parseBranchStatement() - BEFORE parsing final else block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
         auto elseBody = parseBlock();
         std::cerr << "DEBUG: parseBranchStatement() - AFTER parsing final else block, current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
         branchStmt->branches.emplace_back(nullptr, std::move(elseBody));
    }

    std::cerr << "DEBUG: RETURNING from parseBranchStatement(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ") at line " << peek().line << "\\n";
    return branchStmt;
}


// ioStmt -> (BLOOM | WATER) (STREAM_OUT | STREAM_IN) expression ( (STREAM_OUT | STREAM_IN) expression )* SEMICOLON ;
std::unique_ptr<Statement> Parser::parseIOStatement(TokenType ioType) {
    std::cerr << "DEBUG: Entering parseIOStatement for type " << tokenTypeToString(ioType) << ", next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    TokenType direction;
    // Consume the *first* operator
    if (match({TokenType::STREAM_OUT})) {
        direction = TokenType::STREAM_OUT;
        std::cerr << "DEBUG: parseIOStatement matched initial STREAM_OUT, next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    } else if (match({TokenType::STREAM_IN})) {
        direction = TokenType::STREAM_IN;
        std::cerr << "DEBUG: parseIOStatement matched initial STREAM_IN, next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    } else {
        error(peek(), "Expect '<<' or '>>' after bloom/water.");
        return nullptr; // Unreachable
    }

    auto ioStmt = std::make_unique<IOStmt>(ioType, direction);
    
    std::cerr << "DEBUG: parseIOStatement parsing first expression... current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    ioStmt->expressions.push_back(parseExpression());
    std::cerr << "DEBUG: parseIOStatement finished first expression, next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";

    // Loop while subsequent matching operators are found and consumed
    while (match({direction})) { 
        std::cerr << "DEBUG: parseIOStatement matched subsequent " << tokenTypeToString(direction) << ", parsing next expression...\n";
        ioStmt->expressions.push_back(parseExpression());
        std::cerr << "DEBUG: parseIOStatement finished subsequent expression, next token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    } 

    consume(TokenType::SEMICOLON, "Expect ';' after I/O statement.");
    std::cerr << "DEBUG: Exiting parseIOStatement after consuming semicolon.\n";
    return ioStmt;
}


// returnStmt -> BLOSSOM expression? SEMICOLON ;
std::unique_ptr<Statement> Parser::parseReturnStatement() {
    // 'blossom' consumed
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::SEMICOLON)) { // If there's something before the semicolon, parse it as the return value
        value = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after blossom (return) value.");
    std::cerr << "DEBUG: Returning ReturnStmt from parseReturnStatement.\n";
    return std::make_unique<ReturnStmt>(std::move(value));
}

// expressionStatement -> expression SEMICOLON ;
std::unique_ptr<Statement> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    // Removed the extra check here, as parseVariableDeclarationOrExprStmt should now correctly handle declarations.
    return std::make_unique<ExpressionStmt>(std::move(expr));
}


// --- Expression Parsing (Recursive Descent with Precedence) --- 

// expression -> assignment ;
std::unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

// assignment -> ( call "." )? IDENTIFIER ASSIGN assignment | logicalOr ;
// Note: This handles simple assignments like `a = b`, `g.member = c`. 
// It doesn't handle complex left-hand sides like `a[i] = x` yet.
std::unique_ptr<Expression> Parser::parseAssignment() {
     auto expr = parseLogicalOr(); // Parse higher precedence first

     if (match({TokenType::ASSIGN})) {
         Token equals = previous();
         auto value = parseAssignment(); // Right-associative

         // Check if the left side is a valid assignment target (L-value)
         if (IdentifierExpr* identifier = dynamic_cast<IdentifierExpr*>(expr.get())) {
              // Simple variable assignment: a = ...
             // Need to move ownership of name, create new IdentifierExpr for AssignmentStmt
              return std::make_unique<AssignmentStmt>( 
                   std::make_unique<IdentifierExpr>(std::move(identifier->name)),
                   std::move(value));
         } else if (MemberAccessExpr* memberAccess = dynamic_cast<MemberAccessExpr*>(expr.get())){
             // Member assignment: g.member = ...
             // We can directly use the parsed MemberAccessExpr as the left side
             return std::make_unique<AssignmentStmt>(std::move(expr), std::move(value));
         }

         // If the left side is not a valid target
         error(equals, "Invalid assignment target.");
     }

     return expr; // If not an assignment, return the parsed expression
}

// Helper for binary operators - now a static member
std::unique_ptr<Expression> Parser::parseBinaryHelper(
    Parser* parser, // Explicitly pass parser instance
    std::function<std::unique_ptr<Expression>()> parseOperand,
    const std::vector<TokenType>& operators
) {
    std::cerr << "DEBUG: Entering parseBinaryHelper for operators: [ ";
    for(const auto& op : operators) std::cerr << tokenTypeToString(op) << " ";
    std::cerr << "]\n";
    
    auto expr = parseOperand();

    while (true) { // Keep looping as long as matching operators are found
        bool matchedOperator = false;
        for (const TokenType& opType : operators) {
             if (parser->check(opType)) { // Use check() first without consuming
                  std::cerr << "DEBUG: parseBinaryHelper - Found matching operator: " 
                           << tokenTypeToString(opType) 
                           << " at token: " << tokenTypeToString(parser->peek().type) 
                           << " (" << parser->peek().lexeme << ")\n";
                 parser->advance(); // Consume the matched operator
        Token opToken = parser->previous();
        auto right = parseOperand();
        expr = std::make_unique<BinaryOpExpr>(opToken.type, std::move(expr), std::move(right));
                 matchedOperator = true;
                 break; // Exit the inner for loop once an operator is matched and handled
             }
        }
        
        if (!matchedOperator) { 
             std::cerr << "DEBUG: parseBinaryHelper - No more matching operators found. Current token: " 
                      << tokenTypeToString(parser->peek().type) << " (" << parser->peek().lexeme << ")\n";
            break; // Exit the while loop if no operator in the list matched
        }
    }

    std::cerr << "DEBUG: Exiting parseBinaryHelper\n";
    return expr;
}

// logicalOr -> logicalAnd ( OR logicalAnd )* ;
std::unique_ptr<Expression> Parser::parseLogicalOr() {
     return parseBinaryHelper(this, [this]() { return parseLogicalAnd(); }, {TokenType::OR}); // Pass 'this'
}

// logicalAnd -> equality ( AND equality )* ;
std::unique_ptr<Expression> Parser::parseLogicalAnd() {
     return parseBinaryHelper(this, [this]() { return parseEquality(); }, {TokenType::AND}); // Pass 'this'
}

// equality -> comparison ( (NOT_EQUAL | EQUAL) comparison )* ;
std::unique_ptr<Expression> Parser::parseEquality() {
    return parseBinaryHelper(this, [this]() { return parseComparison(); }, {TokenType::NOT_EQUAL, TokenType::EQUAL}); // EQUAL (==) is correct here
}

// comparison -> term ( (GREATER | GREATER_EQUAL | LESS | LESS_EQUAL) term )* ;
std::unique_ptr<Expression> Parser::parseComparison() {
    std::cerr << "DEBUG: Entering parseComparison(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    auto expr = parseBinaryHelper(this, [this]() { return parseTerm(); }, 
        {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}); // This list seems correct based on lexer
    std::cerr << "DEBUG: Exiting parseComparison(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n"; 
    return expr;
}

// term -> factor ( (MINUS | PLUS) factor )* ;
std::unique_ptr<Expression> Parser::parseTerm() {
    std::cerr << "DEBUG: Entering parseTerm(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    auto expr = parseBinaryHelper(this, [this]() { return parseFactor(); }, {TokenType::MINUS, TokenType::PLUS});
    std::cerr << "DEBUG: Exiting parseTerm()\n";
    return expr;
}

// factor -> unary ( (SLASH | STAR | MODULO) unary )* ;
std::unique_ptr<Expression> Parser::parseFactor() {
    std::cerr << "DEBUG: Entering parseFactor(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    auto expr = parseBinaryHelper(this, [this]() { return parseUnary(); }, {TokenType::SLASH, TokenType::STAR, TokenType::MODULO});
    std::cerr << "DEBUG: Exiting parseFactor()\n";
    return expr;
}

// unary -> (NOT | MINUS) unary | call ;
std::unique_ptr<Expression> Parser::parseUnary() {
    std::cerr << "DEBUG: Entering parseUnary(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token opToken = previous();
        auto right = parseUnary();
         error(opToken, "Unary operators not fully implemented yet."); 
         return nullptr; 
    }
    auto expr = parseCall();
    std::cerr << "DEBUG: Exiting parseUnary()\n";
    return expr;
}

// Helper to finish parsing a function call - now a static member
std::unique_ptr<Expression> Parser::finishCall(Parser* parser, std::unique_ptr<Expression> callee) { // Explicitly pass parser instance
    auto callExpr = std::make_unique<FunctionCallExpr>(std::move(callee));

    if (!parser->check(TokenType::RIGHT_PAREN)) {
        do {
            // Handle potential expression limit in arguments
            // if (callExpr->arguments.size() >= MAX_ARGUMENTS) {
            //     parser->error(parser->peek(), "Too many arguments.");
            // }
            callExpr->arguments.push_back(parser->parseExpression());
        } while (parser->match({TokenType::COMMA}));
    }

    parser->consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return callExpr;
}

// call -> primary ( LEFT_PAREN arguments? RIGHT_PAREN | DOT IDENTIFIER )* ;
// arguments -> expression ( COMMA expression )*
std::unique_ptr<Expression> Parser::parseCall() {
    auto expr = parsePrimary();

    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(this, std::move(expr)); // Pass 'this'
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<MemberAccessExpr>(std::move(expr), std::make_unique<IdentifierExpr>(name.lexeme));
        } else {
            break;
        }
    }

    return expr;
}


// primary -> NUMBER | STRING | TRUE | FALSE | IDENTIFIER | LEFT_PAREN expression RIGHT_PAREN ;
// Handle std::string special case
// Ensure FLOAT_LITERAL and DOUBLE_LITERAL are checked
std::unique_ptr<Expression> Parser::parsePrimary() {
    std::cerr << "DEBUG: Entering parsePrimary(), current token: " << tokenTypeToString(peek().type) << " (" << peek().lexeme << ")\n";
    
    if (match({TokenType::FALSE})) { 
        std::cerr << "DEBUG: parsePrimary() matched FALSE\n"; 
        return std::make_unique<BooleanLiteralExpr>(false); 
    }
    if (match({TokenType::TRUE})) { 
        std::cerr << "DEBUG: parsePrimary() matched TRUE\n"; 
        return std::make_unique<BooleanLiteralExpr>(true); 
    }

    if (match({TokenType::NUMBER})) {
        std::cerr << "DEBUG: parsePrimary() matched NUMBER: " << previous().lexeme << "\n";
        return std::make_unique<NumberLiteralExpr>(previous().lexeme);
    }
    if (match({TokenType::FLOAT_LITERAL})) {
        std::cerr << "DEBUG: parsePrimary() matched FLOAT_LITERAL: " << previous().lexeme << "\n";
        return std::make_unique<FloatLiteralExpr>(previous().lexeme);
    }
    if (match({TokenType::DOUBLE_LITERAL})) {
        std::cerr << "DEBUG: parsePrimary() matched DOUBLE_LITERAL: " << previous().lexeme << "\n";
        return std::make_unique<DoubleLiteralExpr>(previous().lexeme);
    }
    if (match({TokenType::STRING})) {
        std::cerr << "DEBUG: parsePrimary() matched STRING: \"" << previous().lexeme << "\"\n";
        return std::make_unique<StringLiteralExpr>(previous().lexeme);
    }

    if (match({TokenType::IDENTIFIER})) {
         std::cerr << "DEBUG: parsePrimary() matched IDENTIFIER: " << previous().lexeme << "\n";
         // Handle `std::string` usage - Check if it was already handled or if it appears here
         if (previous().lexeme == "std" && match({TokenType::SCOPE_RESOLUTION})) {
             Token typeName = consume(TokenType::IDENTIFIER, "Expect type name after 'std::'.");
             if (typeName.lexeme == "string") {
                 std::cerr << "DEBUG: parsePrimary() resolved std::string identifier\n";
                 return std::make_unique<IdentifierExpr>("string");
             }
             std::cerr << "DEBUG: parsePrimary() resolved std::" << typeName.lexeme << " identifier\n";
             return std::make_unique<IdentifierExpr>("std::" + typeName.lexeme);
         }
        return std::make_unique<IdentifierExpr>(previous().lexeme);
    }

    if (match({TokenType::LEFT_PAREN})) {
        std::cerr << "DEBUG: parsePrimary() matched LEFT_PAREN, parsing grouped expression\n";
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        std::cerr << "DEBUG: parsePrimary() finished grouped expression\n";
        return expr; 
    }

    // If none of the above match, it's an error.
    error(peek(), "Expect expression.");
    return nullptr; // Unreachable due to error throw
}

std::unique_ptr<Statement> Parser::parseWhileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after while condition.");
    auto body = parseBlock();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parseForStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

    std::unique_ptr<Statement> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr; // No initializer
    } else if (match({TokenType::IDENTIFIER})) { // Check if it looks like a var decl
         // Need to backtrack or parse more carefully
         // Assuming var decl starts with IDENTIFIER (type)
         // This is simplified - Hanami might not require type keyword here
         current--; // Backtrack IDENTIFIER
         initializer = parseVariableDeclarationOrExprStmt(); // Handles var decl or expr
    } else {
        initializer = parseExpressionStatement(); // Treat as expression
        // Note: parseVariableDeclarationOrExprStmt handles semicolon consumption
        // Need explicit consume here if parseExpressionStatement is used
        // consume(TokenType::SEMICOLON, "Expect ';' after for loop initializer.");
    }

    std::unique_ptr<Expression> condition;
    if (!check(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after for loop condition.");

    std::unique_ptr<Expression> increment;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = parseExpression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    auto body = parseBlock();

    return std::make_unique<ForStmt>(std::move(initializer), std::move(condition), std::move(increment), std::move(body));
}