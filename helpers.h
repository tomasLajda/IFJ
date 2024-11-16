/*
 * IFJ Project
 * @brief Header file for dynamic string
 *
 * @author Tomáš Lajda - xlajdat00
 * @author Matúš Csirik - xcsirim00
 *
 */

#ifndef HELPERS_H

#include "ast.h"
#include "dynamic_string.h"
#include "enums.h"
#include "scanner.h"
#include "stack.h"

/**
 * @brief Function that transforms the token type integer to the corresponding character (string)
 */
char *TokenTypeToString(TokenType type);

/**
 * @brief Function that creates a new token with the given type
 */
Token *createToken(TokenType type);

/**
 * @brief Function that creates a deep copy of the given token
 */
Token *copyToken(Token *token);

/**
 * @brief Function that creates a deep copy of the given ASTNode
 */
ASTNode *copyASTNode(ASTNode *node);

/**
 * @brief Function that creates a new stack element with the given token and ASTNode
 */
StackElement *createStackElement(Token *token, ASTNode *astNodePtr);

/**
 * @brief Function that frees the given token, including its attributes -
 * WARNING: do not attempt to free a token with statically allocated string in keyword attribute
 */
void freeToken(Token *token);

/**
 * @brief Function that checks if the given token is an operator
 * @return 1 if the token is an operator, 0 otherwise
 */
int isOperator(Token *token);

/**
 * @brief Function that checks if the given token is an operand
 * @return 1 if the token is an operand, 0 otherwise
 */
int isOperand(Token *token);

/**
 * @brief Function that checks if the given token is a parentheses
 * @return 1 if the token is a parentheses, 0 otherwise
 */
int isParentheses(Token *token);

/**
 * @brief Function that checks if the given token is a delimiter
 * @return 1 if the token is a delimiter, 0 otherwise
 */
int isDelimiter(Token *token);

#endif // HELPERS_H