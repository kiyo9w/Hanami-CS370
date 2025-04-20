#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "token.h" // Needs TokenType

// Helper function declarations
std::string tokenTypeToString(TokenType type);
TokenType stringToTokenType(const std::string& str);

#endif // UTILS_H 