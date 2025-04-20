#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <functional>

// Include common definitions
#include "../common/token.h" // Needs Token, TokenType
#include "../common/ast.h"   // Needs AST Node definitions (ProgramNode, Statement, Expression, etc.) and ParseError

// --- Parser Class Declaration --- 
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    // Returns the root of the AST, defined in common/ast.h
    std::unique_ptr<ProgramNode> parse(); 

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Helper methods
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);
    void synchronize(); // Error recovery

    // Parsing methods for grammar rules (return types from common/ast.h)
    std::unique_ptr<Statement> parseDeclaration();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseStyleInclude();
    std::unique_ptr<Statement> parseGardenDeclaration();
    std::unique_ptr<Statement> parseSpeciesDeclaration();
    std::unique_ptr<VisibilityBlockStmt> parseVisibilityBlock();
    std::unique_ptr<BlockStmt> parseBlock();
    std::unique_ptr<Statement> parseFunctionDefinition();
    std::unique_ptr<Statement> parseVariableDeclarationOrExprStmt();
    std::unique_ptr<Statement> parseBranchStatement();
    std::unique_ptr<Statement> parseIOStatement(TokenType ioType);
    std::unique_ptr<Statement> parseReturnStatement(); // blossom
    std::unique_ptr<Statement> parseExpressionStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseForStatement();

    // Expression parsing (following operator precedence - types from common/ast.h)
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment(); // Handles assignment (=)
    std::unique_ptr<Expression> parseLogicalOr();  // Handles ||
    std::unique_ptr<Expression> parseLogicalAnd(); // Handles &&
    std::unique_ptr<Expression> parseEquality();   // Handles ==, !=
    std::unique_ptr<Expression> parseComparison(); // Handles <, >, <=, >=
    std::unique_ptr<Expression> parseTerm();       // Handles +, -
    std::unique_ptr<Expression> parseFactor();     // Handles *, /, %
    std::unique_ptr<Expression> parseUnary();      // Handles !, - (unary)
    std::unique_ptr<Expression> parseCall();       // Handles function calls like `expr()` and member access like `expr.member`
    std::unique_ptr<Expression> parsePrimary();    // Handles literals, grouping, identifiers

    // Error reporting (uses ParseError from common/ast.h)
    void error(const Token& token, const std::string& message);

    // Static helper functions
    static std::unique_ptr<Expression> parseBinaryHelper(
        Parser* parser,
        std::function<std::unique_ptr<Expression>()> parseOperand,
        const std::vector<TokenType>& operators
    );
    static std::unique_ptr<Expression> finishCall(Parser* parser, std::unique_ptr<Expression> callee);

};

#endif // PARSER_H 