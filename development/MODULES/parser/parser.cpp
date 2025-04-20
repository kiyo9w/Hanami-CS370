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
void Parser::synchronize() {
    advance(); // Consume the token that caused the error

    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return; // End of a statement

        switch (peek().type) {
            // Keywords that often start a new statement/declaration
            case TokenType::SPECIES:
            case TokenType::GROW:
            case TokenType::BRANCH:
            case TokenType::FOR:
            case TokenType::WHILE:
            case TokenType::BLOOM:
            case TokenType::WATER:
            case TokenType::BLOSSOM: // Return keyword
            case TokenType::OPEN:
            case TokenType::HIDDEN:
            case TokenType::GUARDED:
                return;
            default:
                // Keep consuming tokens
                break;
        }
        advance();
    }
}

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
             program->statements.push_back(parseDeclaration());
        } catch (const ParseError& e) {
            // synchronize() attempts to recover and find the next statement
            synchronize();
            // Optionally, you could track errors instead of just printing
            // hasError = true;
        }
       
    }
    return program;
}

// --- Grammar Rule Parsers --- 

// declaration -> styleInclude | gardenDeclaration | speciesDeclaration | functionDefinition | variableDeclaration | statement ;
std::unique_ptr<Statement> Parser::parseDeclaration() {
    if (match({TokenType::STYLE})) return parseStyleInclude();
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

// styleInclude -> STYLE STYLE_INCLUDE SEMICOLON? ;
std::unique_ptr<Statement> Parser::parseStyleInclude() {
    // 'style' token was already consumed by match()
    Token pathToken = consume(TokenType::STYLE_INCLUDE, "Expect include path after 'style'.");
    // Semicolon is optional for style includes based on example
    match({TokenType::SEMICOLON}); 
    return std::make_unique<StyleIncludeStmt>(pathToken.lexeme);
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
    return speciesDecl;
}

// visibilityBlock -> (OPEN | HIDDEN | GUARDED) COLON blockStmt ;
std::unique_ptr<VisibilityBlockStmt> Parser::parseVisibilityBlock() {
     TokenType visibilityType;
     if (match({TokenType::OPEN})) {
         visibilityType = TokenType::OPEN;
     } else if (match({TokenType::HIDDEN})) {
         visibilityType = TokenType::HIDDEN;
     } else if (match({TokenType::GUARDED})) {
         visibilityType = TokenType::GUARDED;
     } else {
         error(peek(), "Expect 'open:', 'hidden:', or 'guarded:'.");
         // Return dummy value after throwing
         return std::make_unique<VisibilityBlockStmt>(TokenType::ERROR, nullptr);
     }

     consume(TokenType::COLON, "Expect ':' after visibility keyword.");
     // Visibility blocks seem to contain multiple declarations/statements, 
     // so we parse a block implicitly here.
     auto block = std::make_unique<BlockStmt>();
     // Parse statements until the next visibility block or the end of the species
     while (!check(TokenType::RIGHT_BRACE) && 
            !check(TokenType::OPEN) && 
            !check(TokenType::HIDDEN) && 
            !check(TokenType::GUARDED) && 
            !isAtEnd()) {
         block->statements.push_back(parseDeclaration());
     }

     return std::make_unique<VisibilityBlockStmt>(visibilityType, std::move(block));
}


// blockStmt -> LEFT_BRACE declaration* RIGHT_BRACE ;
std::unique_ptr<BlockStmt> Parser::parseBlock() {
    // LEFT_BRACE consumed by caller or match()
    auto block = std::make_unique<BlockStmt>();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        block->statements.push_back(parseDeclaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
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

    consume(TokenType::ARROW, "Expect '->' for return type.");
    Token returnType = consume(TokenType::IDENTIFIER, "Expect return type identifier (e.g., void, int).");

    consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
    auto body = parseBlock(); // Parse the function body as a block

    auto funcDef = std::make_unique<FunctionDefStmt>(name.lexeme, returnType.lexeme, std::move(body));
    funcDef->parameters = std::move(parameters);
    return funcDef;
}

// Variable declaration or expression statement
// This is tricky because both can start with an identifier.
// variableDeclaration -> IDENTIFIER IDENTIFIER (ASSIGN expression)? SEMICOLON ;
// expressionStatement -> expression SEMICOLON ;
std::unique_ptr<Statement> Parser::parseVariableDeclarationOrExprStmt() {
    // Peek ahead to differentiate based on the sequence: TYPE NAME [= or ;]
    if (check(TokenType::IDENTIFIER)) {
        // Case 1: Handle std::string structure
        if (peek().lexeme == "std" && 
            current + 3 < tokens.size() &&
            tokens[current + 1].type == TokenType::SCOPE_RESOLUTION &&
            tokens[current + 2].type == TokenType::IDENTIFIER &&
            tokens[current + 3].type == TokenType::IDENTIFIER) {
            
            Token stdToken = advance(); // Consume 'std'
            consume(TokenType::SCOPE_RESOLUTION, "Expect '::'.");
            Token typeNamePart2 = consume(TokenType::IDENTIFIER, "Expect type name after '::'.");
            Token varName = consume(TokenType::IDENTIFIER, "Expect variable name.");
            
            // Important: Use "string" instead of "std::string" for semantic analyzer
            // But keep track of the full type name for future use if needed
            std::string actualTypeName = "string";
            
            std::unique_ptr<Expression> initializer = nullptr;
            if (match({TokenType::ASSIGN})) {
                initializer = parseExpression();
            }
            
            consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
            return std::make_unique<VariableDeclStmt>(actualTypeName, varName.lexeme, std::move(initializer));
        }
        
        // Case 2: Regular variable declaration (type name)
        if (current + 1 < tokens.size() && tokens[current + 1].type == TokenType::IDENTIFIER) {
            // Additional check to see if it's actually a variable declaration
            if (current + 2 < tokens.size()) {
                TokenType thirdTokenType = tokens[current + 2].type;
                if (thirdTokenType == TokenType::ASSIGN || thirdTokenType == TokenType::SEMICOLON) {
                    // Looks like a variable declaration
                    Token typeName = advance(); // Consume type
                    Token varName = advance(); // Consume name
                    
                    std::unique_ptr<Expression> initializer = nullptr;
                    if (match({TokenType::ASSIGN})) {
                        initializer = parseExpression();
                    }
                    
                    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
                    return std::make_unique<VariableDeclStmt>(typeName.lexeme, varName.lexeme, std::move(initializer));
                }
            }
        }
    }
    
    // If it doesn't match the variable declaration pattern, parse it as an expression statement.
    return parseExpressionStatement();
}



// branchStmt -> BRANCH LEFT_PAREN expression RIGHT_PAREN blockStmt (ELSE BRANCH LEFT_PAREN expression RIGHT_PAREN blockStmt)* (ELSE blockStmt)? ;
std::unique_ptr<Statement> Parser::parseBranchStatement() {
    // 'branch' consumed
    auto branchStmt = std::make_unique<BranchStmt>();

    // First branch (if)
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'branch'.");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after branch condition.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before branch body.");
    auto body = parseBlock();
    branchStmt->branches.emplace_back(std::move(condition), std::move(body));

    // Else branch (else if)
    while (match({TokenType::ELSE}) && check(TokenType::BRANCH)) {
        advance(); // Consume 'branch'
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'else branch'.");
        auto elseIfCondition = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after branch condition.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before branch body.");
        auto elseIfBody = parseBlock();
        branchStmt->branches.emplace_back(std::move(elseIfCondition), std::move(elseIfBody));
    }

    // Else block
    if (previous().type == TokenType::ELSE) { // Check if the last match was just 'else'
         consume(TokenType::LEFT_BRACE, "Expect '{' before else body.");
         auto elseBody = parseBlock();
         // Add with null condition for else
         branchStmt->branches.emplace_back(nullptr, std::move(elseBody));
    }

    return branchStmt;
}


// ioStmt -> (BLOOM | WATER) (STREAM_OUT | STREAM_IN) expression ( (STREAM_OUT | STREAM_IN) expression )* SEMICOLON ;
std::unique_ptr<Statement> Parser::parseIOStatement(TokenType ioType) {
    // ioType (BLOOM or WATER) consumed
    TokenType direction;
    if (match({TokenType::STREAM_OUT})) {
        direction = TokenType::STREAM_OUT;
    } else if (match({TokenType::STREAM_IN})) {
        direction = TokenType::STREAM_IN;
    } else {
        error(peek(), "Expect '<<' or '>>' after bloom/water.");
        return nullptr; // Unreachable
    }

    auto ioStmt = std::make_unique<IOStmt>(ioType, direction);
    
    do {
        ioStmt->expressions.push_back(parseExpression());
        // Allow chaining only with the same direction operator
    } while (match({direction})); 

    consume(TokenType::SEMICOLON, "Expect ';' after I/O statement.");
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
    return std::make_unique<ReturnStmt>(std::move(value));
}

// expressionStatement -> expression SEMICOLON ;
std::unique_ptr<Statement> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
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
    auto expr = parseOperand();

    while (parser->match(operators)) {
        Token opToken = parser->previous();
        auto right = parseOperand();
        expr = std::make_unique<BinaryOpExpr>(opToken.type, std::move(expr), std::move(right));
    }

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
    return parseBinaryHelper(this, [this]() { return parseComparison(); }, {TokenType::NOT_EQUAL, TokenType::EQUAL}); // Pass 'this'
}

// comparison -> term ( (GREATER | GREATER_EQUAL | LESS | LESS_EQUAL) term )* ;
std::unique_ptr<Expression> Parser::parseComparison() {
    return parseBinaryHelper(this, [this]() { return parseTerm(); }, 
        {TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}); // Pass 'this'
}

// term -> factor ( (MINUS | PLUS) factor )* ;
std::unique_ptr<Expression> Parser::parseTerm() {
    return parseBinaryHelper(this, [this]() { return parseFactor(); }, {TokenType::MINUS, TokenType::PLUS}); // Pass 'this'
}

// factor -> unary ( (SLASH | STAR | MODULO) unary )* ;
std::unique_ptr<Expression> Parser::parseFactor() {
    return parseBinaryHelper(this, [this]() { return parseUnary(); }, {TokenType::SLASH, TokenType::STAR, TokenType::MODULO}); // Pass 'this'
}

// unary -> (NOT | MINUS) unary | call ;
std::unique_ptr<Expression> Parser::parseUnary() {
    if (match({TokenType::NOT, TokenType::MINUS})) {
        Token opToken = previous();
        auto right = parseUnary();
        // Represent unary as BinaryOpExpr with nullptr left operand for simplicity?
        // Or create a dedicated UnaryOpExpr node.
        // Let's create UnaryOpExpr for clarity (add this to parser.h if needed)
        // For now, using BinaryOpExpr with null left is a hack:
        // return std::make_unique<BinaryOpExpr>(opToken.type, nullptr, std::move(right));
        // Proper UnaryOpExpr would be better.
        // Let's stick to the plan and use BinaryOpExpr for now
        // TODO: Revisit this if UnaryOpExpr is preferred.
         error(opToken, "Unary operators not fully implemented yet."); // Placeholder
         return nullptr; // Should return a UnaryOpExpr ideally
    }
    return parseCall();
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
std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match({TokenType::FALSE})) return std::make_unique<BooleanLiteralExpr>(false);
    if (match({TokenType::TRUE})) return std::make_unique<BooleanLiteralExpr>(true);

    if (match({TokenType::NUMBER})) {
        return std::make_unique<NumberLiteralExpr>(previous().lexeme);
    }
    if (match({TokenType::STRING})) {
        return std::make_unique<StringLiteralExpr>(previous().lexeme);
    }

    if (match({TokenType::IDENTIFIER})) {
         // Handle `std::string` usage
         if (previous().lexeme == "std" && match({TokenType::SCOPE_RESOLUTION})) {
             Token typeName = consume(TokenType::IDENTIFIER, "Expect type name after 'std::'.");
             // This returns an identifier like "std::string" which might be used 
             // in variable declarations or potentially other contexts. 
             // For now, represent it as a single IdentifierExpr.
             return std::make_unique<IdentifierExpr>("std::" + typeName.lexeme);
         }
        return std::make_unique<IdentifierExpr>(previous().lexeme);
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        // Create a GroupingExpr if needed, or just return the inner expression
        // return std::make_unique<GroupingExpr>(std::move(expr)); 
        return expr; // Often grouping is implicit in the AST structure
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