/**
 * @brief Contains utility functions for working with tokens
 * 
 */

#ifndef TESTING_UTILS_H
#define TESTING_UTILS_H

#include "enums.h"
#include "scanner.h"

/**
 * @brief Get the name of the token type
 * 
 * @param type Token type
 * @return const char* Name of the token type
 */
const char* getTokenTypeName(TokenType type);

/**
 * @brief Get the name of the keyword
 * 
 * @param keyword Keyword
 * @return const char* Name of the keyword
 */
const char* getKeywordName(Keyword keyword);

/**
 * @brief Print information about the token
 * 
 * @param token Token to print
 */
void printTokenInfo(Token* token);

#endif // TESTING_UTILS_H