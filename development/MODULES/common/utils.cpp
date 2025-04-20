#include "utils.h"
#include <unordered_map>
#include <stdexcept>

// Helper function definitions

std::string tokenTypeToString(TokenType type) {
    // ... (Keep the existing switch statement implementation) ...
    switch (type) {
        case TokenType::GARDEN: return "GARDEN";
        case TokenType::SPECIES: return "SPECIES";
        case TokenType::OPEN: return "OPEN";
        case TokenType::HIDDEN: return "HIDDEN";
        case TokenType::GUARDED: return "GUARDED";
        case TokenType::GROW: return "GROW";
        case TokenType::BLOSSOM: return "BLOSSOM";
        case TokenType::STYLE: return "STYLE";
        case TokenType::BLOOM: return "BLOOM";
        case TokenType::WATER: return "WATER";
        case TokenType::BRANCH: return "BRANCH";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::FOR: return "FOR";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::MODULO: return "MODULO";
        case TokenType::STREAM_OUT: return "STREAM_OUT";
        case TokenType::STREAM_IN: return "STREAM_IN";
        case TokenType::ARROW: return "ARROW";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::ERROR: return "ERROR";
        case TokenType::STYLE_INCLUDE: return "STYLE_INCLUDE";
        case TokenType::SCOPE_RESOLUTION: return "SCOPE_RESOLUTION";
        default: return "UNKNOWN"; // Or throw an error for unknown types
    }
}

TokenType stringToTokenType(const std::string& str) {
    // ... (Keep the existing map-based implementation) ...
    // Use static to initialize the map only once
    static const std::unordered_map<std::string, TokenType> map = {
        {"GARDEN", TokenType::GARDEN}, {"SPECIES", TokenType::SPECIES},
        {"OPEN", TokenType::OPEN}, {"HIDDEN", TokenType::HIDDEN},
        {"GUARDED", TokenType::GUARDED}, {"GROW", TokenType::GROW},
        {"BLOSSOM", TokenType::BLOSSOM}, {"STYLE", TokenType::STYLE},
        {"BLOOM", TokenType::BLOOM}, {"WATER", TokenType::WATER},
        {"BRANCH", TokenType::BRANCH}, {"ELSE", TokenType::ELSE},
        {"WHILE", TokenType::WHILE}, {"FOR", TokenType::FOR},
        {"IDENTIFIER", TokenType::IDENTIFIER}, {"NUMBER", TokenType::NUMBER},
        {"STRING", TokenType::STRING}, {"TRUE", TokenType::TRUE},
        {"FALSE", TokenType::FALSE}, {"PLUS", TokenType::PLUS},
        {"MINUS", TokenType::MINUS}, {"STAR", TokenType::STAR},
        {"SLASH", TokenType::SLASH}, {"ASSIGN", TokenType::ASSIGN},
        {"EQUAL", TokenType::EQUAL}, {"NOT_EQUAL", TokenType::NOT_EQUAL},
        {"LESS", TokenType::LESS}, {"LESS_EQUAL", TokenType::LESS_EQUAL},
        {"GREATER", TokenType::GREATER}, {"GREATER_EQUAL", TokenType::GREATER_EQUAL},
        {"AND", TokenType::AND}, {"OR", TokenType::OR}, {"NOT", TokenType::NOT},
        {"MODULO", TokenType::MODULO},
        {"STREAM_OUT", TokenType::STREAM_OUT}, {"STREAM_IN", TokenType::STREAM_IN},
        {"ARROW", TokenType::ARROW},
        {"LEFT_PAREN", TokenType::LEFT_PAREN}, {"RIGHT_PAREN", TokenType::RIGHT_PAREN},
        {"LEFT_BRACE", TokenType::LEFT_BRACE}, {"RIGHT_BRACE", TokenType::RIGHT_BRACE},
        {"LEFT_BRACKET", TokenType::LEFT_BRACKET}, {"RIGHT_BRACKET", TokenType::RIGHT_BRACKET},
        {"COMMA", TokenType::COMMA}, {"SEMICOLON", TokenType::SEMICOLON},
        {"DOT", TokenType::DOT}, {"COLON", TokenType::COLON},
        {"COMMENT", TokenType::COMMENT}, {"EOF_TOKEN", TokenType::EOF_TOKEN},
        {"ERROR", TokenType::ERROR},
        {"STYLE_INCLUDE", TokenType::STYLE_INCLUDE}, {"SCOPE_RESOLUTION", TokenType::SCOPE_RESOLUTION}
    };

    auto it = map.find(str);
    if (it != map.end()) {
        return it->second;
    } 
    // Throw an exception for unknown strings, as this likely indicates an AST format error
    // or an incomplete mapping.
    throw std::runtime_error("Unknown token type string encountered during conversion: " + str);
}
